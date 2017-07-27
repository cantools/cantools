# Load and dump a CAN database in KCD format.

from xml.etree.ElementTree import fromstring

from ..signal import Signal
from ..message import Message
from ..node import Node
from ..database import Database

from .utils import num


_NAMESPACES = {'ns': 'http://kayak.2codeornot2code.org/1.0'}


def _load_signal_element(signal):
    """Load given signal element and return a signal object.

    """

    # Default values.
    length = 1
    byte_order = 'big_endian'
    is_signed = False
    minimum = None
    maximum = None
    slope = 1
    intercept = 0
    unit = None

    # Signal XML attributes.
    for key, value in signal.attrib.items():
        if key == 'length':
            length = int(signal.attrib['length'])
        elif key == 'endianess':
            byte_order = '{}_endian'.format(signal.attrib['endianess'])

    # Value XML element.
    value = signal.find('ns:Value', _NAMESPACES)

    if value is not None:
        for key, value in value.attrib.items():
            if key == 'min':
                minimum = num(value)
            elif key == 'max':
                maximum = num(value)
            elif key == 'slope':
                slope = num(value)
            elif key == 'intercept':
                intercept = num(value)
            elif key == 'unit':
                unit = value
            elif key == 'type':
                is_signed = (value == 'signed')

    # Notes.
    try:
        notes = signal.find('ns:Notes', _NAMESPACES).text
    except AttributeError:
        notes = None

    return Signal(name=signal.attrib['name'],
                  start=int(signal.attrib['offset']),
                  length=length,
                  nodes=[],
                  byte_order=byte_order,
                  is_signed=is_signed,
                  scale=slope,
                  offset=intercept,
                  minimum=minimum,
                  maximum=maximum,
                  unit=unit,
                  choices=None,
                  comment=notes,
                  is_multiplexer=False,
                  multiplexer_id=None)


def _load_message_element(message):
    """Load given message element and return a message object.

    """

    # Frame id.
    frame_id = int(message.attrib['id'], 0)

    # Extended format.
    try:
        is_extended_frame = (message.attrib['format'] == 'extended')
    except KeyError:
        is_extended_frame = False

    # Comment.
    try:
        comment = message.find('ns:Notes', _NAMESPACES).text
    except AttributeError:
        comment = None

    # Find all signals in this message.
    signals = []

    for signal in message.findall('ns:Signal', _NAMESPACES):
        signals.append(_load_signal_element(signal))

    return Message(frame_id=frame_id,
                   is_extended_frame=is_extended_frame,
                   name=message.attrib['name'],
                   length=8,
                   nodes=[],
                   send_type=None,
                   cycle_time=None,
                   signals=signals,
                   comment=comment)


def dump_string(database):
    """Format given database in KCD file format.

    """

    return str(database)


def load_string(string):
    """Parse given KCD format string.

    """

    root = fromstring(string)
    nodes = [node.attrib for node in root.findall('./ns:Node', _NAMESPACES)]
    messages = []

    for message in root.findall('./ns:Bus/ns:Message', _NAMESPACES):
        messages.append(_load_message_element(message))

    return Database(messages,
                    [Node(name=node['name'], comment=None) for node in nodes],
                    [],
                    [],
                    None)
