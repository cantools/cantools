import binascii
import datetime
import enum
import re
import abc
import io
import typing
from collections.abc import Iterator

TimestampType: 'typing.TypeAlias' = 'datetime.datetime|datetime.timedelta|None'

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
                 is_extended_frame: bool,
                 data: bytes,
                 timestamp: 'TimestampType',
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
        self.is_extended_frame = is_extended_frame
        self.data = bytes(data)
        self.timestamp = timestamp
        self.timestamp_format = timestamp_format


class BasePattern:

    pattern: 're.Pattern[str]'

    def match(self, line: str) -> 'DataFrame|None':
        mo = self.pattern.match(line)
        if mo:
            return self.unpack(mo)

        return None

    @abc.abstractmethod
    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        raise NotImplementedError()


class CandumpBasePattern(BasePattern):

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        channel = match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 3
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        timestamp, timestamp_format = self.parse_timestamp(match_object)

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)

    @abc.abstractmethod
    def parse_timestamp(self, match_object: 're.Match[str]') -> 'tuple[TimestampType, TimestampFormat]':
        raise NotImplementedError()


class CandumpDefaultPattern(CandumpBasePattern):
    #candump vcan0
    # vcan0  1F0   [8]  00 00 00 00 00 00 1B C1
    #candump vcan0 -a
    # vcan0  1F0   [8]  00 00 00 00 00 00 1B C1   '.......Á'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    def parse_timestamp(self, match_object: 're.Match[str]') -> 'tuple[TimestampType, TimestampFormat]':
        timestamp = None
        timestamp_format = TimestampFormat.MISSING
        return timestamp, timestamp_format


class CandumpTimestampedPattern(CandumpBasePattern):
    #candump vcan0 -tz
    # (000.000000)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
    #candump vcan0 -tz -a
    # (000.000000)  vcan0  0C8   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>[\d.]+)\)\s+(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    def __init__(self, tz: 'datetime.tzinfo|None') -> None:
        self.tz = tz

    def parse_timestamp(self, match_object: 're.Match[str]') -> 'tuple[TimestampType, TimestampFormat]':
        seconds = float(match_object.group('timestamp'))
        timestamp: 'datetime.timedelta|datetime.datetime'
        if seconds < 662688000:  # 1991-01-01 00:00:00, "Released in 1991, the Mercedes-Benz W140 was the first production vehicle to feature a CAN-based multiplex wiring system."
            timestamp = datetime.timedelta(seconds=seconds)
            timestamp_format = TimestampFormat.RELATIVE
        else:
            timestamp = datetime.datetime.fromtimestamp(seconds, self.tz)
            timestamp_format = TimestampFormat.ABSOLUTE

        return timestamp, timestamp_format


class CandumpDefaultLogPattern(CandumpBasePattern):
    # (1579857014.345944) can2 486#82967A6B006B07F8
    # (1613656104.501098) can2 14C##16A0FFE00606E022400000000000000A0FFFF00FFFF25000600000000000000FE
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>[\d.]+?)\)\s+?(?P<channel>[a-zA-Z0-9]+)\s+?(?P<can_id>[0-9A-F]+?)#(#[0-9A-F])?(?P<can_data>([0-9A-Fa-f]{2})*)(\s+[RT])?$')

    def __init__(self, tz: 'datetime.tzinfo|None') -> None:
        self.tz = tz

    def parse_timestamp(self, match_object: 're.Match[str]') -> 'tuple[TimestampType, TimestampFormat]':
        timestamp = datetime.datetime.fromtimestamp(float(match_object.group('timestamp')), self.tz)
        timestamp_format = TimestampFormat.ABSOLUTE
        return timestamp, timestamp_format


class CandumpAbsoluteLogPattern(CandumpBasePattern):
    #candump vcan0 -tA
    # (2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
    #candump vcan0 -tA -a
    # (2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'
    #(Ignore anything after the end of the data to work with candump's ASCII decoding)
    pattern = re.compile(
        r'^\s*?\((?P<timestamp>\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d+)\)\s+(?P<channel>[a-zA-Z0-9]+)\s+(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*).*?$')

    def parse_timestamp(self, match_object: 're.Match[str]') -> 'tuple[TimestampType, TimestampFormat]':
        timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp_format = TimestampFormat.ABSOLUTE
        return timestamp, timestamp_format


class PCANTracePatternV10(BasePattern):
    """
    Reference for PCAN trace patterns: https://www.peak-system.com/produktcd/Pdf/English/PEAK_CAN_TRC_File_Format.pdf
    1) 1841 0001 8 00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+)\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV10().match(" 1) 1841 0001 8 00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV11(BasePattern):
    """
    Adds "Type" 'Rx' column to 1.0 and 1/10 microsecond resolution
    1)      6357.2 Rx        0401  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+.+\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV11().match("  1)      6357.2  Rx        0401  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV12(BasePattern):
    """
    Adds "Bus" column and 1 microsecond resolution to 1.1
    1)      6357.213 1  Rx        0401  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+(?P<channel>[0-9])\s+.+\s+(?P<can_id>[0-9A-F]+)\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV12().match("  1)      6357.213 1  Rx        0401  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV13(BasePattern):
    """
    Adds "Reserved" '-' column to 1.2
    1)      6357.213 1  Rx        0401 -  8    00 00 00 00 00 00 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+\)\s*?(?P<timestamp>\d+.\d+)\s+(?P<channel>[0-9])\s+.+\s+(?P<can_id>[0-9A-F]+)\s+-\s+(?P<dlc>[0-9])\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV13().match("  1)      6357.213 1  Rx        0401 -  8    00 00 00 00 00 00 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV20(BasePattern):
    """
     1      1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+?\s*?(?P<timestamp>\d+.\d+)\s+(?P<type>\w+)\s+(?P<can_id>[0-9A-F]+)\s+(?P<rxtx>\w+)\s+(?P<dlc>[0-9]+)\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV20().match(" 1      1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcanx'
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class PCANTracePatternV21(BasePattern):
    """
    "Reserved" '-' and "Bus" to 2.0
     1      1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00
    """
    pattern = re.compile(
        r'^\s*?\d+?\s*?(?P<timestamp>\d+.\d+)\s+(?P<type>.+)\s+(?P<channel>[0-9])\s+(?P<can_id>[0-9A-F]+)\s+(?P<rxtx>.+)\s+-\s+(?P<dlc>[0-9]+)\s+(?P<can_data>[0-9A-F ]*)$')

    def unpack(self, match_object: 're.Match[str]') -> DataFrame:
        """
        >>> PCANTracePatternV21().match(" 1      1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00") #doctest: +ELLIPSIS
        <logreader.DataFrame object at ...>
        """
        channel = 'pcan' + match_object.group('channel')
        frame_id = int(match_object.group('can_id'), 16)
        is_extended_frame = len(match_object.group('can_id')) > 4
        data = match_object.group('can_data')
        data = data.replace(' ', '')
        data = binascii.unhexlify(data)
        millis = float(match_object.group('timestamp'))
        # timestamp = datetime.datetime.strptime(match_object.group('timestamp'), "%Y-%m-%d %H:%M:%S.%f")
        timestamp = datetime.timedelta(milliseconds=millis)
        timestamp_format = TimestampFormat.RELATIVE

        return DataFrame(channel=channel, frame_id=frame_id, is_extended_frame=is_extended_frame, data=data, timestamp=timestamp, timestamp_format=timestamp_format)


class Parser:
    """A CAN log file parser.

    Automatically detects the format of the logfile by trying parser patterns
    until the first successful match.

    >>> with open('candump.log') as fd: #doctest: +SKIP
            for frame in cantools.logreader.Parser(fd):
                print(f'{frame.timestamp}: {frame.frame_id}')
    """

    def __init__(self, stream: 'io.TextIOBase|None' = None, *, tz: 'datetime.tzinfo|None' = None) -> None:
        self.stream = stream
        self.pattern: 'BasePattern|None' = None
        self.tz = tz

    def detect_pattern(self, line: str) -> 'BasePattern|None':
        for p in [CandumpDefaultPattern(), CandumpTimestampedPattern(self.tz), CandumpDefaultLogPattern(self.tz), CandumpAbsoluteLogPattern(), PCANTracePatternV21(), PCANTracePatternV20(), PCANTracePatternV13(), PCANTracePatternV12(), PCANTracePatternV11(), PCANTracePatternV10()]:
            mo = p.pattern.match(line)
            if mo:
                return p

        return None

    def parse(self, line: str) -> 'DataFrame|None':
        if self.pattern is None:
            self.pattern = self.detect_pattern(line)
        if self.pattern is None:
            return None
        return self.pattern.match(line)

    def iterlines(self, keep_unknowns: bool = False) -> 'Iterator[tuple[str, DataFrame|None]]':
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

    def __iter__(self) -> 'Iterator[DataFrame|None]':
        """Returns DataFrame log entries. Non-parseable log entries is
        discarded."""
        for _, frame in self.iterlines():
            yield frame
