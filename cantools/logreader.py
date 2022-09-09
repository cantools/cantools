import re
import enum
import binascii
import datetime


class TimestampFormat(enum.Enum):
    """Describes a type of timestamp. ABSOLUTE is referring to UNIX time
    (seconds since epoch). RELATIVE is seconds since start of log, or time
    since last frame depending of the contents of the log file. MISSING means
    that no timestamps is present in the log."""
    ABSOLUTE = 1
    RELATIVE = 2
    MISSING = 3


class DataFrame:
    """Container for a parsed log entry (ie. a CAN frame)."""

    def __init__(self, channel: str,
                 frame_id: int,
                 data: bytes,
                 timestamp: datetime.datetime,
                 timestamp_format: TimestampFormat):
        """Constructor for DataFrame

        :param channel: A string representation of the channel, eg. 'can0'
        :param frame_id: The numeric CAN frame ID :param data: The actual data
        :param timestamp: A timestamp, datetime.datetime if absolute, or
            datetime.timedelta if relative, None if missing
        :param timestamp_format: The format of the timestamp
        : """
        self.channel = channel
        self.frame_id = frame_id
        self.data = data
        self.timestamp = timestamp
        self.timestamp_format = timestamp_format


class BasePattern:
    @classmethod
    def match(clz, line):
        mo = clz.pattern.match(line)
        if mo:
            return clz.unpack(mo)


class CandumpDefaultPattern(BasePattern):
    #candump vcan0
    # vcan0  1F0   [8]  00 00 00 00 00 00 1B C1
    #candump vcan0 -a
    # vcan0  1F0   [8]  00 00 00 00 00 00 1B C1   '.......Á'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    @staticmethod
    def unpack(match_object):
        channel = match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        timestamp = None
        timestamp_format = TimestampFormat.MISSING

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class CandumpTimestampedPattern(BasePattern):
    #candump vcan0 -tz
    # (000.000000)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
    #candump vcan0 -tz -a
    # (000.000000)  vcan0  0C8   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>[\d.]+)\)\s+(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    @staticmethod
    def unpack(match_object):
        channel = match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)

        seconds = float(match_object.group('timestamp'))
        if seconds < 662688000:  # 1991-01-01 00:00:00, "Released in 1991, the Mercedes-Benz W140 was the first production vehicle to feature a CAN-based multiplex wiring system."
            timestamp = datetime.timedelta(seconds=seconds)
            timestamp_format = TimestampFormat.RELATIVE
        else:
            timestamp = datetime.datetime.utcfromtimestamp(seconds)
            timestamp_format = TimestampFormat.ABSOLUTE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class CandumpDefaultLogPattern(BasePattern):
    # (1579857014.345944) can2 486#82967A6B006B07F8
    # (1613656104.501098) can2 14C##16A0FFE00606E022400000000000000A0FFFF00FFFF25000600000000000000FE
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>[\d.]+?)\)\s+?(?P<channel>[a-zA-Z0-9]+)\s+?(?P<can_id>[0-9A-F]+?)#(#[0-9A-F])?(?P<can_data>([0-9A-Fa-f]{2})*?$).*?$')

    @staticmethod
    def unpack(match_object):
        channel = match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        timestamp = datetime.datetime.utcfromtimestamp(float(match_object.group('timestamp')))
        timestamp_format = TimestampFormat.ABSOLUTE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class CandumpAbsoluteLogPattern(BasePattern):
    #candump vcan0 -tA
    # (2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
    #candump vcan0 -tA -a
    # (2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+)\)\s+(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    @staticmethod
    def unpack(match_object):
        channel = match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp_format = TimestampFormat.ABSOLUTE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV10(BasePattern):
    """
    Reference for PCAN trace patterns: https://www.peak-system.com/produktcd/Pdf/English/PEAK_CAN_TRC_File_Format.pdf
    1) 1841 0001 8 00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+)\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV10().match(" 1) 1841 0001 8 00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV11(BasePattern):
    """
    Adds "Type" 'Rx' column to 1.0 and 1/10 microsecond resolution
    1)      6357.2 Rx        0401  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+.+\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV11().match("  1)      6357.2  Rx        0401  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV12(BasePattern):
    """
    Adds "Bus" column and 1 microsecond resolution to 1.1
    1)      6357.213 1  Rx        0401  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+(?P<channel>[0-9])\s+.+\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV12().match("  1)      6357.213 1  Rx        0401  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV13(BasePattern):
    """
    Adds "Reserved" '-' column to 1.2
    1)      6357.213 1  Rx        0401 -  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+(?P<channel>[0-9])\s+.+\s+(?P<can_id>[0-9A-F]+)\s+-\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV13().match("  1)      6357.213 1  Rx        0401 -  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV20(BasePattern):
    """
     1      1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+?\s*?(?P<timestamp>\d+.\d+)\s+(?P<type>\w+)\s+(?P<can_id>[0-9A-F]+)\s+(?P<rxtx>\w+)\s+(?P<dlc>[0-9]+)\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV20().match(" 1      1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV21(BasePattern):
    """
    "Reserved" '-' and "Bus" to 2.0
     1      1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+?\s*?(?P<timestamp>\d+.\d+)\s+(?P<type>.+)\s+(?P<channel>[0-9])\s+(?P<can_id>[0-9A-F]+)\s+(?P<rxtx>.+)\s+-\s+(?P<dlc>[0-9]+)\s+(?P<can_data>[0-9A-F ]*)$')

    @staticmethod
    def unpack(match_object):
        """
        >>> PCANTracePatternV21().match(" 1      1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class Parser:
    """A CAN log file parser.

    Automatically detects the format of the logfile by trying parser patterns
    until the first successful match.

    >>> with open('candump.log') as fd: #doctest: +SKIP
            for frame in cantools.logreader.Parser(fd):
                print(f'{frame.timestamp}: {frame.frame_id}')
    """

    def __init__(self, stream=None):
        self.stream = stream
        self.pattern = None

    @staticmethod
    def detect_pattern(line):
        for p in [CandumpDefaultPattern, CandumpTimestampedPattern, CandumpDefaultLogPattern, CandumpAbsoluteLogPattern, PCANTracePatternV21, PCANTracePatternV20, PCANTracePatternV13, PCANTracePatternV12, PCANTracePatternV11, PCANTracePatternV10]:
            mo = p.pattern.match(line)
            if mo:
                return p

    def parse(self, line):
        if self.pattern is None:
            self.pattern = self.detect_pattern(line)
        if self.pattern is None:
            return None
        return self.pattern.match(line)

    def iterlines(self, keep_unknowns=False):
        """Returns an generator that yields (str, DataFrame) tuples with the
        raw log entry and a parsed log entry. If keep_unknowns=True, (str,
        None) tuples will be returned for log entries that couldn't be decoded.
        If keep_unknowns=False, non-parseable log entries is discarded.
        """
        if self.stream is None:
            return
        while True:
            nl = self.stream.readline()
            if nl == '':
                return
            nl = nl.strip('\r\n')
            frame = self.parse(nl)
            if frame:
                yield nl, frame
            elif keep_unknowns:
                yield nl, None
            else:
                continue

    def __iter__(self):
        """Returns DataFrame log entries. Non-parseable log entries is
        discarded."""
        for _, frame in self.iterlines():
            yield frame
