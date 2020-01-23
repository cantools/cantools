# Load and dump a CAN database in ARXML format.

import logging
from decimal import Decimal

from xml.etree import ElementTree

from ..signal import Signal
from ..signal import Decimal as SignalDecimal
from ..message import Message
from ..internal_database import InternalDatabase


LOGGER = logging.getLogger(__name__)

# The ARXML XML namespace.
NAMESPACE = 'http://autosar.org/schema/r4.0'
NAMESPACES = {'ns': NAMESPACE}

ROOT_TAG = '{{{}}}AUTOSAR'.format(NAMESPACE)


def make_xpath(location):
    return './ns:' + '/ns:'.join(location)


# ARXML XPATHs.
CAN_FRAME_TRIGGERINGS_XPATH = make_xpath([
    'AR-PACKAGES',
    'AR-PACKAGE',
    'ELEMENTS',
    'CAN-CLUSTER',
    'CAN-CLUSTER-VARIANTS',
    'CAN-CLUSTER-CONDITIONAL',
    'PHYSICAL-CHANNELS',
    'CAN-PHYSICAL-CHANNEL',
    'FRAME-TRIGGERINGS',
    'CAN-FRAME-TRIGGERING'
])
FRAME_REF_XPATH = make_xpath(['FRAME-REF'])
SHORT_NAME_XPATH = make_xpath(['SHORT-NAME'])
IDENTIFIER_XPATH = make_xpath(['IDENTIFIER'])
FRAME_LENGTH_XPATH = make_xpath(['FRAME-LENGTH'])
CAN_ADDRESSING_MODE_XPATH = make_xpath(['CAN-ADDRESSING-MODE'])
PDU_REF_XPATH = make_xpath([
    'PDU-TO-FRAME-MAPPINGS',
    'PDU-TO-FRAME-MAPPING',
    'PDU-REF'
])
I_SIGNAL_TO_I_PDU_MAPPING_XPATH = make_xpath([
    'I-SIGNAL-TO-PDU-MAPPINGS',
    'I-SIGNAL-TO-I-PDU-MAPPING'
])
TIME_PERIOD_XPATH = make_xpath([
    'I-PDU-TIMING-SPECIFICATIONS',
    'I-PDU-TIMING',
    'TRANSMISSION-MODE-DECLARATION',
    'TRANSMISSION-MODE-TRUE-TIMING',
    'CYCLIC-TIMING',
    'TIME-PERIOD',
    'VALUE'
])
I_SIGNAL_REF_XPATH = make_xpath(['I-SIGNAL-REF'])
START_POSITION_XPATH = make_xpath(['START-POSITION'])
LENGTH_XPATH = make_xpath(['LENGTH'])
PACKING_BYTE_ORDER_XPATH = make_xpath(['PACKING-BYTE-ORDER'])
DESC_L_2_XPATH = make_xpath(['DESC', 'L-2'])
SYSTEM_SIGNAL_REF_XPATH = make_xpath(['SYSTEM-SIGNAL-REF'])
UNIT_REF_XPATH = make_xpath([
    'PHYSICAL-PROPS',
    'SW-DATA-DEF-PROPS-VARIANTS',
    'SW-DATA-DEF-PROPS-CONDITIONAL',
    'UNIT-REF'
])
COMPU_METHOD_REF_XPATH = make_xpath([
    'PHYSICAL-PROPS',
    'SW-DATA-DEF-PROPS-VARIANTS',
    'SW-DATA-DEF-PROPS-CONDITIONAL',
    'COMPU-METHOD-REF'
])
DISPLAY_NAME_XPATH = make_xpath(['DISPLAY-NAME'])
CATEGORY_XPATH = make_xpath(['CATEGORY'])
COMPU_NUMERATOR_XPATH = make_xpath([
    'COMPU-NUMERATOR',
    'V'
])
COMPU_RATIONAL_COEFFS_XPATH = make_xpath([
    'COMPU-RATIONAL-COEFFS'
])
COMPU_DENOMINATOR_XPATH = make_xpath([
    'COMPU-DENOMINATOR',
    'V'
])
PHYS_LOWER_LIMIT_XPATH = make_xpath([
    'COMPU-INTERNAL-TO-PHYS',
    'COMPU-SCALES',
    'COMPU-SCALE',
    'LOWER-LIMIT'
])
PHYS_UPPER_LIMIT_XPATH = make_xpath([
    'COMPU-INTERNAL-TO-PHYS',
    'COMPU-SCALES',
    'COMPU-SCALE',
    'UPPER-LIMIT'
])
COMPU_SCALE_XPATH = make_xpath([
    'COMPU-INTERNAL-TO-PHYS',
    'COMPU-SCALES',
    'COMPU-SCALE'
])
VT_XPATH = make_xpath([
    'COMPU-CONST',
    'VT'
])
LOWER_LIMIT_XPATH = make_xpath(['LOWER-LIMIT'])
UPPER_LIMIT_XPATH = make_xpath(['UPPER-LIMIT'])
BASE_TYPE_ENCODING_XPATH = make_xpath(['BASE-TYPE-ENCODING'])
BASE_TYPE_REF_XPATH = make_xpath([
    'NETWORK-REPRESENTATION-PROPS',
    'SW-DATA-DEF-PROPS-VARIANTS',
    'SW-DATA-DEF-PROPS-CONDITIONAL',
    'BASE-TYPE-REF'
])
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
VALUE_REF_XPATH = make_xpath(['VALUE-REF'])
PARAMETER_VALUES_XPATH = make_xpath(['PARAMETER-VALUES'])
DEFINITION_REF_XPATH = make_xpath(['DEFINITION-REF'])
VALUE_XPATH = make_xpath(['VALUE'])
REFERENCE_VALUES_XPATH = make_xpath([
    'REFERENCE-VALUES'
])


class SystemLoader(object):

    def __init__(self, root, strict):
        self.root = root
        self.strict = strict
        self._system_signal_cache = {}
        self._compu_method_cache = {}
        self._sw_base_type_cache = {}

    def load(self):
        buses = []
        messages = []
        version = None

        can_frame_triggerings = self.root.iterfind(CAN_FRAME_TRIGGERINGS_XPATH,
                                                   NAMESPACES)

        for can_frame_triggering in can_frame_triggerings:
            messages.append(self.load_message(can_frame_triggering))

        return InternalDatabase(messages,
                                [],
                                buses,
                                version)

    def load_message(self, can_frame_triggering):
        """Load given message and return a message object.

        """

        # Default values.
        cycle_time = None
        senders = []

        frame_ref_xpath = can_frame_triggering.find(FRAME_REF_XPATH,
                                                    NAMESPACES).text
        can_frame = self.find_can_frame(frame_ref_xpath)

        # Name, frame id, length, is_extended_frame and comment.
        name = self.load_message_name(can_frame)
        frame_id = self.load_message_frame_id(can_frame_triggering)
        length = self.load_message_length(can_frame)
        is_extended_frame = self.load_message_is_extended_frame(
            can_frame_triggering)
        comment = self.load_message_comment(can_frame)

        # ToDo: senders

        # Find all signals in this message.
        signals = []

        pdu_ref_xpath = can_frame.find(PDU_REF_XPATH, NAMESPACES).text
        i_signal_i_pdu = self.find_i_signal_i_pdu(pdu_ref_xpath)

        if i_signal_i_pdu is not None:
            time_period = i_signal_i_pdu.find(TIME_PERIOD_XPATH, NAMESPACES)

            if time_period is not None:
                cycle_time = int(float(time_period.text) * 1000)

            i_signal_to_i_pdu_mappings = i_signal_i_pdu.iterfind(
                I_SIGNAL_TO_I_PDU_MAPPING_XPATH,
                NAMESPACES)

            for i_signal_to_i_pdu_mapping in i_signal_to_i_pdu_mappings:
                signal = self.load_signal(i_signal_to_i_pdu_mapping)

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
                       comment=comment,
                       bus_name=None,
                       strict=self.strict)

    def load_message_name(self, can_frame_triggering):
        return can_frame_triggering.find(SHORT_NAME_XPATH, NAMESPACES).text

    def load_message_frame_id(self, can_frame_triggering):
        return int(can_frame_triggering.find(IDENTIFIER_XPATH,
                                             NAMESPACES).text)

    def load_message_length(self, can_frame):
        return int(can_frame.find(FRAME_LENGTH_XPATH, NAMESPACES).text)

    def load_message_is_extended_frame(self, can_frame_triggering):
        can_addressing_mode = can_frame_triggering.find(
            CAN_ADDRESSING_MODE_XPATH,
            NAMESPACES).text

        return can_addressing_mode == 'EXTENDED'

    def load_message_comment(self, can_frame):
        l_2 = can_frame.find(DESC_L_2_XPATH, NAMESPACES)

        if l_2 is not None:
            return l_2.text
        else:
            return None

    def load_signal(self, i_signal_to_i_pdu_mapping):
        """Load given signal and return a signal object.

        """

        # Default values.
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        unit = None
        choices = None
        comment = None
        receivers = []
        decimal = SignalDecimal(Decimal(factor), Decimal(offset))

        i_signal_ref = i_signal_to_i_pdu_mapping.find(
            I_SIGNAL_REF_XPATH,
            NAMESPACES)

        if i_signal_ref is None:
            # Probably a signal group (I-SIGNAL-GROUP-REF).
            return None

        i_signal = self.find_i_signal(i_signal_ref.text)

        # Name, start position, length and byte order.
        name = self.load_signal_name(i_signal_to_i_pdu_mapping)
        start_position = self.load_signal_start_position(
            i_signal_to_i_pdu_mapping)
        length = self.load_signal_length(i_signal)
        byte_order = self.load_signal_byte_order(i_signal_to_i_pdu_mapping)

        system_signal_ref = i_signal.find(SYSTEM_SIGNAL_REF_XPATH,
                                          NAMESPACES)

        if system_signal_ref is not None:
            system_signal = self.get_system_signal(system_signal_ref.text)

            # Unit and comment.
            unit = self.load_signal_unit(system_signal)
            comment = self.load_signal_comment(system_signal)

            # Minimum, maximum, factor, offset and choices.
            minimum, maximum, factor, offset, choices = self.load_system_signal(
                system_signal,
                decimal)

        # Type.
        is_signed, is_float = self.load_signal_type(i_signal)

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
                      comment=comment,
                      is_float=is_float,
                      decimal=decimal)

    def load_signal_name(self, i_signal_to_i_pdu_mapping):
        return i_signal_to_i_pdu_mapping.find(SHORT_NAME_XPATH,
                                              NAMESPACES).text

    def load_signal_start_position(self, i_signal_to_i_pdu_mapping):
        return int(i_signal_to_i_pdu_mapping.find(
            START_POSITION_XPATH,
            NAMESPACES).text)

    def load_signal_length(self, i_signal):
        return int(i_signal.find(LENGTH_XPATH, NAMESPACES).text)

    def load_signal_byte_order(self, i_signal_to_i_pdu_mapping):
        packing_byte_order = i_signal_to_i_pdu_mapping.find(
            PACKING_BYTE_ORDER_XPATH,
            NAMESPACES).text

        if packing_byte_order == 'MOST-SIGNIFICANT-BYTE-FIRST':
            return 'big_endian'
        else:
            return 'little_endian'

    def load_signal_unit(self, system_signal):
        unit_ref = system_signal.find(UNIT_REF_XPATH, NAMESPACES)

        if unit_ref is not None:
            return self.find_unit(unit_ref.text).find(DISPLAY_NAME_XPATH,
                                                      NAMESPACES).text

    def load_signal_comment(self, system_signal):
        l_2 = system_signal.find(DESC_L_2_XPATH, NAMESPACES)

        if l_2 is not None:
            return l_2.text
        else:
            return None

    def load_minimum(self, minimum, decimal):
        if minimum is not None:
            decimal.minimum = Decimal(minimum.text)
            minimum = float(decimal.minimum)

        return minimum

    def load_maximum(self, maximum, decimal):
        if maximum is not None:
            decimal.maximum = Decimal(maximum.text)
            maximum = float(decimal.maximum)

        return maximum

    def load_texttable(self, compu_method, decimal):
        minimum = None
        maximum = None
        choices = {}

        for compu_scale in compu_method.iterfind(COMPU_SCALE_XPATH, NAMESPACES):
            lower_limit = compu_scale.find(LOWER_LIMIT_XPATH, NAMESPACES)
            upper_limit = compu_scale.find(UPPER_LIMIT_XPATH, NAMESPACES)
            vt = compu_scale.find(VT_XPATH, NAMESPACES)

            if vt is not None:
                choices[vt.text] = int(lower_limit.text)
            else:
                minimum = self.load_minimum(lower_limit, decimal)
                maximum = self.load_maximum(upper_limit, decimal)

        return minimum, maximum, choices

    def load_linear_factor_and_offset(self, compu_scale, decimal):
        compu_rational_coeffs = compu_scale.find(
            COMPU_RATIONAL_COEFFS_XPATH,
            NAMESPACES)

        if compu_rational_coeffs is None:
            return 1, 0

        numerators = compu_rational_coeffs.findall(COMPU_NUMERATOR_XPATH,
                                                   NAMESPACES)

        if len(numerators) != 2:
            raise ValueError(
                'Expected 2 numerator values for linear scaling, but '
                'got {}.'.format(len(numerators)))

        denominators = compu_rational_coeffs.findall(COMPU_DENOMINATOR_XPATH,
                                                     NAMESPACES)

        if len(denominators) != 1:
            raise ValueError(
                'Expected 1 denominator value for linear scaling, but '
                'got {}.'.format(len(denominators)))

        denominator = Decimal(denominators[0].text)
        decimal.scale = Decimal(numerators[1].text) / denominator
        decimal.offset = Decimal(numerators[0].text) / denominator

        return float(decimal.scale), float(decimal.offset)

    def load_linear(self, compu_method, decimal):
        compu_scale = compu_method.find(COMPU_SCALE_XPATH,
                                        NAMESPACES)

        lower_limit = compu_scale.find(LOWER_LIMIT_XPATH, NAMESPACES)
        upper_limit = compu_scale.find(UPPER_LIMIT_XPATH, NAMESPACES)

        minimum = self.load_minimum(lower_limit, decimal)
        maximum = self.load_maximum(upper_limit, decimal)

        factor, offset = self.load_linear_factor_and_offset(
            compu_scale,
            decimal)

        return minimum, maximum, factor, offset

    def load_scale_linear_and_texttable(self, compu_method, decimal):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = {}

        for compu_scale in compu_method.iterfind(COMPU_SCALE_XPATH, NAMESPACES):
            lower_limit = compu_scale.find(LOWER_LIMIT_XPATH, NAMESPACES)
            upper_limit = compu_scale.find(UPPER_LIMIT_XPATH, NAMESPACES)
            vt = compu_scale.find(VT_XPATH, NAMESPACES)

            if vt is not None:
                choices[vt.text] = int(lower_limit.text)
            else:
                minimum = self.load_minimum(lower_limit, decimal)
                maximum = self.load_maximum(upper_limit, decimal)
                factor, offset = self.load_linear_factor_and_offset(
                    compu_scale,
                    decimal)

        return minimum, maximum, factor, offset, choices

    def load_system_signal(self, system_signal, decimal):
        minimum = None
        maximum = None
        factor = 1
        offset = 0
        choices = None

        compu_method_ref = system_signal.find(COMPU_METHOD_REF_XPATH,
                                              NAMESPACES)

        if compu_method_ref is not None:
            compu_method = self.get_compu_method(compu_method_ref.text)
            category = compu_method.find(CATEGORY_XPATH, NAMESPACES)

            if category is None:
                raise ValueError(
                    'CATEGORY in {} does not exist.'.format(
                        compu_method_ref.text))

            category = category.text

            if category == 'TEXTTABLE':
                minimum, maximum, choices = self.load_texttable(
                    compu_method,
                    decimal)
            elif category == 'LINEAR':
                minimum, maximum, factor, offset = self.load_linear(
                    compu_method,
                    decimal)
            elif category == 'SCALE_LINEAR_AND_TEXTTABLE':
                (minimum,
                 maximum,
                 factor,
                 offset,
                 choices) = self.load_scale_linear_and_texttable(
                     compu_method,
                     decimal)
            else:
                LOGGER.debug('Category %s is not yet implemented.', category)

        return minimum, maximum, factor, offset, choices


    def load_signal_type(self, i_signal):
        is_signed = False
        is_float = False

        base_type_ref = i_signal.find(BASE_TYPE_REF_XPATH, NAMESPACES)

        if base_type_ref is not None:
            sw_base_type = self.get_sw_base_type(base_type_ref.text)
            base_type_encoding = sw_base_type.find(BASE_TYPE_ENCODING_XPATH,
                                                   NAMESPACES)

            if base_type_encoding is None:
                raise ValueError(
                    'BASE-TYPE-ENCODING in {} does not exist.'.format(
                        base_type_ref.text))

            base_type_encoding = base_type_encoding.text

            if base_type_encoding == '2C':
                is_signed = True
            elif base_type_encoding == 'IEEE754':
                is_float = True

        return is_signed, is_float

    def find(self, child_elem, xpath):
        short_names = xpath.lstrip('/').split('/')
        location = []

        for short_name in short_names[:-1]:
            location += [
                'AR-PACKAGES',
                "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(short_name)
            ]

        location += [
            'ELEMENTS',
            "{}/[ns:SHORT-NAME='{}']".format(child_elem,
                                             short_names[-1])
        ]

        return self.root.find(make_xpath(location), NAMESPACES)

    def find_can_frame(self, xpath):
        return self.find('CAN-FRAME', xpath)

    def find_i_signal(self, xpath):
        return self.find('I-SIGNAL', xpath)

    def find_i_signal_i_pdu(self, xpath):
        return self.find('I-SIGNAL-I-PDU', xpath)

    def get_system_signal(self, xpath):
        if xpath in self._system_signal_cache:
            system_signal = self._system_signal_cache[xpath]
        else:
            system_signal = self.find('SYSTEM-SIGNAL', xpath)

            if system_signal is None:
                raise ValueError(
                    'SYSTEM-SIGNAL at {} does not exist.'.format(xpath))

            self._system_signal_cache[xpath] = system_signal

        return system_signal

    def find_unit(self, xpath):
        unit = self.find('UNIT', xpath)

        if unit is None:
            raise ValueError(
                'UNIT at {} does not exist.'.format(xpath))

        return unit

    def get_compu_method(self, xpath):
        if xpath in self._compu_method_cache:
            compu_method = self._compu_method_cache[xpath]
        else:
            compu_method = self.find('COMPU-METHOD', xpath)

            if compu_method is None:
                raise ValueError(
                    'COMPU-METHOD at {} does not exist.'.format(xpath))

            self._compu_method_cache[xpath] = compu_method

        return compu_method

    def get_sw_base_type(self, xpath):
        if xpath in self._sw_base_type_cache:
            sw_base_type = self._sw_base_type_cache[xpath]
        else:
            sw_base_type = self.find('SW-BASE-TYPE', xpath)

            if sw_base_type is None:
                raise ValueError(
                    'SW-BASE-TYPE at {} does not exist.'.format(xpath))

            self._sw_base_type_cache[xpath] = sw_base_type

        return sw_base_type


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
        comment = None

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

        # ToDo: interval, senders, comment

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
                       comment=comment,
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
        comment = None
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
        #       comment and receivers.

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
                      comment=comment,
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

    # Should be replaced with a validation using the XSD file.
    if root.tag != ROOT_TAG:
        raise ValueError(
            'Expected root element tag {}, but got {}.'.format(
                ROOT_TAG,
                root.tag))

    if is_ecu_extract(root):
        return EcuExtractLoader(root, strict).load()
    else:
        return SystemLoader(root, strict).load()
