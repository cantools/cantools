import sys
from collections import OrderedDict
from collections.abc import Mapping, Sequence
from typing import (
    TYPE_CHECKING,
    Any,
    Callable,
    Literal,
    NamedTuple,
    Optional,
    TypedDict,
    Union,
)

if sys.version_info >= (3, 10):
    from typing import TypeAlias
else:
    from typing_extensions import TypeAlias

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
Comments = dict[Optional[str], str]
class Codec(TypedDict):
    signals: list["Signal"]
    formats: Formats
    multiplexers: Mapping[str, Mapping[int, Any]]

ByteOrder = Literal["little_endian", "big_endian"]
Choices = OrderedDict[int, Union[str, "NamedSignalValue"]]

# Type aliases. Introduced to reduce type annotation complexity while
# allowing for more complex encode/decode schemes like the one used
# for AUTOSAR container messages.
SignalValueType = Union[int, float, str, "NamedSignalValue"]
SignalDictType = dict[str, SignalValueType]
SignalMappingType = Mapping[str, SignalValueType]
ContainerHeaderSpecType = Union["Message", str, int]
ContainerUnpackResultType = Sequence[Union[tuple["Message", bytes], tuple[int, bytes]]]
ContainerUnpackListType = list[Union[tuple["Message", bytes], tuple[int, bytes]]]
ContainerDecodeResultType = Sequence[
    Union[tuple["Message", SignalMappingType], tuple["Message", bytes], tuple[int, bytes]]
]
ContainerDecodeResultListType = list[
    Union[tuple["Message", SignalDictType], tuple["Message", bytes], tuple[int, bytes]]
]
ContainerEncodeInputType = Sequence[
    tuple[ContainerHeaderSpecType, Union[bytes, SignalMappingType]]
]
DecodeResultType = Union[SignalDictType, ContainerDecodeResultType]
EncodeInputType = Union[SignalMappingType, ContainerEncodeInputType]

SecOCAuthenticatorFn = Callable[["Message", bytes, int], bytes]

TAdditionalCliArgs: TypeAlias = dict[str, Union[str, int, float, bool]]
