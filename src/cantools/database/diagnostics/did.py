# A DID.

import binascii
from typing import TypedDict

from cantools.database.diagnostics.data import Data
from cantools.typechecking import Formats, SignalDictType, SignalMappingType

from ..utils import create_encode_decode_formats, decode_data, encode_data


class _Codec(TypedDict):
    datas: list[Data]
    formats: Formats


class Did:
    """A DID with identifier and other information.

    """

    def __init__(self,
                 identifier: int,
                 name: str,
                 length: int,
                 datas: list[Data]) -> None:
        self._identifier = identifier
        self._name = name
        self._length = length
        self._datas = datas
        self.refresh()

    @property
    def identifier(self) -> int:
        """The did identifier as an integer.

        """

        return self._identifier

    @identifier.setter
    def identifier(self, value: int) -> None:
        self._identifier = value

    @property
    def name(self) -> str:
        """The did name as a string.

        """

        return self._name

    @name.setter
    def name(self, value: str) -> None:
        self._name = value

    @property
    def length(self) -> int:
        """The did length as an int.

        """

        return self._length

    @length.setter
    def length(self, value: int) -> None:
        self._length = value

    @property
    def datas(self) -> list[Data]:
        """The did datas as a string.

        """

        return self._datas

    @datas.setter
    def datas(self, value: list[Data]) -> None:
        self._datas = value

    def get_data_by_name(self, name: str) -> Data:
        for data in self._datas:
            if data.name == name:
                return data

        raise KeyError(name)

    def encode(self, data: SignalMappingType, scaling: bool = True) -> bytes:
        """Encode given data as a DID of this type.

        If `scaling` is ``False`` no scaling of datas is performed.

        >>> foo = db.get_did_by_name('Foo')
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        encoded = encode_data(data,
                              self._codec['datas'],
                              self._codec['formats'],
                              scaling)
        encoded |= (0x80 << (8 * self._length))
        encoded_str = hex(encoded)[4:].rstrip('L')

        return binascii.unhexlify(encoded_str)[:self._length]

    def decode(self,
               data: bytes,
               decode_choices: bool = True,
               scaling: bool = True,
               allow_truncated: bool = False,
               allow_excess: bool = True) -> SignalDictType:
        """Decode given data as a DID of this type.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        If `scaling` is ``False`` no scaling of datas is performed.

        >>> foo = db.get_did_by_name('Foo')
        >>> foo.decode(b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        return decode_data(data[:self._length],
                           self.length,
                           self._codec['datas'],
                           self._codec['formats'],
                           decode_choices,
                           scaling,
                           allow_truncated,
                           allow_excess)

    def refresh(self) -> None:
        """Refresh the internal DID state.

        """

        self._codec = _Codec(
            datas=self._datas,
            formats=create_encode_decode_formats(self._datas,
                                                 self._length)
        )

    def __repr__(self) -> str:
        return f"did('{self._name}', 0x{self._identifier:04x})"
