from typing import (
    TYPE_CHECKING,
    Any,
    Callable,
    Dict,
    List,
    Literal,
    Mapping,
    NamedTuple,
    Optional,
    OrderedDict,
    Sequence,
    Tuple,
    TypedDict,
    Union,
)

from bitstruct import CompiledFormatDict

if TYPE_CHECKING:
    import os

    from .database import Message, Signal
    from .database.namedsignalvalue import NamedSignalValue


class Formats(NamedTuple):
    big_endian: CompiledFormatDict
    little_endian: CompiledFormatDict
    padding_mask: int


StringPathLike = Union[str, "os.PathLike[str]"]
Comments = Dict[Optional[str], str]
class Codec(TypedDict):
    signals: List["Signal"]
    formats: Formats
    multiplexers: Mapping[str, Mapping[int, Any]]

ByteOrder = Literal["little_endian", "big_endian"]
Choices = OrderedDict[int, Union[str, "NamedSignalValue"]]

# Type aliases. Introduced to reduce type annotation complexity while
# allowing for more complex encode/decode schemes like the one used
# for AUTOSAR container messages.
SignalValueType = Union[int, float, str, "NamedSignalValue"]
SignalDictType = Dict[str, SignalValueType]
SignalMappingType = Mapping[str, SignalValueType]
ContainerHeaderSpecType = Union["Message", str, int]
ContainerUnpackResultType = Sequence[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerUnpackListType = List[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerDecodeResultType = Sequence[
    Union[Tuple["Message", SignalMappingType], Tuple["Message", bytes], Tuple[int, bytes]]
]
ContainerDecodeResultListType = List[
    Union[Tuple["Message", SignalDictType], Tuple["Message", bytes], Tuple[int, bytes]]
]
ContainerEncodeInputType = Sequence[
    Tuple[ContainerHeaderSpecType, Union[bytes, SignalMappingType]]
]
DecodeResultType = Union[SignalDictType, ContainerDecodeResultType]
EncodeInputType = Union[SignalMappingType, ContainerEncodeInputType]

SecOCAuthenticatorFn = Callable[["Message", bytes, int], bytes]
