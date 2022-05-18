# Utilities for dealing with AUTOSAR secure on-board communication.
# (SecOC, i.e., verification of the authenticity of the sender of
# messages.)

from cantools.database.can.message import Message
from cantools.errors import Error

from typing import Union, List, Optional, Callable

import bitstruct

class SecOCError(Error):
    """Exception that is raised if something SecOC related goes wrong.

    """
    pass

def apply_authenticator(raw_payload: Union[bytes, bytearray],
                        dbmsg: Message,
                        authenticator_fn: Callable[[Message,
                                                    bytearray,
                                                    int],
                                                   bytearray],
                        freshness_value: int) \
                        -> bytearray:
    """Given a byte-like object that contains the encoded signals to be
    send, compute the full message which ought to be send.

    This is basically the concatination of the raw payload, the
    truncated freshness value and the truncated authenticator for the
    message.
    """

    if dbmsg.autosar is None or dbmsg.autosar.secoc is None:
        raise SecOCError(f'Message "{dbmsg.name}" is not secured')

    secoc_props = dbmsg.autosar.secoc
    result = bytearray(raw_payload)

    payload_len = secoc_props.payload_length

    # get the last N bits of the freshness value. we assume that the
    # full freshness value is at most 64 bits.
    n_fresh = secoc_props.freshness_bit_length
    n_fresh_tx = secoc_props.freshness_tx_bit_length
    mask = 0xffffffffffffffff >> (64 - n_fresh_tx)
    freshness_header_value = freshness_value&mask

    # compute the authentificator value
    auth_data = bitstruct.pack(f'u16' # data ID
                               f'r{payload_len*8}' # payload to be secured
                               f'u{n_fresh}', # freshness value
                               secoc_props.data_id,
                               raw_payload[:payload_len],
                               freshness_value)

    # compute authenticator and pack it into the result array
    auth_value = authenticator_fn(dbmsg, auth_data, freshness_value)

    bitstruct.pack_into(f'u{n_fresh_tx}r{secoc_props.auth_tx_bit_length}',
                        result,
                        payload_len*8,
                        freshness_header_value,
                        auth_value)

    return result

def verify_authenticator(raw_payload: Union[bytes, bytearray],
                         dbmsg: Message,
                         authenticator_fn: Callable[[Message,
                                                     bytearray,
                                                     int],
                                                    bytearray],
                         freshness_value: int) \
                    -> bool:
    """Verify that a message that is secured via SecOC is valid."""

    tmp_payload = apply_authenticator(raw_payload,
                                      dbmsg,
                                      authenticator_fn,
                                      freshness_value)

    return raw_payload == tmp_payload
