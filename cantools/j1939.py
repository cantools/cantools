from collections import namedtuple

import bitstruct

from .errors import Error

FrameId = namedtuple('FrameId',
                     [
                         'priority',
                         'reserved',
                         'data_page',
                         'pdu_format',
                         'pdu_specific',
                         'source_address',
                     ])


PGN = namedtuple('PGN',
                 [
                     'reserved',
                     'data_page',
                     'pdu_format',
                     'pdu_specific'
                 ])


def is_pdu_format_1(pdu_format):
    return (pdu_format < 240)


def frame_id_pack(priority,
                  reserved,
                  data_page,
                  pdu_format,
                  pdu_specific,
                  source_address):
    """Pack given values as a frame id and return it as an integer.

    """

    try:
        packed = bitstruct.pack('u3u1u1u8u8u8',
                                priority,
                                reserved,
                                data_page,
                                pdu_format,
                                pdu_specific,
                                source_address)
    except bitstruct.Error:
        if priority > 7:
            raise Error(f'Expected priority 0..7, but got {priority}.')
        elif reserved > 1:
            raise Error(f'Expected reserved 0..1, but got {reserved}.')
        elif data_page > 1:
            raise Error(f'Expected data page 0..1, but got {data_page}.')
        elif pdu_format > 255:
            raise Error('Expected PDU format 0..255, but got {}.'.format(
                pdu_format))
        elif pdu_specific > 255:
            raise Error('Expected PDU specific 0..255, but got {}.'.format(
                pdu_specific))
        elif source_address > 255:
            raise Error('Expected source address 0..255, but got {}.'.format(
                source_address))
        else:
            raise Error('Internal error.')

    return bitstruct.unpack('u29', packed)[0]


def frame_id_unpack(frame_id):
    """Unpack given frame id and return a tuple of priority, reserved,
    data page, PDU format, PDU specific and source address.

    """

    try:
        packed = bitstruct.pack('u29', frame_id)
    except bitstruct.Error:
        raise Error(
            'Expected a frame id 0..0x1fffffff, but got {}.'.format(
                hex(frame_id)))

    return FrameId(*bitstruct.unpack('u3u1u1u8u8u8', packed))


def pgn_pack(reserved, data_page, pdu_format, pdu_specific=0):
    """Pack given values as a parameter group number (PGN) and return it
    as an integer.

    """

    if pdu_format < 240 and pdu_specific != 0:
        raise Error(
            'Expected PDU specific 0 when PDU format is 0..239, but got {}.'.format(
                pdu_specific))

    try:
        packed = bitstruct.pack('u1u1u8u8',
                                reserved,
                                data_page,
                                pdu_format,
                                pdu_specific)
    except bitstruct.Error:
        if reserved > 1:
            raise Error(f'Expected reserved 0..1, but got {reserved}.')
        elif data_page > 1:
            raise Error('Expected data page 0..1, but got {}.'.format(
                data_page))
        elif pdu_format > 255:
            raise Error('Expected PDU format 0..255, but got {}.'.format(
                pdu_format))
        elif pdu_specific > 255:
            raise Error('Expected PDU specific 0..255, but got {}.'.format(
                pdu_specific))
        else:
            raise Error('Internal error.')

    return bitstruct.unpack('u18', packed)[0]


def pgn_unpack(pgn):
    """Unpack given parameter group number (PGN) and return a tuple of
    Reserved, Data Page, PDU Format and PDU Specific.

    """

    try:
        packed = bitstruct.pack('u18', pgn)
    except bitstruct.Error:
        raise Error(
            'Expected a parameter group number 0..0x3ffff, but got {}.'.format(
                hex(pgn)))

    return PGN(*bitstruct.unpack('u1u1u8u8', packed))


def pgn_from_frame_id(frame_id):
    """Get the parameter group number (PGN) from given frame id.

    """

    unpacked = frame_id_unpack(frame_id)

    if unpacked.pdu_format < 240:
        pdu_specific = 0
    else:
        pdu_specific = unpacked.pdu_specific

    return pgn_pack(unpacked.reserved,
                    unpacked.data_page,
                    unpacked.pdu_format,
                    pdu_specific)
