"""Exercise the optional union layout against the Python wire codec."""

import shutil
import subprocess
from pathlib import Path

import pytest
from freezegun import freeze_time

import cantools
from cantools.database.can import c_source
from cantools.database.can.message import Message
from cantools.database.can.signal import Signal

DATABASE = Path(__file__).parent / 'files' / 'dbc' / 'multiplexer_unions.dbc'

# These are public member paths, not paths obtained from the generator under test.
PATHS = {
    'Basic': {
        'Mode': 'mode', 'Common': 'common',
        'SignedValue': 'mux_union_mode.m0.signed_value',
        'BigEndian': 'mux_union_mode.m0.big_endian',
        'Floating': 'mux_union_mode.m1.floating',
        'Wide': 'mux_union_mode.m2.wide',
    },
    'Nested': {
        'Mode': 'mode', 'Outer': 'mux_union_mode.m0.outer',
        'Inner': 'mux_union_mode.m1.inner',
        'InnerSigned': 'mux_union_mode.m1.mux_union_inner.m0.inner_signed',
        'InnerFloat': 'mux_union_mode.m1.mux_union_inner.m1.inner_float',
    },
    'Independent': {
        'SelectA': 'select_a', 'SelectB': 'select_b', 'Common': 'common',
        'A0': 'mux_union_select_a.m0.a0',
        'A1': 'mux_union_select_a.m1.a1',
        'B0': 'mux_union_select_b.m0.b0',
        'B1': 'mux_union_select_b.m1.b1',
    },
    'Shared': {
        'Mode': 'mode', 'SharedValue': 'shared_value',
        'First': 'mux_union_mode.m1.first',
        'Second': 'mux_union_mode.m2.second',
        'Third': 'mux_union_mode.m3.third',
    },
    'Collision': {
        'Mode': 'mode', 'mux_union_mode': 'mux_union_mode',
        'First': 'mux_union_mode_.m0.first',
        'Second': 'mux_union_mode_.m1.second',
    },
    'SharedNested': {
        'OuterMode': 'outer_mode', 'Inner': 'inner',
        'OuterValue': 'mux_union_outer_mode.m0.outer_value',
        'InnerFirst': 'mux_union_inner.m0.inner_first',
        'InnerSecond': 'mux_union_inner.m1.inner_second',
    },
}

CASES = [
    ('Basic', {'Mode': 0, 'Common': 42, 'SignedValue': -32768, 'BigEndian': 0xabcd}),
    ('Basic', {'Mode': 1, 'Common': 7, 'Floating': -3.25}),
    ('Basic', {'Mode': 2, 'Common': 255, 'Wide': 0xdeadbeef}),
    ('Nested', {'Mode': 0, 'Outer': 0x89abcdef}),
    ('Nested', {'Mode': 1, 'Inner': 0, 'InnerSigned': -2222}),
    ('Nested', {'Mode': 1, 'Inner': 1, 'InnerFloat': 1.5}),
    ('Independent', {'SelectA': 0, 'SelectB': 0, 'Common': 3,
                     'A0': 0x1234, 'B0': 0x89abcdef}),
    ('Independent', {'SelectA': 0, 'SelectB': 1, 'Common': 4,
                     'A0': 65535, 'B1': -3.25}),
    ('Independent', {'SelectA': 1, 'SelectB': 0, 'Common': 5,
                     'A1': -32768, 'B0': 0xffffffff}),
    ('Independent', {'SelectA': 1, 'SelectB': 1, 'Common': 6,
                     'A1': -1234, 'B1': 1.5}),
    ('Shared', {'Mode': 1, 'SharedValue': 0x1234, 'First': 0x56}),
    ('Shared', {'Mode': 2, 'SharedValue': 0xabcd, 'Second': 0xef}),
    ('Shared', {'Mode': 3, 'Third': 0x89abcdef}),
    ('Collision', {'Mode': 0, 'mux_union_mode': 123, 'First': 0x12345678}),
    ('Collision', {'Mode': 1, 'mux_union_mode': 234, 'Second': 0x89abcdef}),
    ('SharedNested', {'OuterMode': 0, 'OuterValue': 0x12345678}),
    ('SharedNested', {'OuterMode': 1, 'Inner': 0, 'InnerFirst': 0x12345678}),
    ('SharedNested', {'OuterMode': 2, 'Inner': 1, 'InnerSecond': 0x89abcdef}),
]


def generate(database, name, **options):
    return c_source.generate(database, name, f'{name}.h', f'{name}.c',
                             f'{name}_fuzzer.c', **options)


def compile_and_run(tmp_path, generated, body, compiler, standard):
    executable = shutil.which(compiler)
    if executable is None:
        pytest.skip(f'{compiler} is required to execute generated code')

    sources = []
    includes = []
    for name, (header, source, _, _) in generated.items():
        (tmp_path / f'{name}.h').write_text(header)
        path = tmp_path / f'{name}.c'
        path.write_text(source)
        sources.append(str(path))
        includes.append(f'#include "{name}.h"')

    runner = tmp_path / 'runner.c'
    runner.write_text('\n'.join(includes) + '''
#include <assert.h>
#include <stdint.h>
#include <string.h>
int main(void)
{
''' + '\n'.join(body) + '\n    return 0;\n}\n')
    output = tmp_path / 'runner'
    subprocess.run([executable, f'-std={standard}', '-Wall', '-Wextra',
                    '-Wpedantic', '-Werror', *sources, str(runner), '-lm',
                    '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True)


def wire_case(database, message_name, values, prefix='unions', receiver=False):
    message = database.get_message_by_name(message_name)
    payload = message.encode(values, scaling=False)
    name = f'{prefix}_{c_source.camel_to_snake_case(message_name)}'
    expected = ', '.join(f'0x{byte:02x}' for byte in payload)
    lines = ['    {', f'        struct {name}_t tx;',
             f'        const uint8_t expected[8] = {{ {expected} }};']
    if not receiver:
        lines += ['        uint8_t actual[8];',
                  '        memset(&tx, 0xa5, sizeof(tx));']
        for signal_name, value in values.items():
            lines.append(f'        tx.{PATHS[message_name][signal_name]} = {value};')
        lines += [f'        assert({name}_pack(actual, &tx, sizeof(actual)) == 8);',
                  '        assert(memcmp(actual, expected, 8) == 0);']

    lines += ['        memset(&tx, 0x5a, sizeof(tx));',
              f'        assert({name}_unpack(&tx, expected, 8) == 0);']
    for signal_name, value in values.items():
        signal = message.get_signal_by_name(signal_name)
        if not receiver or 'Receiver' in signal.receivers:
            lines.append(f'        assert(tx.{PATHS[message_name][signal_name]} == {value});')
    lines.append('    }')
    return lines


@pytest.mark.parametrize(('compiler', 'standard'), [('gcc', 'c99'), ('g++', 'c++11')])
@pytest.mark.parametrize(('bit_fields', 'use_float'), [(False, False), (True, False), (False, True)])
def test_union_wire_compatibility(tmp_path, compiler, standard, bit_fields, use_float):
    database = cantools.database.load_file(DATABASE)
    options = {'bit_fields': bit_fields, 'use_float': use_float}
    generated = {'unions': generate(database, 'unions', use_unions=True, **options),
                 'flat': generate(database, 'flat', **options)}
    body = []
    if not bit_fields:
        body.append('    assert(sizeof(struct unions_basic_t) < sizeof(struct flat_basic_t));')
    for message_name, values in CASES:
        body += wire_case(database, message_name, values)

    # Inactive initial values share storage. They must not overwrite the branch
    # selected by the initialized selector, including a nested selector.
    for message_name, values in [('Basic', {'Mode': 1, 'Common': 42, 'Floating': -3.25}),
                                 ('Nested', {'Mode': 1, 'Inner': 1, 'InnerFloat': 1.5}),
                                 ('SharedNested', {'OuterMode': 0, 'OuterValue': 0x12345678})]:
        message = database.get_message_by_name(message_name)
        expected = ', '.join(str(byte) for byte in message.encode(values, scaling=False))
        name = f'unions_{c_source.camel_to_snake_case(message_name)}'
        body += ['    {', f'        struct {name}_t value;',
                 '        uint8_t actual[8];',
                 f'        const uint8_t expected[8] = {{ {expected} }};',
                 '        memset(&value, 0xa5, sizeof(value));',
                 f'        assert({name}_init(&value) == 0);',
                 f'        assert({name}_pack(actual, &value, 8) == 8);',
                 '        assert(memcmp(actual, expected, 8) == 0);',
                 f'        assert({name}_init(NULL) == -1);', '    }']

    # A hoisted child mux is still inactive when its outer branch is inactive.
    body += ['    {', '        struct unions_shared_nested_t value;',
             '        assert(unions_shared_nested_init(&value) == 0);',
             '        assert(value.inner == 0);',
             '        const uint8_t zero[sizeof(value.mux_union_inner)] = { 0 };',
             '        assert(memcmp(&value.mux_union_inner, zero, sizeof(zero)) == 0);',
             '    }']

    # Preserve existing C behavior for unknown selectors: no branch is accessed.
    body += ['    {', '        struct unions_basic_t value;',
             '        uint8_t payload[8];',
             '        const uint8_t expected[8] = { 15, 42, 0, 0, 0, 0, 0, 0 };',
             '        memset(&value, 0xa5, sizeof(value));',
             '        value.mode = 15;', '        value.common = 42;',
             '        assert(unions_basic_pack(payload, &value, 8) == 8);',
             '        assert(memcmp(payload, expected, 8) == 0);',
             '        assert(unions_basic_unpack(&value, expected, 8) == 0);',
             '        assert(value.mode == 15 && value.common == 42);', '    }']
    compile_and_run(tmp_path, generated, body, compiler, standard)


@pytest.mark.parametrize(('compiler', 'standard'), [('gcc', 'c99'), ('g++', 'c++11')])
def test_union_receiver_node(tmp_path, compiler, standard):
    database = cantools.database.load_file(DATABASE)
    generated = generate(database, 'receiver', use_unions=True, node_name='Receiver')
    assert 'receiver_basic_pack(' not in generated[0]
    assert 'receiver_basic_wide_decode(' not in generated[0]
    body = []
    for message_name, values in CASES:
        body += wire_case(database, message_name, values, prefix='receiver', receiver=True)
    compile_and_run(tmp_path, {'receiver': generated}, body, compiler, standard)


@pytest.mark.parametrize('selector', [0, 15])
def test_union_initial_selector(tmp_path, selector):
    database = cantools.database.load_file(DATABASE)
    message = database.get_message_by_name('Basic')
    message.get_signal_by_name('Mode').raw_initial = selector
    generated = generate(database, 'unions', use_unions=True)
    payload = message.encode({'Mode': 0, 'Common': 42, 'SignedValue': -1234,
                              'BigEndian': 12345}, scaling=False) if selector == 0 else bytes([15, 42, 0, 0, 0, 0, 0, 0])
    expected = ', '.join(str(byte) for byte in payload)
    body = ['    struct unions_basic_t value;', '    uint8_t actual[8];',
            f'    const uint8_t expected[8] = {{ {expected} }};',
            '    assert(unions_basic_init(&value) == 0);',
            '    assert(unions_basic_pack(actual, &value, 8) == 8);',
            '    assert(memcmp(actual, expected, 8) == 0);']
    if selector == 15:
        body += ['    const uint8_t zero[sizeof(value.mux_union_mode)] = { 0 };',
                 '    assert(memcmp(&value.mux_union_mode, zero, sizeof(zero)) == 0);']
    compile_and_run(tmp_path, {'unions': generated}, body, 'gcc', 'c99')


def test_union_zero_initial_nested_members(tmp_path):
    database = cantools.database.load_file(DATABASE)
    message = database.get_message_by_name('Nested')
    for signal in message.signals:
        signal.raw_initial = None
    message.get_signal_by_name('Mode').raw_initial = 1
    generated = generate(database, 'unions', use_unions=True)
    init_body = generated[1].split('int unions_nested_init(', 1)[1].split('\n}', 1)[0]
    # These writes start the C++ union-member lifetimes. A byte-wise zeroing
    # operation alone can pass runtime tests without doing so.
    assert 'msg_p->mux_union_mode.m1.inner = 0;' in init_body
    assert 'msg_p->mux_union_mode.m1.mux_union_inner.m0.inner_signed = 0;' in init_body
    body = ['    struct unions_nested_t value;', '    uint8_t actual[8];',
            '    const uint8_t expected[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };',
            '    assert(unions_nested_init(&value) == 0);',
            '    assert(unions_nested_pack(actual, &value, 8) == 8);',
            '    assert(memcmp(actual, expected, 8) == 0);']
    compile_and_run(tmp_path, {'unions': generated}, body, 'g++', 'c++11')


@pytest.mark.parametrize(('compiler', 'standard'), [('gcc', 'c99'), ('g++', 'c++11')])
def test_union_many_branches(tmp_path, compiler, standard):
    signals = [Signal('Mode', 0, 8, is_multiplexer=True)]
    signals += [Signal(f'Value{index}', 8, 32, multiplexer_signal='Mode',
                       multiplexer_ids=[index]) for index in range(40)]
    message = Message(100, 'Many', 8, signals)
    database = cantools.database.Database(messages=[message])
    generated = {'unions': generate(database, 'unions', use_unions=True),
                 'flat': generate(database, 'flat')}
    body = ['    assert(sizeof(struct unions_many_t) < sizeof(struct flat_many_t));']
    for index in range(40):
        value = 0xdead0000 + index
        payload = message.encode({'Mode': index, f'Value{index}': value}, scaling=False)
        expected = ', '.join(str(byte) for byte in payload)
        path = f'mux_union_mode.m{index}.value{index}'
        body += ['    {', '        struct unions_many_t tx;',
                 '        struct unions_many_t rx;', '        uint8_t actual[8];',
                 f'        const uint8_t expected[8] = {{ {expected} }};',
                 '        assert(unions_many_init(&tx) == 0);',
                 f'        tx.mode = {index};', f'        tx.{path} = {value};',
                 '        assert(unions_many_pack(actual, &tx, 8) == 8);',
                 '        assert(memcmp(actual, expected, 8) == 0);',
                 '        assert(unions_many_unpack(&rx, expected, 8) == 0);',
                 f'        assert(rx.mode == {index} && rx.{path} == {value});', '    }']
    compile_and_run(tmp_path, generated, body, compiler, standard)


@freeze_time('2026-10-23')
def test_use_unions_is_opt_in():
    database = cantools.database.load_file(DATABASE)
    assert generate(database, 'test') == generate(database, 'test', use_unions=False)


@freeze_time('2026-10-23')
def test_use_unions_command_line(tmp_path, monkeypatch):
    monkeypatch.setattr('sys.argv', ['cantools', 'generate_c_source', '--use-unions',
                                    '--database-name', 'cli', '-o', str(tmp_path),
                                    str(DATABASE)])
    cantools._main()
    database = cantools.database.load_file(DATABASE)
    header, source, _, _ = generate(database, 'cli', use_unions=True)
    assert (tmp_path / 'cli.h').read_text() == header
    assert (tmp_path / 'cli.c').read_text() == source
