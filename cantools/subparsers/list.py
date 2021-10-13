import argparse
import cantools

def _print_message(message):
    print(f'{message.name}:')

    if message.comment:
        for lang in message.comments:
            print(f'  Comment[{lang}]: {message.comments[lang]}')

    if message.bus_name:
        print(f'  Bus: {message.bus_name}')

    print(f'  Frame ID: 0x{message.frame_id:x} ({message.frame_id})')
    print(f'  Size: {message.length} bytes')
    print(f'  Is extended frame: {message.is_extended_frame}')

    if message.cycle_time is not None:
        print(f'  Cycle time: {message.cycle_time} ms')

    if message.signals:
        print(f'  Signals:')

    for signal in message.signals:
        signal_type = 'Integer'
        if signal.is_float:
            signal_type = 'Float'
        elif signal.is_multiplexer:
            signal_type = 'Multiplexer'

        print(f'    {signal.name}:')
        if signal.comments is not None:
            for lang in signal.comments:
                print(f'      Comment[{lang}]: {signal.comments[lang]}')
        print(f'      Type: {signal_type}')
        print(f'      Start bit: {signal.start}')
        print(f'      Length: {signal.length} bits')
        if signal.unit:
            print(f'      Unit: {signal.unit}')
        if signal.initial is not None:
            iv = signal.initial
            if isinstance(iv, int) and not signal.is_signed:
                iv = f'{signal.initial} (0x{signal.initial:x})'
            print(f'      Initial value: {iv}')
        if signal.is_signed is not None:
            print(f'      Is signed: {signal.is_signed}')
        if signal.minimum is not None:
            print(f'      Minimum: {signal.minimum}')
        if signal.maximum is not None:
            print(f'      Maximum: {signal.maximum}')

        has_offset = signal.offset is not None and signal.offset != 0
        has_scale = \
            signal.scale is not None \
            and (signal.scale > 1 + 1e-10 or signal.scale < 1 - 1e-10)
        if has_offset or has_scale:
            offset = signal.offset if signal.offset is not None else 0
            print(f'      Offset: {offset}')

            scale = signal.scale if signal.scale is not None else 1
            print(f'      Scaling factor: {signal.scale}')

        if signal.choices:
            print(f'      Named values:')
            for value, choice in signal.choices.items():
                print(f'        {value}: {choice}')
                if choice.comments is not None:
                    for lang, description in choice.comments.items():
                        print(f'          Comment[{lang}]: {description}')

def _do_list(args):
    input_file_name = args.file[0]
    messages = args.messages
    print_all = args.print_all
    exclude_extended = args.exclude_extended
    exclude_normal = args.exclude_normal

    can_db = cantools.database.load_file(input_file_name)

    if print_all:
        # if no messages have been specified, we print the list of
        # messages in the database
        for message in can_db.messages:
            if message.is_extended_frame and exclude_extended:
                continue
            if not message.is_extended_frame and exclude_normal:
                continue
            messages.append(message.name)    
    
        messages.sort()

    if not messages:
        # if no messages have been specified, we print the list of
        # messages in the database
        message_names = []
        for message in can_db.messages:
            if message.is_extended_frame and exclude_extended:
                continue
            if not message.is_extended_frame and exclude_normal:
                continue

            message_names.append(message.name)    
    
        message_names.sort()
        for message_name in message_names:
            print(message_name)

        return
    else:
        # if a list of messages has been specified, the details of these
        # are printed.
        for message_name in messages:
            try:
                message = can_db.get_message_by_name(message_name)
            except:
                print(f'No message named "{message_name}" has been found in input file.')
                continue

            _print_message(message)



def add_subparser(subparsers):
    list_parser = subparsers.add_parser(
        'list',
        description=('Print the contents of a bus description file in an easy '
                     'to process and humanly readable format. This is similar '
                     'to "dump" with the output being less pretty but more '
                     'complete and much easier to process by shell scripts.'),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    list_parser.add_argument(
        '-n', '--exclude-normal',
        default=False,
        action='store_const',
        const=True,
        required=False,
        help='Do not print non-extended CAN messages.')
    list_parser.add_argument(
        '-x', '--exclude-extended',
        default=False,
        action='store_const',
        const=True,
        required=False,
        help='Do not print extended CAN messages.')
    list_parser.add_argument(
        '-a', '--all',
        default=False,
        action='store_const',
        dest="print_all",
        const=True,
        required=False,
        help='Print detailed infos for all messages found in the input file.')
    list_parser.add_argument('file', metavar='FILE', nargs=1)
    list_parser.add_argument(
        'messages',
        metavar='[MESSAGES]',
        nargs='*',
        help='The names of the messages which shall be inspected')

    list_parser.set_defaults(func=_do_list)
