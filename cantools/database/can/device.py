import can
from cantools import database
import time

class Device(can.Listener):
    """
    A device ref to a can bus, which provide interface to get signals's value easily.
    """
    def __init__(self, data_base, bus_type, channel, bit_rate, encoding="cp1252", frame_id_mask=None, strict = 0):
        self._dbase = database.load_file(data_base,
                                         encoding=encoding,
                                         frame_id_mask=frame_id_mask,
                                         strict=strict)
        self._filter = ''
        self._compiled_filter = None
        self._formatted_messages = {}
        self._message_by_name = {}
        self._message_by_id = {}


        self._bus = self.create_bus(bus_type, channel, bit_rate)
        self._notifier = can.Notifier(self._bus, [self])

    def create_bus(self, bus_type, channel, bit_rate):
        kwargs = {}

        if bit_rate is not None:
            kwargs['bitrate'] = int(bit_rate)

        try:
            return can.Bus(bustype=bus_type,
                           channel=channel,
                           **kwargs)
        except:
            raise Exception(
                "Failed to create CAN bus with bustype='{}' and "
                "channel='{}'.".format(bus_type,
                                       channel))

    def on_message_received(self, msg):
        message = self._dbase.decode_message(msg.arbitration_id, msg.data)
        if message:
            msg_ = self._dbase.get_message_by_frame_id(msg.arbitration_id)
            self._message_by_name[msg_.name] = message
            self._message_by_id[msg.arbitration_id] = message

    def get_signal_by_message(self, message_name, signal_name):
        """

        :param message_name: message name in the database
        :param signal_name:  signal name in the database
        :return: the last value on the can bus for the specific signal,
                 if message_name, signal_name not exsit in database or the
                 message not been received yet, will return None.
        """
        try:
            msg = self._message_by_name[message_name]
            value = msg[signal_name]
        except KeyError:
            return None

        return value


