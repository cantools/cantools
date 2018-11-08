import bitstruct

from .errors import Error


def frame_id_pack(priority, parameter_group_number, source_address):
    """Pack given values as a frame id and return it as an integer.

    """

    try:
        packed = bitstruct.pack('u3u18u8',
                                priority,
                                parameter_group_number,
                                source_address)
    except bitstruct.Error:
        if priority > 7:
            raise Error('Expected priority 0..7, but got {}.'.format(priority))
        elif parameter_group_number > 0x3ffff:
            raise Error(
                'Expected parameter group number 0..0x3ffff, but got {}.'.format(
                    hex(parameter_group_number)))
        elif source_address > 255:
            raise Error('Expected source address 0..255, but got {}.'.format(
                source_address))
        else:
            raise Error('Internal error.')

    return bitstruct.unpack('u29', packed)[0]


def frame_id_unpack(frame_id):
    """Unpack given frame id and return a tuple of Priority, Parameter
    Group Number (PGN) and Source Address.

    """

    try:
        packed = bitstruct.pack('u29', frame_id)
    except bitstruct.Error:
        raise Error(
            'Expected a frame id 0..0x1fffffff, but got {}.'.format(
                hex(frame_id)))

    return bitstruct.unpack('u3u18u8', packed)


def parameter_group_number_pack(reserved,
                                data_page,
                                pdu_format,
                                pdu_specific):
    """Pack given values as a parameter group number (PGN) and return it
    as an integer.

    """

    try:
        packed = bitstruct.pack('u1u1u8u8',
                                reserved,
                                data_page,
                                pdu_format,
                                pdu_specific)
    except bitstruct.Error:
        if reserved > 1:
            raise Error('Expected reserved 0..1, but got {}.'.format(reserved))
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


def parameter_group_number_unpack(parameter_group_number):
    """Unpack given parameter group number (PGN) and return a tuple of
    Data Page, PDU Format, PDU Specific and Reserved.

    """

    try:
        packed = bitstruct.pack('u18', parameter_group_number)
    except bitstruct.Error:
        raise Error(
            'Expected a parameter group number 0..0x3ffff, but got {}.'.format(
                hex(parameter_group_number)))

    return bitstruct.unpack('u1u1u8u8', packed)
