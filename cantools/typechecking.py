import typing

import typing_extensions
from bitstruct import CompiledFormatDict

if typing.TYPE_CHECKING:
    import os
    import sys
    from cantools.database import Signal


class Formats(typing.NamedTuple):
    big_endian: CompiledFormatDict
    little_endian: CompiledFormatDict
    padding_mask: int


StringPathLike = typing.Union[str, "os.PathLike[str]"]
Comments = typing.Dict[typing.Optional[str], str]
Codec = typing_extensions.TypedDict(
    "Codec",
    {
        "signals": typing.List["Signal"],
        "formats": Formats,
        "multiplexers": typing.Dict[str, typing.Dict[int, typing.Any]],
    },
)
