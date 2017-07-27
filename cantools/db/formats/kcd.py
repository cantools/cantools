# Load and dump a CAN database in KCD format.

from xml.etree.ElementTree import fromstring

from ..signal import Signal
from ..message import Message
from ..node import Node
from ..database import Database

from .utils import num


_NAMESPACES = {'ns': 'http://kayak.2codeornot2code.org/1.0'}


def dumps(database):
    """Format database in DBC file format.

    """

    return str(database)


def loads(string):
    """Parse given string.

    """

    root = fromstring(string)

    nodes = [node.attrib for node in root.findall('./ns:Node', _NAMESPACES)]

    messages = []

    for message in root.findall('./ns:Bus/ns:Message', _NAMESPACES):
        # Find all signals in this message.
        signals = []

        for signal in message.findall('ns:Signal', _NAMESPACES):
            # Length.
            length = (int(signal.attrib['length'])
                      if 'length' in signal.attrib
                      else 1)

            # Byte order.
            byte_order = (signal.attrib['endianess']
                          if 'endianess' in signal.attrib
                          else 'big')
            byte_order += '_endian'

            value = signal.find('ns:Value', _NAMESPACES)

            # Minimum.
            try:
                minimum = num(value.attrib['min'])
            except (AttributeError, KeyError):
                minimum = None

            # Maximum.
            try:
                maximum = num(value.attrib['max'])
            except (AttributeError, KeyError):
                maximum = None

            # Slope.
            try:
                slope = num(value.attrib['slope'])
            except (AttributeError, KeyError):
                slope = 1

            # Intercept.
            try:
                intercept = num(value.attrib['intercept'])
            except (AttributeError, KeyError):
                intercept = 0

            # Unit.
            try:
                unit = value.attrib['unit']
            except (AttributeError, KeyError):
                unit = None

            # Notes.
            try:
                notes = signal.find('ns:Notes', _NAMESPACES).text
            except AttributeError:
                notes = None

            signals.append(Signal(name=signal.attrib['name'],
                                  start=int(signal.attrib['offset']),
                                  length=length,
                                  nodes=[],
                                  byte_order=byte_order,
                                  is_signed=False,
                                  scale=slope,
                                  offset=intercept,
                                  minimum=minimum,
                                  maximum=maximum,
                                  unit=unit,
                                  choices=None,
                                  comment=notes,
                                  is_multiplexer=False,
                                  multiplexer_id=None))

        # Frame id.
        frame_id = int(message.attrib['id'], 0)

        # Extended format.
        try:
            is_extended_frame = (message.attrib['format'] == 'extended')
        except KeyError:
            is_extended_frame = False

        # Comment.
        try:
            comment = message.findall('ns:Notes', _NAMESPACES)[0].text
        except:
            comment = None


        messages.append(Message(
            frame_id=frame_id,
            is_extended_frame=is_extended_frame,
            name=message.attrib['name'],
            length=8,
            nodes=[],
            send_type=None,
            cycle_time=None,
            signals=signals,
            comment=comment))

    return Database(messages,
                    [Node(name=node['name'], comment=None) for node in nodes],
                    [],
                    [],
                    None)
