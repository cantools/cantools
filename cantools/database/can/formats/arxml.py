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
    'COMPU-INTERNAL-TO-PHYS',
    'COMPU-SCALES',
    'COMPU-SCALE',
    'COMPU-RATIONAL-COEFFS',
    'COMPU-NUMERATOR',
    'V'
])
COMPU_DENOMINATOR_XPATH = make_xpath([
    'COMPU-INTERNAL-TO-PHYS',
    'COMPU-SCALES',
    'COMPU-SCALE',
    'COMPU-RATIONAL-COEFFS',
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
LOWER_LIMIT_XPATH = make_xpath([
    'LOWER-LIMIT'
])
UPPER_LIMIT_XPATH = make_xpath([
    'UPPER-LIMIT'
])


class Loader(object):

    def __init__(self, string, strict):
        self.string = string
        self.strict = strict
        self.root = None

    def load(self):
        self.root = ElementTree.fromstring(self.string)

        # Should be replaced with a validation using the XSD file.
        if self.root.tag != ROOT_TAG:
            raise ValueError(
                'Expected root element tag {}, but got {}.'.format(
                    ROOT_TAG,
                    self.root.tag))

        buses = []
        messages = []
        version = None

        can_frame_triggerings = self.root.findall(CAN_FRAME_TRIGGERINGS_XPATH,
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
        interval = None
        senders = []
        comment = None

        frame_ref_xpath = can_frame_triggering.find(FRAME_REF_XPATH,
                                                    NAMESPACES).text
        can_frame = self.find_can_frame(frame_ref_xpath)

        name = can_frame_triggering.find(SHORT_NAME_XPATH, NAMESPACES).text
        frame_id = int(can_frame_triggering.find(IDENTIFIER_XPATH,
                                                 NAMESPACES).text)
        length = int(can_frame.find(FRAME_LENGTH_XPATH, NAMESPACES).text)
        can_addressing_mode = can_frame_triggering.find(
            CAN_ADDRESSING_MODE_XPATH,
            NAMESPACES).text
        is_extended_frame = (can_addressing_mode == 'EXTENDED')

        # Comment.
        l_2 = can_frame.find(DESC_L_2_XPATH, NAMESPACES)

        if l_2 is not None:
            comment = l_2.text

        # ToDo: interval, senders

        # Find all signals in this message.
        signals = []

        pdu_ref_xpath = can_frame.find(PDU_REF_XPATH, NAMESPACES).text
        i_signal_i_pdu = self.find_i_signal_i_pdu(pdu_ref_xpath)
        i_signal_to_i_pdu_mappings = i_signal_i_pdu.findall(
            I_SIGNAL_TO_I_PDU_MAPPING_XPATH,
            NAMESPACES)

        for i_signal_to_i_pdu_mapping in i_signal_to_i_pdu_mappings:
            signals.append(self.load_signal(i_signal_to_i_pdu_mapping))

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

    def load_texttable(self, compu_method, decimal):
        # Default values.
        minimum = None
        maximum = None
        choices = {}

        for compu_scale in compu_method.findall(COMPU_SCALE_XPATH, NAMESPACES):
            lower_limit = compu_scale.find(LOWER_LIMIT_XPATH, NAMESPACES)
            upper_limit = compu_scale.find(UPPER_LIMIT_XPATH, NAMESPACES)
            vt = compu_scale.find(VT_XPATH, NAMESPACES)

            if vt is not None:
                choices[vt.text] = int(lower_limit.text)
            else:
                if lower_limit is not None:
                    decimal.minimum = Decimal(lower_limit.text)
                    minimum = float(decimal.minimum)

                if upper_limit is not None:
                    decimal.maximum = Decimal(upper_limit.text)
                    maximum = float(decimal.maximum)

        return minimum, maximum, choices

    def load_linear(self, compu_method, decimal):
        # Minimum.
        minimum = compu_method.find(PHYS_LOWER_LIMIT_XPATH, NAMESPACES)

        if minimum is not None:
            decimal.minimum = Decimal(minimum.text)
            minimum = float(decimal.minimum)

        # Maximum.
        maximum = compu_method.find(PHYS_UPPER_LIMIT_XPATH, NAMESPACES)

        if maximum is not None:
            decimal.maximum = Decimal(maximum.text)
            maximum = float(decimal.maximum)

        # Factor and offset.
        numerators = compu_method.findall(COMPU_NUMERATOR_XPATH,
                                          NAMESPACES)

        if len(numerators) != 2:
            raise ValueError(
                'Expected 2 numerator values for linear scaling, but '
                'got {}.'.format(len(numerators)))

        denominators = compu_method.findall(COMPU_DENOMINATOR_XPATH,
                                            NAMESPACES)

        if len(denominators) != 1:
            raise ValueError(
                'Expected 1 denominator value for linear scaling, but '
                'got {}.'.format(len(denominators)))

        denominator = Decimal(denominators[0].text)
        decimal.scale = Decimal(numerators[1].text) / denominator
        decimal.offset = Decimal(numerators[0].text) / denominator
        factor = float(decimal.scale)
        offset = float(decimal.offset)

        return minimum, maximum, factor, offset

    def load_signal(self, i_signal_to_i_pdu_mapping):
        """Load given signal and return a signal object.

        """

        # Default values.
        byte_order = 'little_endian'
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

        # Name, start position and length.
        i_signal_ref_xpath = i_signal_to_i_pdu_mapping.find(
            I_SIGNAL_REF_XPATH,
            NAMESPACES).text
        i_signal = self.find_i_signal(i_signal_ref_xpath)

        name = i_signal_to_i_pdu_mapping.find(SHORT_NAME_XPATH,
                                              NAMESPACES).text
        start_position = int(i_signal_to_i_pdu_mapping.find(
            START_POSITION_XPATH,
            NAMESPACES).text)
        length = int(i_signal.find(LENGTH_XPATH, NAMESPACES).text)

        # Byte order.
        packing_byte_order = i_signal_to_i_pdu_mapping.find(
            PACKING_BYTE_ORDER_XPATH,
            NAMESPACES).text

        if packing_byte_order == 'MOST-SIGNIFICANT-BYTE-FIRST':
            byte_order = 'big_endian'

        system_signal_ref = i_signal.find(SYSTEM_SIGNAL_REF_XPATH,
                                          NAMESPACES)

        if system_signal_ref is not None:
            system_signal = self.find_system_signal(system_signal_ref.text)

            # Unit.
            unit_ref = system_signal.find(UNIT_REF_XPATH, NAMESPACES)

            try:
                unit = self.find_unit(unit_ref.text).find(DISPLAY_NAME_XPATH,
                                                          NAMESPACES).text
            except AttributeError:
                pass

            # Comment.
            l_2 = system_signal.find(DESC_L_2_XPATH, NAMESPACES)

            if l_2 is not None:
                comment = l_2.text

            # Minimum and maximum.
            compu_method_ref = system_signal.find(COMPU_METHOD_REF_XPATH,
                                                  NAMESPACES)
            try:
                compu_method = self.find_compu_method(compu_method_ref.text)
                category = compu_method.find(CATEGORY_XPATH, NAMESPACES).text

                if category == 'TEXTTABLE':
                    minimum, maximum, choices = self.load_texttable(
                        compu_method,
                        decimal)
                elif category == 'LINEAR':
                    minimum, maximum, factor, offset = self.load_linear(
                        compu_method,
                        decimal)
                else:
                    raise NotImplementedError(
                        'Category {} is not yet implemented.'.format(category))
            except AttributeError:
                pass


        # ToDo: is_signed, is_float, receivers

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

    def find_system_signal(self, xpath):
        return self.find('SYSTEM-SIGNAL', xpath)

    def find_unit(self, xpath):
        return self.find('UNIT', xpath)

    def find_compu_method(self, xpath):
        return self.find('COMPU-METHOD', xpath)


def load_string(string, strict=True):
    """Parse given ARXML format string.

    """

    return Loader(string, strict).load()
