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


def make_xpath(*args):
    return './ns:' + '/ns:'.join(args)


# ARXML XPATHs.
CAN_FRAME_TRIGGERINGS_XPATH = make_xpath('AR-PACKAGES',
                                         'AR-PACKAGE',
                                         'ELEMENTS',
                                         'CAN-CLUSTER',
                                         'CAN-CLUSTER-VARIANTS',
                                         'CAN-CLUSTER-CONDITIONAL',
                                         'PHYSICAL-CHANNELS',
                                         'CAN-PHYSICAL-CHANNEL',
                                         'FRAME-TRIGGERINGS',
                                         'CAN-FRAME-TRIGGERING')
FRAME_REF_XPATH = make_xpath('FRAME-REF')
SHORT_NAME_XPATH = make_xpath('SHORT-NAME')
IDENTIFIER_XPATH = make_xpath('IDENTIFIER')
FRAME_LENGTH_XPATH = make_xpath('FRAME-LENGTH')
CAN_ADDRESSING_MODE_XPATH = make_xpath('CAN-ADDRESSING-MODE')
PDU_REF_XPATH = make_xpath('PDU-TO-FRAME-MAPPINGS',
                           'PDU-TO-FRAME-MAPPING',
                           'PDU-REF')
I_SIGNAL_TO_I_PDU_MAPPING_XPATH = make_xpath('I-SIGNAL-TO-PDU-MAPPINGS',
                                             'I-SIGNAL-TO-I-PDU-MAPPING')
I_SIGNAL_REF_XPATH = make_xpath('I-SIGNAL-REF')
START_POSITION_XPATH = make_xpath('START-POSITION')
LENGTH_XPATH = make_xpath('LENGTH')
PACKING_BYTE_ORDER_XPATH = make_xpath('PACKING-BYTE-ORDER')


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
                       comment=None,
                       bus_name=None,
                       strict=self.strict)

    def load_signal(self, i_signal_to_i_pdu_mapping):
        """Load given signal and return a signal object.

        """

        # Default values.
        byte_order = 'little_endian'
        is_signed = False
        is_float = False
        minimum = None
        maximum = None
        slope = 1
        intercept = 0
        unit = None
        labels = None
        notes = None
        receivers = []
        decimal = SignalDecimal(Decimal(slope), Decimal(intercept))

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

        return Signal(name=name,
                      start=start_position,
                      length=length,
                      receivers=receivers,
                      byte_order=byte_order,
                      is_signed=is_signed,
                      scale=slope,
                      offset=intercept,
                      minimum=minimum,
                      maximum=maximum,
                      unit=unit,
                      choices=labels,
                      comment=notes,
                      is_float=is_float,
                      decimal=decimal)

    def find(self, child_elem, xpath):
        package_short_name, child_short_name = xpath.lstrip('/').split('/')

        return self.root.find(
            make_xpath('AR-PACKAGES',
                       "AR-PACKAGE/[ns:SHORT-NAME='{}']".format(
                           package_short_name),
                       'ELEMENTS',
                       "{}/[ns:SHORT-NAME='{}']".format(child_elem,
                                                        child_short_name)),
            NAMESPACES)

    def find_can_frame(self, xpath):
        return self.find('CAN-FRAME', xpath)

    def find_i_signal(self, xpath):
        return self.find('I-SIGNAL', xpath)

    def find_i_signal_i_pdu(self, xpath):
        return self.find('I-SIGNAL-I-PDU', xpath)


def load_string(string, strict=True):
    """Parse given ARXML format string.

    """

    return Loader(string, strict).load()
