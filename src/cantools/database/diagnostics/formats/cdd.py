# Load and dump a diagnostics database in CDD format.
import logging
from collections import OrderedDict
from xml.etree import ElementTree

from cantools.typechecking import ByteOrder, Choices

from ...can.formats.utils import parse_float, parse_int
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
                 bit_length: int,
                 encoding: str | None,
                 minimum: int | None,
                 maximum: int | None,
                 choices: Choices | None,
                 byte_order: ByteOrder,
                 unit: str | None,
                 factor: float,
                 offset: float) -> None:
        self.name = name
        self.id_ = id_
        self.bit_length = bit_length
        self.encoding = encoding
        self.minimum = minimum
        self.maximum = maximum
        self.choices = choices
        self.byte_order = byte_order
        self.unit = unit
        self.factor = factor
        self.offset = offset


def _attribute(element: ElementTree.Element, name: str) -> str:
    try:
        return element.attrib[name]
    except KeyError:
        raise ParseError(f"Could not find attribute '{name}' in {element.tag} "
                         f"element with id={element.attrib.get('id')}!") from None


def _load_choices(data_type: ElementTree.Element) -> Choices | None:
    choices: Choices = OrderedDict()

    for choice in data_type.findall('TEXTMAP'):
        start = parse_int(_attribute(choice, 's').strip('()'),
                          "attribute 's' of TEXTMAP")
        end = parse_int(_attribute(choice, 'e').strip('()'),
                        "attribute 'e' of TEXTMAP")

        if start == end:
            choice_text = choice.findtext('TEXT/TUV[1]')
            if choice_text is None:
                raise ParseError(f"Could not find name in TUV!")
            choices[start] = choice_text

    if not choices:
        return None

    return choices


def _load_data_types(ecu_doc: ElementTree.Element | None) -> dict[str, DataType]:
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
        factor: float = 1
        offset: float = 0
        bit_length = 0
        encoding = None
        minimum = None
        maximum = None

        # Name and id.
        type_name = data_type.findtext('NAME/TUV[1]')
        if type_name is None:
            raise ParseError(f"Could not find name in DATATYPE IDENT with id={data_type.attrib.get('id')}!")
        type_id = _attribute(data_type, 'id')

        # Load from C-type element.
        ctype = data_type.find('CVALUETYPE')
        if ctype is not None:
            for key, value in ctype.attrib.items():
                what = f"attribute '{key}' of CVALUETYPE in data type {type_name}"

                if key == 'bl':
                    bit_length = parse_int(value, what)
                elif key == 'enc':
                    encoding = value
                elif key == 'minsz':
                    minimum = parse_int(value, what)
                elif key == 'maxsz':
                    maximum = parse_int(value, what)
                else:
                    LOGGER.debug("Ignoring unsupported attribute '%s'.", key)

            byte_order_code = _attribute(ctype, 'bo')

            if byte_order_code == '21':
                byte_order = 'big_endian'
            elif byte_order_code == '12':
                byte_order = 'little_endian'
            else:
                raise ParseError(f"Unknown byte order code: {byte_order_code}")

        # Load from P-type element.
        unit = data_type.findtext('PVALUETYPE/UNIT')

        # Choices, scale and offset.
        choices = _load_choices(data_type)

        # Slope and offset.
        comp = data_type.find('COMP')

        if comp is not None:
            factor = parse_float(_attribute(comp, 'f'),
                                 f"attribute 'f' of COMP in data type {type_name}")
            offset = parse_float(_attribute(comp, 'o'),
                                 f"attribute 'o' of COMP in data type {type_name}")

        if bit_length == 0:
            raise ParseError("CVALUETYPE element cannot have bit length 0!")

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

    data_type_id = _attribute(data, 'dtref')

    try:
        data_type = data_types[data_type_id]
    except KeyError:
        raise ParseError(f"Could not find data type {data_type_id} referenced "
                         f"by data with id={data.attrib.get('id')}!") from None

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

    name = data.findtext('QUAL')
    if name is None:
        raise ParseError(f"Could not get QUAL text in data with id={data.attrib.get('id')}!")

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
            offset += data.length or 0

    static_value = did.find('STATICVALUE')
    if static_value is None:
        raise ParseError(f"Could not find STATICVALUE element in DID with id={did.attrib.get('id')}!")
    identifier = parse_int(_attribute(static_value, 'v'),
                           f"attribute 'v' of STATICVALUE in DID with "
                           f"id={did.attrib.get('id')}")
    name = did.findtext('QUAL')
    if name is None:
        raise ParseError(f"Could not get QUAL text in DID with id={did.attrib.get('id')}!")
    length = (offset + 7) // 8

    return Did(identifier=identifier,
               name=name,
               length=length,
               datas=datas)


def _load_did_data_refs(ecu_doc: ElementTree.Element | None) -> dict[str, ElementTree.Element]:
    """Load DID data references from given ECU doc element.

    """
    if ecu_doc is None:
        return {}

    dids = ecu_doc.find('DIDS')

    if dids is None:
        return {}
    else:
        return {_attribute(did, 'id'): did for did in dids.findall('DID')}


def load_string(string: str) -> InternalDatabase:
    """Parse given CDD format string.

    """

    try:
        root = ElementTree.fromstring(string)
    except ElementTree.ParseError as e:
        raise ParseError(str(e)) from e

    ecu_doc = root.find('ECUDOC')
    if ecu_doc is None:
        raise ParseError("Could not find ECUDOC root element!")
    data_types = _load_data_types(ecu_doc)
    did_data_lib = _load_did_data_refs(ecu_doc)
    ecu = ecu_doc.find('ECU')
    if ecu is None:
        raise ParseError("Could not find ECU element in ECUDOC!")
    var = ecu.find('VAR')
    if var is None:
        raise ParseError(f"Could not find VAR element in ECU with id={ecu.attrib.get('id')}!")
    dids: list[Did] = []

    for diag_class in var.findall('DIAGCLASS'):
        for diag_inst in diag_class.findall('DIAGINST'):
            did = _load_did_element(diag_inst,
                                    data_types,
                                    did_data_lib)
            dids.append(did)

    return InternalDatabase(dids)
