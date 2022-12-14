from typing import (
    TYPE_CHECKING,
    NamedTuple,
    Union,
    Mapping,
    Dict,
    Optional,
    List,
    Any,
    Sequence,
    Tuple,
    Callable,
)

from typing_extensions import TypedDict, Literal, OrderedDict
from bitstruct import CompiledFormatDict

if TYPE_CHECKING:
    import os
    import sys
    from .database import Signal, Message
    from .database.can.signal import NamedSignalValue


class Formats(NamedTuple):
    big_endian: CompiledFormatDict
    little_endian: CompiledFormatDict
    padding_mask: int


StringPathLike = Union[str, "os.PathLike[str]"]
Comments = Dict[Optional[str], str]
Codec = TypedDict(
    "Codec",
    {
        "signals": List["Signal"],
        "formats": Formats,
        "multiplexers": Mapping[str, Mapping[int, Any]],  # "Any" should be "Codec" (cyclic definition is not possible though)
    },
)

ByteOrder = Literal["little_endian", "big_endian"]
Choices = OrderedDict[int, Union[str, "NamedSignalValue"]]

# Type aliases. Introduced to reduce type annotation complexity while
# allowing for more complex encode/decode schemes like the one used
# for AUTOSAR container messages.
SignalValueType = Union[float, str, "NamedSignalValue"]
SignalDictType = Dict[str, SignalValueType]
SignalMappingType = Mapping[str, SignalValueType]
ContainerHeaderSpecType = Union["Message", str, int]
ContainerUnpackResultType = Sequence[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerUnpackListType = List[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerDecodeResultType = Sequence[
    Union[Tuple["Message", SignalMappingType], Tuple[int, bytes]]
]
ContainerDecodeResultListType = List[
    Union[Tuple["Message", SignalDictType], Tuple[int, bytes]]
]
ContainerEncodeInputType = Sequence[
    Tuple[ContainerHeaderSpecType, Union[bytes, SignalMappingType]]
]
DecodeResultType = Union[SignalDictType, ContainerDecodeResultType]
EncodeInputType = Union[SignalMappingType, ContainerEncodeInputType]

SecOCAuthenticatorFn = Callable[["Message", bytearray, int], bytearray]
