MULTI_LINE_FMT = '''
{message}(
{signals}
)\
'''


def _format_signals(message, decoded_signals):
    formatted_signals = []

    for signal in message.signals:
        try:
            value = decoded_signals[signal.name]
        except KeyError:
            continue

        if isinstance(value, str):
            value = "'{}'".format(value)

        signal_name = signal.name

        formatted_signals.append(
            '{}: {}{}'.format(signal_name,
                               value,
                              ''
                              if signal.unit is None
                              else ' ' + signal.unit))

    return formatted_signals


def _format_message_single_line(message, formatted_signals):
    return ' {}({})'.format(message.name,
                            ', '.join(formatted_signals))


def _format_message_multi_line(message, formatted_signals):
    indented_signals = [
        '    ' + formatted_signal
        for formatted_signal in formatted_signals
    ]

    return MULTI_LINE_FMT.format(message=message.name,
                                 signals=',\n'.join(indented_signals))


def format_message_by_frame_id(dbase,
                               frame_id,
                               data,
                               decode_choices,
                               single_line):
    try:
        message = dbase.get_message_by_frame_id(frame_id)
    except KeyError:
        return ' Unknown frame id {0} (0x{0:x})'.format(frame_id)

    return format_message(message, data, decode_choices, single_line)


def format_message(message, data, decode_choices, single_line):
    try:
        decoded_signals = message.decode(data, decode_choices)
    except Exception as e:
        return ' ' + str(e)

    formatted_signals = _format_signals(message, decoded_signals)

    if single_line:
        return _format_message_single_line(message, formatted_signals)
    else:
        return _format_message_multi_line(message, formatted_signals)

def format_multiplexed_name(message, data, decode_choices):
    decoded_signals = message.decode(data, decode_choices)

    # The idea here is that we rely on the sorted order of the Signals, and
    # then simply go through each possible Multiplexer and build a composite
    # key consisting of the Message name prepended to all the possible MUX
    # Signals (and their values). This composite key is therefore unique for
    # all the different possible enumerations of MUX values, which allows us
    # to display each MUXed Message on its own separate line.
    result = [message.name]

    for signal in message.signals:
        if signal.is_multiplexer:
            if signal.name in decoded_signals:
                result.append(str(decoded_signals[signal.name]))
            elif signal.initial is not None:
                result.append(str(signal.initial))
            else:
                result.append('0')

    return '__'.join(result)
