# Load and dump a diagnostics database in CDD format.
import logging
from collections import OrderedDict
from typing import Optional
from xml.etree import ElementTree

from cantools.typechecking import ByteOrder, Choices

from ...conversion import BaseConversion
from ...errors import ParseError
from ...utils import cdd_offset_to_dbc_start_bit
from ..data import Data
from ..did import Did
from ..internal_database import InternalDatabase

LOGGER = logging.getLogger(__name__)


class DataType:

    def __init__(self,
                 name: str,
                 id_: str,
                 bit_length: Optional[int],
                 encoding: Optional[str],
                 minimum: Optional[int],
                 maximum: Optional[int],
                 choices: Optional[Choices],
                 byte_order: ByteOrder,
                 unit: Optional[str],
                 factor: float,
                 offset: float) -> None:
        self.name = name
        self.id_ = id_
        self.bit_length = bit_length
        self.encoding = encoding
        self.minimum = minimum
        self.maximum = maximum
        self.choices = choices
        self.byte_order: ByteOrder = byte_order
        self.unit = unit
        self.factor = factor
        self.offset = offset


def _load_choices(data_type: ElementTree.Element) -> Optional[Choices]:
    choices: Choices = OrderedDict()

    for choice in data_type.findall('TEXTMAP'):
        start = int(choice.attrib['s'].strip('()'))
        end = int(choice.attrib['e'].strip('()'))

        if start == end:
            choice_text_elem = choice.find('TEXT/TUV[1]')
            if choice_text_elem is None:
                raise KeyError("Could not find TUV element in TEXTMAP!")
            if choice_text_elem.text is None:
                raise ValueError(f"Could not find name in TUV!")
            choices[start] = choice_text_elem.text

    if not choices:
        return None

    return choices


def _load_data_types(ecu_doc: Optional[ElementTree.Element]) -> dict[str, DataType]:
    """Load all data types found in given ECU doc element.

    """

    data_types: dict[str, DataType] = {}

    if ecu_doc is None:
        return data_types

    types = ecu_doc.findall('DATATYPES/*')

    for data_type in types:
        # Default values.
        byte_order: ByteOrder = 'big_endian'
        unit = None
        factor = 1.0
        offset = 0.0
        bit_length = None
        encoding = None
        minimum = None
        maximum = None

        # Name and id.
        type_name_elem = data_type.find('NAME/TUV[1]')
        if type_name_elem is None:
            raise KeyError(f"Could not find TUV element in DATATYPE IDENT with id{data_type.attrib.get('id')}!")
        type_name = type_name_elem.text
        if type_name is None:
            raise ValueError(f"Could not find name in DATATYPE IDENT with id={data_type.attrib.get('id')}!")
        type_id = data_type.attrib['id']

        # Load from C-type element.
        ctype = data_type.find('CVALUETYPE')
        if ctype is not None:
            for key, value in ctype.attrib.items():
                if key == 'bl':
                    bit_length = int(value)
                elif key == 'enc':
                    encoding = value
                elif key == 'minsz':
                    minimum = int(value)
                elif key == 'maxsz':
                    maximum = int(value)
                else:
                    LOGGER.debug("Ignoring unsupported attribute '%s'.", key)

            if ctype.attrib['bo'] == '21':
                byte_order = 'big_endian'
            elif ctype.attrib['bo'] == '12':
                byte_order = 'little_endian'
            else:
                raise ParseError(f"Unknown byte order code: {ctype.attrib['bo']}")

        # Load from P-type element.
        ptype_unit = data_type.find('PVALUETYPE/UNIT')

        if ptype_unit is not None:
            unit = ptype_unit.text

        # Choices, scale and offset.
        choices = _load_choices(data_type)

        # Slope and offset.
        comp = data_type.find('COMP')

        if comp is not None:
            factor = float(comp.attrib['f'])
            offset = float(comp.attrib['o'])

        data_types[type_id] = DataType(type_name,
                                       type_id,
                                       bit_length,
                                       encoding,
                                       minimum,
                                       maximum,
                                       choices,
                                       byte_order,
                                       unit,
                                       factor,
                                       offset)

    return data_types


def _load_data_element(data: ElementTree.Element, offset: int, data_types: dict[str, DataType]) -> Data:
    """Load given signal element and return a signal object.

    """

    data_type = data_types[data.attrib['dtref']]

    # Map CDD/c-style field offset to the DBC/can.Signal.start bit numbering
    # convention for compatibility with can.Signal objects and the shared codec
    # infrastructure.
    #
    dbc_start_bitnum = cdd_offset_to_dbc_start_bit(offset, data_type.bit_length, data_type.byte_order)

    conversion = BaseConversion.factory(
        scale=data_type.factor,
        offset=data_type.offset,
        choices=data_type.choices,
        is_float=False
    )

    qual_elem = data.find('QUAL')
    if qual_elem is None:
        raise ValueError(f"Could not find QUAL in data with id={data.attrib.get('id')}!")
    name = qual_elem.text
    if name is None:
        raise ValueError(f"Could not get QUAL text in data with id={data.attrib.get('id')}!")

    return Data(name=name,
                start=dbc_start_bitnum,
                length=data_type.bit_length,
                byte_order=data_type.byte_order,
                conversion=conversion,
                minimum=data_type.minimum,
                maximum=data_type.maximum,
                unit=data_type.unit)


def _load_did_element(did: ElementTree.Element, data_types: dict[str, DataType], did_data_lib: dict[str, ElementTree.Element]) -> Did:
    """Load given DID element and return a did object.

    """

    offset = 0
    datas: list[Data] = []
    data_objs = did.findall('SIMPLECOMPCONT/DATAOBJ')
    data_objs += did.findall('SIMPLECOMPCONT/UNION/STRUCT/DATAOBJ')
    did_data_refs = did.findall('SIMPLECOMPCONT/DIDDATAREF')

    for data_ref in did_data_refs:
        try:
            data_objs += did_data_lib[data_ref.attrib['didRef']].findall('STRUCTURE/DATAOBJ')
        except KeyError:
            pass

    for data_obj in data_objs:
        data = _load_data_element(data_obj,
                                  offset,
                                  data_types)

        if data:
            datas.append(data)
            offset += data.length

    static_value = did.find('STATICVALUE')
    if static_value is None:
        raise KeyError(f"Could not find STATICVALUE element in DID with id={did.attrib.get('id')}!")
    identifier = int(static_value.attrib['v'])
    qual_elem = did.find('QUAL')
    if qual_elem is None:
        raise ValueError(f"Could not find QUAL in DID with id={did.attrib.get('id')}!")
    name = qual_elem.text
    if name is None:
        raise ValueError(f"Could not get QUAL text in DID with id={did.attrib.get('id')}!")
    length = (offset + 7) // 8

    return Did(identifier=identifier,
               name=name,
               length=length,
               datas=datas)


def _load_did_data_refs(ecu_doc: Optional[ElementTree.Element]) -> dict[str, ElementTree.Element]:
    """Load DID data references from given ECU doc element.

    """
    if ecu_doc is None:
        return {}

    dids = ecu_doc.find('DIDS')

    if dids is None:
        return {}
    else:
        return {did.attrib['id']: did for did in dids.findall('DID')}


def load_string(string: str) -> InternalDatabase:
    """Parse given CDD format string.

    """

    root = ElementTree.fromstring(string)
    ecu_doc = root.find('ECUDOC')
    if ecu_doc is None:
        raise KeyError("Could not find ECUDOC root element!")
    data_types = _load_data_types(ecu_doc)
    did_data_lib = _load_did_data_refs(ecu_doc)
    var = ecu_doc.findall('ECU')[0].find('VAR')
    if var is None:
        raise KeyError(f"Could not find VAR element in ECU with id={ecu_doc.findall('ECU')[0].attrib.get('id')}!")
    dids: list[Did] = []

    for diag_class in var.findall('DIAGCLASS'):
        for diag_inst in diag_class.findall('DIAGINST'):
            did = _load_did_element(diag_inst,
                                    data_types,
                                    did_data_lib)
            dids.append(did)

    return InternalDatabase(dids)
