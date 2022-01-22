# A CAN message.

import binascii
from copy import deepcopy
from typing import List, Optional, Union, Dict, TYPE_CHECKING, Set

from .signal import NamedSignalValue, Signal
from .signal_group import SignalGroup
from ..utils import format_or
from ..utils import start_bit
from ..utils import encode_data
from ..utils import decode_data
from ..utils import create_encode_decode_formats
from ..utils import type_sort_signals
from ..utils import sort_signals_by_start_bit
from ..utils import SORT_SIGNALS_DEFAULT
from ..errors import Error
from ..errors import EncodeError
from ..errors import DecodeError
from ...typechecking import Comments, Codec

if TYPE_CHECKING:
    from .formats.arxml import AutosarMessageSpecifics
    from .formats.dbc import DbcSpecifics


class Message(object):
    """A CAN message with frame id, comment, signals and other
    information.

    If `strict` is ``True`` an exception is raised if any signals are
    overlapping or if they don't fit in the message.

    By default signals are sorted by their start bit when their Message object is created.
    If you don't want them to be sorted pass `sort_signals = None`.
    If you want the signals to be sorted in another way pass something like
    `sort_signals = lambda signals: list(sorted(signals, key=lambda sig: sig.name))`
    """

    def __init__(self,
                 frame_id: int,
                 name: str,
                 length: int,
                 signals: List[Signal],
                 unused_bit_pattern: int = 0x00,
                 comment: Optional[Union[str, Comments]] = None,
                 senders: Optional[List[str]] = None,
                 send_type: Optional[str] = None,
                 cycle_time: Optional[int] = None,
                 dbc_specifics: Optional["DbcSpecifics"] = None,
                 autosar_specifics: Optional["AutosarMessageSpecifics"] = None,
                 is_extended_frame: bool = False,
                 bus_name: Optional[str] = None,
                 signal_groups: Optional[List[SignalGroup]] = None,
                 strict: bool = True,
                 protocol: Optional[str] = None,
                 sort_signals: type_sort_signals = sort_signals_by_start_bit,
                 ) -> None:
        frame_id_bit_length = frame_id.bit_length()

        if is_extended_frame:
            if frame_id_bit_length > 29:
                raise Error(
                    'Extended frame id 0x{:x} is more than 29 bits in '
                    'message {}.'.format(frame_id, name))
        elif frame_id_bit_length > 11:
            raise Error(
                'Standard frame id 0x{:x} is more than 11 bits in '
                'message {}.'.format(frame_id, name))

        self._frame_id = frame_id
        self._is_extended_frame = is_extended_frame
        self._name = name
        self._length = length
        self._unused_bit_pattern = unused_bit_pattern
        if sort_signals == SORT_SIGNALS_DEFAULT:
            self._signals = sort_signals_by_start_bit(signals)
        elif sort_signals:
            self._signals = sort_signals(signals)
        else:
            self._signals = signals


        # if the 'comment' argument is a string, we assume that is an
        # english comment. this is slightly hacky because the
        # function's behavior depends on the type of the passed
        # argument, but it is quite convenient...
        self._comments: Optional[Comments]
        if isinstance(comment, str):
            # use the first comment in the dictionary as "The" comment
            self._comments = {None: comment}
        else:
            # assume that we have either no comment at all or a
            # multi-lingual dictionary
            self._comments = comment

        self._senders = senders if senders else []
        self._send_type = send_type
        self._cycle_time = cycle_time
        self._dbc = dbc_specifics
        self._autosar = autosar_specifics
        self._bus_name = bus_name
        self._signal_groups = signal_groups
        self._codecs: Optional[Codec] = None
        self._signal_tree = None
        self._strict = strict
        self._protocol = protocol
        self.refresh()

    def _create_codec(self,
                      parent_signal: Optional[str] = None,
                      multiplexer_id: Optional[int] = None,
                      ) -> Codec:
        """Create a codec of all signals with given parent signal. This is a
        recursive function.

        """

        signals = []
        multiplexers: Dict[str, Dict[int, Codec]] = {}

        # Find all signals matching given parent signal name and given
        # multiplexer id. Root signals' parent and multiplexer id are
        # both None.
        for signal in self._signals:
            if signal.multiplexer_signal != parent_signal:
                continue

            if (
                    multiplexer_id is not None
                    and (signal.multiplexer_ids is None or multiplexer_id not in signal.multiplexer_ids)
            ):
                continue

            if signal.is_multiplexer:
                children_ids: Set[int] = set()

                for s in self._signals:
                    if s.multiplexer_signal != signal.name:
                        continue

                    if s.multiplexer_ids is not None:
                        children_ids.update(s.multiplexer_ids)

                # Some CAN messages will have muxes containing only
                # the multiplexer and no additional signals. At Tesla
                # these are indicated in advance by assigning them an
                # enumeration. Here we ensure that any named
                # multiplexer is included, even if it has no child
                # signals.
                if signal.choices:
                    children_ids.update(signal.choices.keys())

                for child_id in children_ids:
                    codec = self._create_codec(signal.name, child_id)

                    if signal.name not in multiplexers:
                        multiplexers[signal.name] = {}

                    multiplexers[signal.name][child_id] = codec

            signals.append(signal)

        return {
            'signals': signals,
            'formats': create_encode_decode_formats(signals,
                                                    self._length),
            'multiplexers': multiplexers
        }

    def _create_signal_tree(self, codec):
        """Create a multiplexing tree node of given codec. This is a recursive
        function.

        """

        nodes = []

        for signal in codec['signals']:
            multiplexers = codec['multiplexers']

            if signal.name in multiplexers:
                node = {
                    signal.name: {
                        mux: self._create_signal_tree(mux_codec)
                        for mux, mux_codec in multiplexers[signal.name].items()
                    }
                }
            else:
                node = signal.name

            nodes.append(node)

        return nodes

    @property
    def frame_id(self) -> int:
        """The message frame id.

        """

        return self._frame_id

    @frame_id.setter
    def frame_id(self, value: int) -> None:
        self._frame_id = value

    @property
    def is_extended_frame(self) -> bool:
        """``True`` if the message is an extended frame, ``False`` otherwise.

        """

        return self._is_extended_frame

    @is_extended_frame.setter
    def is_extended_frame(self, value: bool) -> None:
        self._is_extended_frame = value

    @property
    def name(self) -> str:
        """The message name as a string.

        """

        return self._name

    @name.setter
    def name(self, value: str) -> None:
        self._name = value

    @property
    def length(self) -> int:
        """The message data length in bytes.

        """

        return self._length

    @length.setter
    def length(self, value: int) -> None:
        self._length = value

    @property
    def signals(self) -> List[Signal]:
        """A list of all signals in the message.

        """

        return self._signals

    @property
    def unused_bit_pattern(self) -> int:
        """The pattern used for unused bits of a message.

        This prevents undefined behaviour and/or information leaks
        when encoding messages.
        """

        return self._unused_bit_pattern

    @unused_bit_pattern.setter
    def unused_bit_pattern(self, value):
        if value < 0 or value > 255:
            LOGGER.warnig(f'Invalid unused bit pattern "{value}". Must be '
                          f'an integer between 0 and 255')
            self._unused_bit_pattern = 0
            return

        self._unused_bit_pattern = value

    @property
    def signal_groups(self) -> Optional[List[SignalGroup]]:
        """A list of all signal groups in the message.

        """

        return self._signal_groups

    @signal_groups.setter
    def signal_groups(self, value: List[SignalGroup]) -> None:
        self._signal_groups = value

    @property
    def comment(self) -> Optional[str]:
        """The message comment, or ``None`` if unavailable.

        Note that we implicitly try to return the English comment if
        multiple languages were specified.

        """
        if self._comments is None:
            return None
        elif self._comments.get(None) is not None:
            return self._comments.get(None)
        elif self._comments.get("FOR-ALL") is not None:
            return self._comments.get("FOR-ALL")

        return self._comments.get('EN')

    @comment.setter
    def comment(self, value: Optional[str]) -> None:
        if value is None:
            self._comments = None
        else:
            self._comments = {None: value}

    @property
    def comments(self):
        """The dictionary with the descriptions of the message in multiple
        languages. ``None`` if unavailable.

        """
        return self._comments

    @comments.setter
    def comments(self, value):
        self._comments = value

    @property
    def senders(self):
        """A list of all sender nodes of this message.

        """

        return self._senders

    @property
    def send_type(self) -> Optional[str]:
        """The message send type, or ``None`` if unavailable.

        """

        return self._send_type

    @property
    def cycle_time(self) -> Optional[int]:
        """The message cycle time, or ``None`` if unavailable.

        """

        return self._cycle_time

    @property
    def dbc(self) -> Optional["DbcSpecifics"]:
        """An object containing dbc specific properties like e.g. attributes.

        """

        return self._dbc

    @dbc.setter
    def dbc(self, value: Optional["DbcSpecifics"]) -> None:
        self._dbc = value

    @property
    def autosar(self) -> Optional["AutosarMessageSpecifics"]:
        """An object containing AUTOSAR specific properties

        e.g. auxiliary data required to implement CRCs, secure on-board
        communication (secOC) or container messages.
        """

        return self._autosar

    @autosar.setter
    def autosar(self, value: Optional["AutosarMessageSpecifics"]) -> None:
        self._autosar = value

    @property
    def bus_name(self) -> Optional[str]:
        """The message bus name, or ``None`` if unavailable.

        """

        return self._bus_name

    @bus_name.setter
    def bus_name(self, value: Optional[str]) -> None:
        self._bus_name = value

    @property
    def protocol(self) -> Optional[str]:
        """The message protocol, or ``None`` if unavailable. Only one protocol
        is currently supported; ``'j1939'``.

        """

        return self._protocol

    @protocol.setter
    def protocol(self, value: Optional[str]) -> None:
        self._protocol = value

    @property
    def signal_tree(self):
        """All signal names and multiplexer ids as a tree. Multiplexer signals
        are dictionaries, while other signals are strings.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.signal_tree
        ['Bar', 'Fum']
        >>> bar = db.get_message_by_name('Bar')
        >>> bar.signal_tree
        [{'A': {0: ['C', 'D'], 1: ['E']}}, 'B']

        """

        return self._signal_tree

    def _get_mux_number(self, decoded, signal_name):
        mux = decoded[signal_name]

        if isinstance(mux, str) or isinstance(mux, NamedSignalValue):
            signal = self.get_signal_by_name(signal_name)
            mux = signal.choice_string_to_number(mux)

        return mux

    def _check_signals_ranges_scaling(self, signals, data):
        for signal in signals:
            value = data[signal.name]

            # Choices are checked later.
            if isinstance(value, str) or isinstance(value, NamedSignalValue):
                continue

            if signal.minimum is not None:
                if value < signal.minimum:
                    raise EncodeError(
                        "Expected signal '{}' value greater than or equal to "
                        "{} in message '{}', but got {}.".format(signal.name,
                                                                 signal.minimum,
                                                                 self._name,
                                                                 value))

            if signal.maximum is not None:
                if value > signal.maximum:
                    raise EncodeError(
                        "Expected signal '{}' value less than or equal to "
                        "{} in message '{}', but got {}.".format(signal.name,
                                                                 signal.maximum,
                                                                 self.name,
                                                                 value))

    def _check_signals(self, signals, data, scaling):
        for signal in signals:
            if signal.name not in data:
                raise EncodeError(
                    "Expected signal value for '{}' in data, but got {}.".format(
                        signal.name,
                        data))

        if scaling:
            self._check_signals_ranges_scaling(signals, data)

    def _check_unknown_signals(self, signals, data):
            signal_set = set(map(lambda x: x.name, signals))
            for signal in data:
                if signal not in signal_set:
                    raise EncodeError(
                        f"No signal named '{signal}' specified in CAN bus "
                        f"description database.")

    def _encode(self, node, data, scaling, strict):
        if strict:
            self._check_signals(node['signals'], data, scaling)

        encoded = encode_data(data,
                              node['signals'],
                              node['formats'],
                              scaling)
        padding_mask = node['formats'].padding_mask
        multiplexers = node['multiplexers']

        all_signals = list(node['signals'])
        for signal in multiplexers:
            mux = self._get_mux_number(data, signal)

            try:
                node = multiplexers[signal][mux]

                if strict:
                    self._check_signals(node['signals'], data, scaling)
            except KeyError:
                raise EncodeError('expected multiplexer id {}, but got {}'.format(
                    format_or(multiplexers[signal]),
                    mux))

            mux_encoded, mux_padding_mask, mux_signals = \
                self._encode(node, data, scaling, strict)
            all_signals.extend(mux_signals)

            encoded |= mux_encoded
            padding_mask &= mux_padding_mask

        return encoded, padding_mask, all_signals

    def encode(self,
               data: Dict[str, float],
               scaling: bool = True,
               padding: bool = False,
               strict: bool = True,
               ) -> bytes:
        """Encode given data as a message of this type.

        If `scaling` is ``False`` no scaling of signals is performed.

        If `padding` is ``True`` unused bits are encoded as 1.

        If `strict` is ``True`` the specified signals must exactly be the
        ones expected, and their values must be within their allowed ranges,
        or an `EncodeError` exception is raised.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        encoded, padding_mask, all_signals = \
            self._encode(self._codecs, data, scaling, strict=strict)

        if strict:
            self._check_unknown_signals(all_signals, data)

        if padding:
            # there is probably a cleaner and more performant way to
            # do this...
            padding_pattern = 0
            for i in range(0, self.length):
                padding_pattern |= self.unused_bit_pattern << (8*i)

            encoded &= ~padding_mask
            encoded |= padding_mask & padding_pattern

        encoded |= (0x80 << (8 * self._length))
        encoded = hex(encoded)[4:].rstrip('L')

        return binascii.unhexlify(encoded)[:self._length]

    def _decode(self,
                node: Codec,
                data: bytes,
                decode_choices: bool,
                scaling: bool) -> Dict[str, Union[float, str]]:
        decoded = decode_data(data,
                              node['signals'],
                              node['formats'],
                              decode_choices,
                              scaling)

        multiplexers = node['multiplexers']

        for signal in multiplexers:
            mux = self._get_mux_number(decoded, signal)

            try:
                node = multiplexers[signal][mux]
            except KeyError:
                raise DecodeError('expected multiplexer id {}, but got {}'.format(
                    format_or(multiplexers[signal]),
                    mux))

            decoded.update(self._decode(node,
                                        data,
                                        decode_choices,
                                        scaling))

        return decoded

    def decode(self,
               data: bytes,
               decode_choices: bool = True,
               scaling: bool = True,
               ) -> Dict[str, Union[float, str]]:
        """Decode given data as a message of this type.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        If `scaling` is ``False`` no scaling of signals is performed.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.decode(b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        if self._codecs is None:
            raise ValueError("Codec is not initialized.")

        data = data[:self._length]

        return self._decode(self._codecs, data, decode_choices, scaling)

    def get_signal_by_name(self, name: str) -> Signal:
        for signal in self._signals:
            if signal.name == name:
                return signal

        raise KeyError(name)

    def is_multiplexed(self) -> bool:
        """Returns ``True`` if the message is multiplexed, otherwise
        ``False``.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.is_multiplexed()
        False
        >>> bar = db.get_message_by_name('Bar')
        >>> bar.is_multiplexed()
        True

        """
        if self._codecs is None:
            raise ValueError("Codec is not initialized.")

        return bool(self._codecs['multiplexers'])

    def _check_signal(self, message_bits, signal):
        signal_bits = signal.length * [signal.name]

        if signal.byte_order == 'big_endian':
            padding = start_bit(signal) * [None]
            signal_bits = padding + signal_bits
        else:
            signal_bits += signal.start * [None]

            if len(signal_bits) < len(message_bits):
                padding = (len(message_bits) - len(signal_bits)) * [None]
                reversed_signal_bits = padding + signal_bits
            else:
                reversed_signal_bits = signal_bits

            signal_bits = []

            for i in range(0, len(reversed_signal_bits), 8):
                signal_bits = reversed_signal_bits[i:i + 8] + signal_bits

        # Check that the signal fits in the message.
        if len(signal_bits) > len(message_bits):
            raise Error(
                'The signal {} does not fit in message {}.'.format(
                    signal.name,
                    self.name))

        # Check that the signal does not overlap with other
        # signals.
        for offset, signal_bit in enumerate(signal_bits):
            if signal_bit is not None:
                if message_bits[offset] is not None:
                    raise Error(
                        'The signals {} and {} are overlapping in message {}.'.format(
                            signal.name,
                            message_bits[offset],
                            self.name))

                message_bits[offset] = signal.name

    def _check_mux(self, message_bits, mux):
        signal_name, children = list(mux.items())[0]
        self._check_signal(message_bits,
                           self.get_signal_by_name(signal_name))
        children_message_bits = deepcopy(message_bits)

        for multiplexer_id in sorted(children):
            child_tree = children[multiplexer_id]
            child_message_bits = deepcopy(children_message_bits)
            self._check_signal_tree(child_message_bits, child_tree)

            for i, child_bit in enumerate(child_message_bits):
                if child_bit is not None:
                    message_bits[i] = child_bit

    def _check_signal_tree(self, message_bits, signal_tree):
        for signal_name in signal_tree:
            if isinstance(signal_name, dict):
                self._check_mux(message_bits, signal_name)
            else:
                self._check_signal(message_bits,
                                   self.get_signal_by_name(signal_name))

    def _check_signal_lengths(self):
        for signal in self._signals:
            if signal.length <= 0:
                raise Error(
                    'The signal {} length {} is not greater than 0 in '
                    'message {}.'.format(
                        signal.name,
                        signal.length,
                        self.name))

    def refresh(self, strict: Optional[bool] = None) -> None:
        """Refresh the internal message state.

        If `strict` is ``True`` an exception is raised if any signals
        are overlapping or if they don't fit in the message. This
        argument overrides the value of the same argument passed to
        the constructor.

        """

        self._check_signal_lengths()
        self._codecs = self._create_codec()
        self._signal_tree = self._create_signal_tree(self._codecs)

        if strict is None:
            strict = self._strict

        if strict:
            message_bits = 8 * self.length * [None]
            self._check_signal_tree(message_bits, self.signal_tree)

    def __repr__(self) -> str:
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self._name,
            self._frame_id,
            self._is_extended_frame,
            self._length,
            self._comments)
