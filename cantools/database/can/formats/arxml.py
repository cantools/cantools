# Load and dump a CAN database in ARXML format.

import re
import logging
from decimal import Decimal

from xml.etree import ElementTree

from ..signal import Signal
from ..signal import Decimal as SignalDecimal
from ..message import Message
from ..internal_database import InternalDatabase


LOGGER = logging.getLogger(__name__)

class SystemLoader(object):
    def __init__(self, root, strict):
        self._root = root
        self._strict = strict

        m = re.match('^\\{(.*)\\}AUTOSAR$', self._root.tag)

        if not m:
            raise ValueError(f"No XML namespace specified or illegal root tag name '{self._root.tag}'")

        xml_namespace = m.group(1)
        self.xml_namespace = xml_namespace
        self._xml_namespaces = { 'ns': xml_namespace }

        m = re.match('^http://autosar\.org/schema/r(4\..*)$', xml_namespace)
        if m:
            # AUTOSAR 4
            autosar_version_string = m.group(1)
        else:
            raise ValueError(f"Unrecognized AUTOSAR XML namespace '{xml_namespace}'")

        m = re.match('^([0-9]*)(\.[0-9]*)?(\.[0-9]*)?$', autosar_version_string)
        if not m:
            raise ValueError(f"Could not parse AUTOSAR version '{autosar_version_string}'")

        self.autosar_version_major = int(m.group(1))
        self.autosar_version_minor = 0 if m.group(2) is None else int(m.group(2)[1:])
        self.autosar_version_patch = 0 if m.group(3) is None else int(m.group(3)[1:])

        if self.autosar_version_major != 4:
            raise ValueError('This class only supports AUTOSAR version 4')

        self._arxml_reference_cache = {}

    def autosar_version_newer(self, major, minor=None, patch=None):
        """Returns true iff the AUTOSAR version specified in the ARXML it at
        least as the version specified by the function parameters

        If a part of the specified version is 'None', it and the
        'lesser' parts of the version are not considered. Also, the
        major version number *must* be specified.
        """

        if self.autosar_version_major > major:
            return True
        elif self.autosar_version_major < major:
            return False

        # the major part of the queried version is identical to the
        # one used by the ARXML
        if minor is None:
            # don't care
            return True
        elif self.autosar_version_minor > minor:
            return True
        elif self.autosar_version_minor < minor:
            return False

        # the major and minor parts of the queried version are identical
        # to the one used by the ARXML
        if patch is None:
            # don't care
            return True
        elif self.autosar_version_patch > patch:
            return True
        elif self.autosar_version_patch < patch:
            return False

        # all parts of the queried version are identical to the one
        # actually used by the ARXML
        return True

    def load(self):
        buses = []
        messages = []
        version = None

        # This code inspects the top level packages. Packages in
        # sub-packages are not treated yet.  This might or might not
        # be necessary.
        can_frame_triggerings = \
            self._get_arxml_children(self._root,
                                     [
                                         'AR-PACKAGES',
                                         '*AR-PACKAGE',
                                         'ELEMENTS',
                                         '*&CAN-CLUSTER',
                                         'CAN-CLUSTER-VARIANTS',
                                         '*&CAN-CLUSTER-CONDITIONAL',
                                         'PHYSICAL-CHANNELS',
                                         '*&CAN-PHYSICAL-CHANNEL',
                                         'FRAME-TRIGGERINGS',
                                         '*&CAN-FRAME-TRIGGERING'
                                     ])
        for can_frame_triggering in can_frame_triggerings:
            messages.append(self._load_message(can_frame_triggering))

        return InternalDatabase(messages,
                                [],
                                buses,
                                version)

    def _load_message(self, can_frame_triggering):
        """Load given message and return a message object.

        """

        # Default values.
        cycle_time = None
        senders = []

        can_frame = self._get_can_frame(can_frame_triggering)

        # Name, frame id, length, is_extended_frame and comment.
        name = self._load_message_name(can_frame)
        frame_id = self._load_message_frame_id(can_frame_triggering)
        length = self._load_message_length(can_frame)
        is_extended_frame = self._load_message_is_extended_frame(can_frame_triggering)
        comments = self._load_message_comments(can_frame)

        # ToDo: senders

        # Find all signals in this message.
        signals = []

        # For "sane" bus systems like CAN or LIN, there ought to be
        # only a single PDU per frame. AUTOSAR also supports "insane"
        # bus systems like flexray, though...
        pdu = self._get_pdu(can_frame)

        if pdu is not None:
            time_period = \
                self._get_unique_arxml_child(pdu,
                                             [
                                                 'I-PDU-TIMING-SPECIFICATIONS',
                                                 'I-PDU-TIMING',
                                                 'TRANSMISSION-MODE-DECLARATION',
                                                 'TRANSMISSION-MODE-TRUE-TIMING',
                                                 'CYCLIC-TIMING',
                                                 'TIME-PERIOD',
                                                 'VALUE'
                                             ])

            if time_period is not None:
                cycle_time = int(float(time_period.text) * 1000)

            i_signal_to_i_pdu_mappings = \
                self._get_arxml_children(pdu,
                                         [
                                             'I-SIGNAL-TO-PDU-MAPPINGS',
                                             '*&I-SIGNAL-TO-I-PDU-MAPPING'
                                         ])

            for i_signal_to_i_pdu_mapping in i_signal_to_i_pdu_mappings:
                signal = self._load_signal(i_signal_to_i_pdu_mapping)

                if signal is not None:
                    signals.append(signal)

        return Message(frame_id=frame_id,
                       is_extended_frame=is_extended_frame,
                       name=name,
                       length=length,
                       senders=senders,
                       send_type=None,
                       cycle_time=cycle_time,
                       signals=signals,
                       comment=comments,
                       bus_name=None,
                       strict=self._strict)

    def _load_message_name(self, can_frame_triggering):
        return self._get_unique_arxml_child(can_frame_triggering,
                                            'SHORT-NAME').text

    def _load_message_frame_id(self, can_frame_triggering):
        return int(self._get_unique_arxml_child(can_frame_triggering,
                                                'IDENTIFIER').text)

    def _load_message_length(self, can_frame):
        return int(self._get_unique_arxml_child(can_frame,
                                                'FRAME-LENGTH').text)

    def _load_message_is_extended_frame(self, can_frame_triggering):
        can_addressing_mode = \
            self._get_unique_arxml_child(can_frame_triggering,
                                         'CAN-ADDRESSING-MODE')

        return False if can_addressing_mode is None \
                     else can_addressing_mode.text == 'EXTENDED'

    def _load_message_comments(self, can_frame):
        result = {}

        for l_2 in self._get_arxml_children(can_frame, ['DESC', '*L-2']):
            lang = l_2.attrib.get('L', 'EN')
            result[lang] = l_2.text

        if len(result) == 0:
            return None
        return result

    def _load_signal(self, i_signal_to_i_pdu_mapping):
        """Load given signal and return a signal object.

        """

        # Default values.
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        unit = None
        choices = None
        comments = None
        receivers = []
        decimal = SignalDecimal(Decimal(factor), Decimal(offset))

        i_signal = self._get_i_signal(i_signal_to_i_pdu_mapping)

        if i_signal is None:
            # Probably a signal group (I-SIGNAL-GROUP).
            return None

        # Name, start position, length and byte order.
        name = self._load_signal_name(i_signal)
        start_position = self._load_signal_start_position(i_signal_to_i_pdu_mapping)
        length = self._load_signal_length(i_signal)
        byte_order = self._load_signal_byte_order(i_signal_to_i_pdu_mapping)

        system_signal = self._get_system_signal(i_signal)

        if system_signal is not None:
            # Unit and comment.
            unit = self._load_signal_unit(system_signal)
            comments = self._load_signal_comments(system_signal)

            # Minimum, maximum, factor, offset and choices.
            minimum, maximum, factor, offset, choices = \
                self._load_system_signal(system_signal, decimal)

        # Type.
        is_signed, is_float = self._load_signal_type(i_signal)

        # ToDo: receivers

        return Signal(name=name,
                      start=start_position,
                      length=length,
                      receivers=receivers,
                      byte_order=byte_order,
                      is_signed=is_signed,
                      scale=factor,
                      offset=offset,
                      minimum=minimum,
                      maximum=maximum,
                      unit=unit,
                      choices=choices,
                      comment=comments,
                      is_float=is_float,
                      decimal=decimal)

    def _load_signal_name(self, i_signal):
        return self._get_unique_arxml_child(i_signal,
                                            'SHORT-NAME').text

    def _load_signal_start_position(self, i_signal_to_i_pdu_mapping):
        return int(self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                                'START-POSITION').text)

    def _load_signal_length(self, i_signal):
        return int(self._get_unique_arxml_child(i_signal, 'LENGTH').text)

    def _load_signal_byte_order(self, i_signal_to_i_pdu_mapping):
        packing_byte_order = \
            self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                         'PACKING-BYTE-ORDER')

        if packing_byte_order is not None \
           and packing_byte_order.text == 'MOST-SIGNIFICANT-BYTE-FIRST':
            return 'big_endian'
        else:
            return 'little_endian'

    def _load_signal_unit(self, system_signal):
        res = self._get_unique_arxml_child(system_signal,
                                           [
                                               'PHYSICAL-PROPS',
                                               'SW-DATA-DEF-PROPS-VARIANTS',
                                               '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                               '&UNIT',
                                               'DISPLAY-NAME'
                                           ])

        return None if res is None else res.text

    def _load_signal_comments(self, system_signal):
        result = {}

        for l_2 in self._get_arxml_children(system_signal, ['DESC', '*L-2']):
            lang = l_2.attrib.get('L', 'EN')
            result[lang] = l_2.text

        if len(result) == 0:
            return None
        return result

    def _load_minimum(self, minimum, decimal):
        if minimum is not None:
            decimal.minimum = Decimal(minimum.text)
            minimum = float(decimal.minimum)

        return minimum

    def _load_maximum(self, maximum, decimal):
        if maximum is not None:
            decimal.maximum = Decimal(maximum.text)
            maximum = float(decimal.maximum)

        return maximum

    def _load_texttable(self, compu_method, decimal):
        minimum = None
        maximum = None
        choices = {}

        for compu_scale in self._get_arxml_children(compu_method,
                                                    [
                                                        '&COMPU-INTERNAL-TO-PHYS',
                                                        'COMPU-SCALES',
                                                        '*&COMPU-SCALE'
                                                    ]):
            lower_limit = self._get_unique_arxml_child(compu_scale, 'LOWER-LIMIT')
            upper_limit = self._get_unique_arxml_child(compu_scale, 'UPPER-LIMIT')
            vt = self._get_unique_arxml_child(compu_scale, ['&COMPU-CONST', 'VT'])

            if vt is not None:
                choices[vt.text] = int(lower_limit.text)
            else:
                minimum = self._load_minimum(lower_limit, decimal)
                maximum = self._load_maximum(upper_limit, decimal)

        return minimum, maximum, choices

    def _load_linear_factor_and_offset(self, compu_scale, decimal):
        compu_rational_coeffs = \
            self._get_unique_arxml_child(compu_scale, '&COMPU-RATIONAL-COEFFS')

        if compu_rational_coeffs is None:
            return 1, 0

        numerators = self._get_arxml_children(compu_rational_coeffs,
                                              ['&COMPU-NUMERATOR', '*&V'])

        if len(numerators) != 2:
            raise ValueError(
                'Expected 2 numerator values for linear scaling, but '
                'got {}.'.format(len(numerators)))

        denominators = self._get_arxml_children(compu_rational_coeffs,
                                                ['&COMPU-DENOMINATOR', '*&V'])

        if len(denominators) != 1:
            raise ValueError(
                'Expected 1 denominator value for linear scaling, but '
                'got {}.'.format(len(denominators)))

        denominator = Decimal(denominators[0].text)
        decimal.scale = Decimal(numerators[1].text) / denominator
        decimal.offset = Decimal(numerators[0].text) / denominator

        return float(decimal.scale), float(decimal.offset)

    def _load_linear(self, compu_method, decimal):
        compu_scale = self._get_unique_arxml_child(compu_method,
                                                   [
                                                       'COMPU-INTERNAL-TO-PHYS',
                                                       'COMPU-SCALES',
                                                       '&COMPU-SCALE'
                                                   ])

        lower_limit = self._get_unique_arxml_child(compu_scale, '&LOWER-LIMIT')
        upper_limit = self._get_unique_arxml_child(compu_scale, '&UPPER-LIMIT')

        minimum = self._load_minimum(lower_limit, decimal)
        maximum = self._load_maximum(upper_limit, decimal)

        factor, offset = self._load_linear_factor_and_offset(
            compu_scale,
            decimal)

        return minimum, maximum, factor, offset

    def _load_scale_linear_and_texttable(self, compu_method, decimal):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = {}

        for compu_scale in self._get_arxml_children(compu_method,
                                                    [
                                                        '&COMPU-INTERNAL-TO-PHYS',
                                                        'COMPU-SCALES',
                                                        '*&COMPU-SCALE'
                                                    ]):

            lower_limit = self._get_unique_arxml_child(compu_scale, 'LOWER-LIMIT')
            upper_limit = self._get_unique_arxml_child(compu_scale, 'UPPER-LIMIT')
            vt = self._get_unique_arxml_child(compu_scale, ['&COMPU-CONST', 'VT'])

            if vt is not None:
                choices[vt.text] = int(lower_limit.text)
            else:
                minimum = self._load_minimum(lower_limit, decimal)
                maximum = self._load_maximum(upper_limit, decimal)
                factor, offset = self._load_linear_factor_and_offset(
                    compu_scale,
                    decimal)

        return minimum, maximum, factor, offset, choices

    def _load_system_signal(self, system_signal, decimal):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = None

        compu_method = self._get_compu_method(system_signal)

        if compu_method is not None:
            category = self._get_unique_arxml_child(compu_method, 'CATEGORY')

            if category is None:
                raise ValueError(
                    'CATEGORY in compu method {} does not exist.'.format(
                        compu_method.find('SHORT-NAME').text))

            category = category.text

            if category == 'TEXTTABLE':
                minimum, maximum, choices = \
                    self._load_texttable(compu_method, decimal)
            elif category == 'LINEAR':
                minimum, maximum, factor, offset = \
                    self._load_linear(compu_method, decimal)
            elif category == 'SCALE_LINEAR_AND_TEXTTABLE':
                (minimum,
                 maximum,
                 factor,
                 offset,
                 choices) = self._load_scale_linear_and_texttable(compu_method,
                                                                  decimal)
            else:
                LOGGER.debug('Compu method category %s is not yet implemented.',
                             category)

        return minimum, maximum, factor, offset, choices


    def _load_signal_type(self, i_signal):
        is_signed = False
        is_float = False

        base_type = self._get_sw_base_type(i_signal)

        if base_type is not None:
            base_type_encoding = \
                self._get_unique_arxml_child(base_type, '&BASE-TYPE-ENCODING')

            if base_type_encoding is None:
                raise ValueError(
                    'BASE-TYPE-ENCODING in base type {} does not exist.'.format(
                        base_type.find('SHORT-NAME').text))

            base_type_encoding = base_type_encoding.text

            if base_type_encoding in ('2C', '1C', 'SM'):
                # types which use two-complement, one-complement or
                # sign+magnitude encodings are signed. TODO (?): The
                # fact that if anything other than two complement
                # notation is used for negative numbers is not
                # reflected anywhere. In practice this should not
                # matter, though, since two-complement notation is
                # basically always used for systems build after
                # ~1970...
                is_signed = True
            elif base_type_encoding == 'IEEE754':
                is_float = True

        return is_signed, is_float

    def _follow_arxml_reference(self, base_elem, arxml_path, child_tag_name):
        """Follow a relative or absolute ARXML reference

        It returns the ElementTree node which corrosponds to the given
        path through the ARXML package structure. If no such node
        exists, a ValueError exception is raised.
        """

        is_absolute_path = arxml_path.startswith('/')

        if is_absolute_path and arxml_path in self._arxml_reference_cache:
            return self._arxml_reference_cache[arxml_path]

        base_elem = self._root if is_absolute_path else base_elem
        if not base_elem:
            raise ValueError(
                'Tried to dereference a relative ARXML path without '
                'specifying the base location.')

        short_names = arxml_path.lstrip('/').split('/')
        location = []

        for short_name in short_names[:-1]:
            location += [
                'AR-PACKAGES',
                "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(short_name)
            ]

        location += [
            'ELEMENTS',
            "{}/[ns:SHORT-NAME='{}']".format(child_tag_name,
                                             short_names[-1])
        ]

        result = base_elem.find(make_xpath(location), self._xml_namespaces)

        if is_absolute_path:
            self._arxml_reference_cache[arxml_path] = result

        return result

    def _get_arxml_children(self, base_elems, children_location):
        """Locate a set of ElementTree child nodes at a given location.

        This is a method that retrieves a list of ElementTree nodes
        that match a given ARXML location. An ARXML location is a list
        of strings that specify the nesting order of the XML tag
        names; potential references for entries are preceeded by an
        '&': If a sub-element that exhibits the specified name, it is
        used directly while if there is a sub-node called
        '{child_tag_name}-REF' it is assumed to contain an ARXML
        reference. This reference is then resolved and the remaining
        location specification is relative to the result of that
        resolution. If a location atom is preceeded by '*', then
        multiple sub-elements are possible. The '&' and '*' qualifiers
        may be combined.

        Example:

        # Return all frame triggerings in any physical channel of a
        # CAN cluster, where each conditional, each the physical
        # channel and its individual frame triggerings can be
        # references
        loader._get_arxml_children(can_cluster,
                                   [
                                       'CAN-CLUSTER-VARIANTS',
                                       '*&CAN-CLUSTER-CONDITIONAL',
                                       'PHYSICAL-CHANNELS',
                                       '*&CAN-PHYSICAL-CHANNEL',
                                       'FRAME-TRIGGERINGS',
                                       '*&CAN-FRAME-TRIGGERING'
                                   ])
        """

        if base_elems is None:
            raise ValueError(
                'Cannot retrieve a child element of a non-existing node!')

        # make sure that the children_location is a list. for convenience we
        # also allow it to be a string. In this case we take it that a
        # direct child node needs to be found.
        if isinstance(children_location, str):
            children_location = [ children_location ]

        # make sure that the base elements are iterable. for
        # convenience we also allow it to be an individiual node.
        if type(base_elems).__name__ == 'Element':
            base_elems = [base_elems]

        for child_tag_name in children_location:
            if len(base_elems) == 0:
                return [] # the base elements left are the empty set...

            # handle the set and reference specifiers of the current
            # sub-location
            allow_references = '&' in child_tag_name[:2]
            is_nodeset = '*' in child_tag_name[:2]

            if allow_references:
                child_tag_name = child_tag_name[1:]
            if is_nodeset:
                child_tag_name = child_tag_name[1:]

            # traverse the specified path one level deeper
            result = []
            for base_elem in base_elems:
                local_result = []

                for child_elem in base_elem:
                    if child_elem.tag == f'{{{self.xml_namespace}}}{child_tag_name}':
                        local_result.append(child_elem)
                    elif child_elem.tag == f'{{{self.xml_namespace}}}{child_tag_name}-REF':
                        tmp = self._follow_arxml_reference(base_elem,
                                                           child_elem.text,
                                                           child_elem.attrib.get('DEST'))

                        if tmp is None:
                            raise ValueError(f'Encountered dangling reference '
                                             f'{child_tag_name}-REF: '
                                             f'{child_elem.text}')

                        local_result.append(tmp)

                if not is_nodeset and len(local_result) > 1:
                    raise ValueError(f'Encountered a a non-unique child node '
                                     f'of type {child_tag_name} which ought to '
                                     f'be unique')

                result.extend(local_result)

            base_elems = result

        return base_elems

    def _get_unique_arxml_child(self, base_elem, child_location):
        """This method does the same as get_arxml_children, but it assumes
        that the location yields at most a single node.

        It returns None if no match was found and it raises ValueError
        if multiple nodes match the location, i.e., the returned
        object can be used directly if the corresponding node is
        assumed to be present.
        """

        tmp = self._get_arxml_children(base_elem, child_location)
        if len(tmp) == 0:
            return None
        elif len(tmp) == 1:
            return tmp[0]
        else:
            raise ValueError(f'{child_location} does not resolve into a unique node')

    def _get_can_frame(self, can_frame_triggering):
        return self._get_unique_arxml_child(can_frame_triggering, '&FRAME')

    def _get_i_signal(self, i_signal_to_i_pdu_mapping):
        return self._get_unique_arxml_child(i_signal_to_i_pdu_mapping,
                                            'I-SIGNAL')

    def _get_pdu(self, can_frame):
        return self._get_unique_arxml_child(can_frame,
                                            [
                                                'PDU-TO-FRAME-MAPPINGS',
                                                '&PDU-TO-FRAME-MAPPING',
                                                '&PDU'
                                            ])

    def _get_system_signal(self, i_signal):
        return self._get_unique_arxml_child(i_signal, '&SYSTEM-SIGNAL')

    def _get_compu_method(self, system_signal):
        return self._get_unique_arxml_child(system_signal,
                                            [
                                                '&PHYSICAL-PROPS',
                                                'SW-DATA-DEF-PROPS-VARIANTS',
                                                '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                                '&COMPU-METHOD'
                                            ])

    def _get_sw_base_type(self, i_signal):
        return self._get_unique_arxml_child(i_signal,
                                            [
                                                '&NETWORK-REPRESENTATION-PROPS',
                                                'SW-DATA-DEF-PROPS-VARIANTS',
                                                '&SW-DATA-DEF-PROPS-CONDITIONAL',
                                                '&BASE-TYPE'
                                            ])

# The ARXML XML namespace for the EcuExtractLoader
NAMESPACE = 'http://autosar.org/schema/r4.0'
NAMESPACES = {'ns': NAMESPACE}

ROOT_TAG = '{{{}}}AUTOSAR'.format(NAMESPACE)

# ARXML XPATHs used by the EcuExtractLoader
def make_xpath(location):
    return './ns:' + '/ns:'.join(location)

ECUC_VALUE_COLLECTION_XPATH = make_xpath([
    'AR-PACKAGES',
    'AR-PACKAGE',
    'ELEMENTS',
    'ECUC-VALUE-COLLECTION'
])
ECUC_MODULE_CONFIGURATION_VALUES_REF_XPATH = make_xpath([
    'ECUC-VALUES',
    'ECUC-MODULE-CONFIGURATION-VALUES-REF-CONDITIONAL',
    'ECUC-MODULE-CONFIGURATION-VALUES-REF'
])
ECUC_REFERENCE_VALUE_XPATH = make_xpath([
    'REFERENCE-VALUES',
    'ECUC-REFERENCE-VALUE'
])
DEFINITION_REF_XPATH = make_xpath(['DEFINITION-REF'])
VALUE_XPATH = make_xpath(['VALUE'])
VALUE_REF_XPATH = make_xpath(['VALUE-REF'])
SHORT_NAME_XPATH = make_xpath(['SHORT-NAME'])
PARAMETER_VALUES_XPATH = make_xpath(['PARAMETER-VALUES'])
REFERENCE_VALUES_XPATH = make_xpath([
    'REFERENCE-VALUES'
])

class EcuExtractLoader(object):

    def __init__(self, root, strict):
        self.root = root
        self.strict = strict

    def load(self):
        buses = []
        messages = []
        version = None

        ecuc_value_collection = self.root.find(ECUC_VALUE_COLLECTION_XPATH,
                                               NAMESPACES)
        values_refs = ecuc_value_collection.iterfind(
            ECUC_MODULE_CONFIGURATION_VALUES_REF_XPATH,
            NAMESPACES)
        com_xpaths = [
            value_ref.text
            for value_ref in values_refs
            if value_ref.text.endswith('/Com')
        ]

        if len(com_xpaths) != 1:
            raise ValueError(
                'Expected 1 /Com, but got {}.'.format(len(com_xpaths)))

        com_config = self.find_com_config(com_xpaths[0] + '/ComConfig')

        for ecuc_container_value in com_config:
            definition_ref = ecuc_container_value.find(DEFINITION_REF_XPATH,
                                                       NAMESPACES).text

            if not definition_ref.endswith('ComIPdu'):
                continue

            message = self.load_message(ecuc_container_value)

            if message is not None:
                messages.append(message)

        return InternalDatabase(messages,
                                [],
                                buses,
                                version)

    def load_message(self, com_i_pdu):
        # Default values.
        interval = None
        senders = []
        comments = None

        # Name, frame id, length and is_extended_frame.
        name = com_i_pdu.find(SHORT_NAME_XPATH, NAMESPACES).text
        direction = None

        for parameter, value in self.iter_parameter_values(com_i_pdu):
            if parameter == 'ComIPduDirection':
                direction = value
                break

        com_pdu_id_ref = None

        for reference, value in self.iter_reference_values(com_i_pdu):
            if reference == 'ComPduIdRef':
                com_pdu_id_ref = value
                break

        if com_pdu_id_ref is None:
            raise ValueError('No ComPduIdRef reference found.')

        if direction == 'SEND':
            frame_id, length, is_extended_frame = self.load_message_tx(
                com_pdu_id_ref)
        elif direction == 'RECEIVE':
            frame_id, length, is_extended_frame = self.load_message_rx(
                com_pdu_id_ref)
        else:
            raise NotImplementedError(
                'Direction {} not supported.'.format(direction))

        if frame_id is None:
            LOGGER.warning('No frame id found for message %s.', name)

            return None

        if is_extended_frame is None:
            LOGGER.warning('No frame type found for message %s.', name)

            return None

        if length is None:
            LOGGER.warning('No length found for message %s.', name)

            return None

        # ToDo: interval, senders, comments

        # Find all signals in this message.
        signals = []
        values = com_i_pdu.iterfind(ECUC_REFERENCE_VALUE_XPATH,
                                    NAMESPACES)

        for value in values:
            definition_ref = value.find(DEFINITION_REF_XPATH,
                                        NAMESPACES).text

            if not definition_ref.endswith('ComIPduSignalRef'):
                continue

            value_ref = value.find(VALUE_REF_XPATH, NAMESPACES)
            signal = self.load_signal(value_ref.text)

            if signal is not None:
                signals.append(signal)

        return Message(frame_id=frame_id,
                       is_extended_frame=is_extended_frame,
                       name=name,
                       length=length,
                       senders=senders,
                       send_type=None,
                       cycle_time=interval,
                       signals=signals,
                       comment=comments,
                       bus_name=None,
                       strict=self.strict)

    def load_message_tx(self, com_pdu_id_ref):
        return self.load_message_rx_tx(com_pdu_id_ref,
                                       'CanIfTxPduCanId',
                                       'CanIfTxPduDlc',
                                       'CanIfTxPduCanIdType')

    def load_message_rx(self, com_pdu_id_ref):
        return self.load_message_rx_tx(com_pdu_id_ref,
                                       'CanIfRxPduCanId',
                                       'CanIfRxPduDlc',
                                       'CanIfRxPduCanIdType')

    def load_message_rx_tx(self,
                           com_pdu_id_ref,
                           parameter_can_id,
                           parameter_dlc,
                           parameter_can_id_type):
        can_if_tx_pdu_cfg = self.find_can_if_rx_tx_pdu_cfg(com_pdu_id_ref)
        frame_id = None
        length = None
        is_extended_frame = None

        if can_if_tx_pdu_cfg is not None:
            for parameter, value in self.iter_parameter_values(can_if_tx_pdu_cfg):
                if parameter == parameter_can_id:
                    frame_id = int(value)
                elif parameter == parameter_dlc:
                    length = int(value)
                elif parameter == parameter_can_id_type:
                    is_extended_frame = (value == 'EXTENDED_CAN')

        return frame_id, length, is_extended_frame

    def load_signal(self, xpath):
        ecuc_container_value = self.find_value(xpath)

        if ecuc_container_value is None:
            return None

        name = ecuc_container_value.find(SHORT_NAME_XPATH, NAMESPACES).text

        # Default values.
        is_signed = False
        is_float = False
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        unit = None
        choices = None
        comments = None
        receivers = []
        decimal = SignalDecimal(Decimal(factor), Decimal(offset))

        # Bit position, length, byte order, is_signed and is_float.
        bit_position = None
        length = None
        byte_order = None

        for parameter, value in self.iter_parameter_values(ecuc_container_value):
            if parameter == 'ComBitPosition':
                bit_position = int(value)
            elif parameter == 'ComBitSize':
                length = int(value)
            elif parameter == 'ComSignalEndianness':
                byte_order = value.lower()
            elif parameter == 'ComSignalType':
                if value in ['SINT8', 'SINT16', 'SINT32']:
                    is_signed = True
                elif value in ['FLOAT32', 'FLOAT64']:
                    is_float = True

        if bit_position is None:
            LOGGER.warning('No bit position found for signal %s.',name)

            return None

        if length is None:
            LOGGER.warning('No bit size found for signal %s.', name)

            return None

        if byte_order is None:
            LOGGER.warning('No endianness found for signal %s.', name)

            return None

        # ToDo: minimum, maximum, factor, offset, unit, choices,
        #       comments and receivers.

        return Signal(name=name,
                      start=bit_position,
                      length=length,
                      receivers=receivers,
                      byte_order=byte_order,
                      is_signed=is_signed,
                      scale=factor,
                      offset=offset,
                      minimum=minimum,
                      maximum=maximum,
                      unit=unit,
                      choices=choices,
                      comment=comments,
                      is_float=is_float,
                      decimal=decimal)

    def find_com_config(self, xpath):
        return self.root.find(make_xpath([
            "AR-PACKAGES",
            "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[1]),
            "ELEMENTS",
            "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='Com']",
            "CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='ComConfig']",
            "SUB-CONTAINERS"
        ]),
                              NAMESPACES)

    def find_value(self, xpath):
        return self.root.find(make_xpath([
            "AR-PACKAGES",
            "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[1]),
            "ELEMENTS",
            "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='Com']",
            "CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='ComConfig']",
            "SUB-CONTAINERS",
            "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='{}']".format(xpath.split('/')[-1])
        ]),
                              NAMESPACES)

    def find_can_if_rx_tx_pdu_cfg(self, com_pdu_id_ref):
        messages = self.root.iterfind(
            make_xpath([
                "AR-PACKAGES",
                "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(
                    com_pdu_id_ref.split('/')[1]),
                "ELEMENTS",
                "ECUC-MODULE-CONFIGURATION-VALUES/[ns:SHORT-NAME='CanIf']",
                'CONTAINERS',
                "ECUC-CONTAINER-VALUE/[ns:SHORT-NAME='CanIfInitCfg']",
                'SUB-CONTAINERS',
                'ECUC-CONTAINER-VALUE'
            ]),
            NAMESPACES)

        for message in messages:
            definition_ref = message.find(DEFINITION_REF_XPATH,
                                          NAMESPACES).text

            if definition_ref.endswith('CanIfTxPduCfg'):
                expected_reference = 'CanIfTxPduRef'
            elif definition_ref.endswith('CanIfRxPduCfg'):
                expected_reference = 'CanIfRxPduRef'
            else:
                continue

            for reference, value in self.iter_reference_values(message):
                if reference == expected_reference:
                    if value == com_pdu_id_ref:
                        return message

    def iter_parameter_values(self, param_conf_container):
        parameters = param_conf_container.find(PARAMETER_VALUES_XPATH,
                                               NAMESPACES)

        if parameters is None:
            raise ValueError('PARAMETER-VALUES does not exist.')

        for parameter in parameters:
            definition_ref = parameter.find(DEFINITION_REF_XPATH,
                                            NAMESPACES).text
            value = parameter.find(VALUE_XPATH, NAMESPACES).text
            name = definition_ref.split('/')[-1]

            yield name, value

    def iter_reference_values(self, param_conf_container):
        references = param_conf_container.find(REFERENCE_VALUES_XPATH,
                                               NAMESPACES)

        if references is None:
            raise ValueError('REFERENCE-VALUES does not exist.')

        for reference in references:
            definition_ref = reference.find(DEFINITION_REF_XPATH,
                                            NAMESPACES).text
            value = reference.find(VALUE_REF_XPATH, NAMESPACES).text
            name = definition_ref.split('/')[-1]

            yield name, value


def is_ecu_extract(root):
    ecuc_value_collection = root.find(ECUC_VALUE_COLLECTION_XPATH,
                                      NAMESPACES)

    return ecuc_value_collection is not None


def load_string(string, strict=True):
    """Parse given ARXML format string.

    """

    root = ElementTree.fromstring(string)

    m = re.match("{(.*)}AUTOSAR", root.tag)
    if not m:
        raise ValueError(f"No XML namespace specified or illegal root tag name '{root.tag}'")
    xml_namespace = m.group(1)

    # Should be replaced with a validation using the XSD file.
    recognized_namespace = False
    if re.match("http://autosar.org/schema/r(.*)", xml_namespace):
        recognized_namespace = True

    if not recognized_namespace:
        raise ValueError(f"Unrecognized XML namespace '{xml_namespace}'")

    if is_ecu_extract(root):
        if root.tag != ROOT_TAG:
            raise ValueError(
                'Expected root element tag {}, but got {}.'.format(
                    ROOT_TAG,
                    root.tag))

        return EcuExtractLoader(root, strict).load()
    else:
        return SystemLoader(root, strict).load()
