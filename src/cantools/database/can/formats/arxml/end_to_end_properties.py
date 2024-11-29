from typing import Optional


class AutosarEnd2EndProperties:
    """This class collects all attributes that are required to implement
    AUTOSAR-conformant End-to-End protection (CRCs) of messages
    """

    def __init__(self) -> None:
        self._category: Optional[str] = None
        self._data_ids: Optional[list[int]] = None
        self._payload_length: int = 0

    @property
    def category(self) -> Optional[str]:
        """The category string of the applicable end-to-end protection
        mechanism

        Note that the contents of these are not specified by the
        AUTOSAR standard.
        """
        return self._category

    @category.setter
    def category(self, value: Optional[str]) -> None:
        self._category = value

    @property
    def data_ids(self) -> Optional[list[int]]:
        """The list of data IDs applicable
        """
        return self._data_ids

    @data_ids.setter
    def data_ids(self, value: Optional[list[int]]) -> None:
        self._data_ids = value

    @property
    def payload_length(self) -> int:
        """The size of the end-to-end protected data in bytes

        This number includes the end-to-end protection signals
        themselves (i.e. the sequence counter and the CRC value)

        """
        return self._payload_length

    @payload_length.setter
    def payload_length(self, value: int) -> None:
        self._payload_length = value
