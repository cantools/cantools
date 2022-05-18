from typing import (
    TYPE_CHECKING,
    NamedTuple,
    Union,
    Dict,
    Optional,
    List,
    Any,
    Sequence,
    Tuple,
)

from typing_extensions import TypedDict
from bitstruct import CompiledFormatDict


if TYPE_CHECKING:
    import os
    import sys
    from cantools.database import Signal, Message
    from cantools.database.can.signal import NamedSignalValue


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
        "multiplexers": Dict[str, Dict[int, Any]],
    },
)


# Type aliases. Introduced to reduce type annotation complexity while
# allowing for more complex encode/decode schemes like the one used
# for AUTOSAR container messages.
SignalDictType = Dict[str, Union[float, str, "NamedSignalValue"]]
ContainerHeaderSpecType = Union["Message", str, int]
ContainerUnpackResultType = Sequence[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerUnpackListType = List[Union[Tuple["Message", bytes], Tuple[int, bytes]]]
ContainerDecodeResultType = Sequence[
    Union[Tuple["Message", SignalDictType], Tuple[int, bytes]]
]
ContainerDecodeResultListType = List[
    Union[Tuple["Message", SignalDictType], Tuple[int, bytes]]
]
ContainerEncodeInputType = Sequence[
    Tuple[ContainerHeaderSpecType, Union[bytes, SignalDictType]]
]
DecodeResultType = Union[SignalDictType, ContainerDecodeResultType]
EncodeInputType = Union[SignalDictType, ContainerEncodeInputType]
