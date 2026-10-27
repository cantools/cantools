"""Independent output filenames for the C source generator."""

from unittest.mock import patch

import pytest

import cantools


@pytest.mark.parametrize('namespace', [None, 'vehicle_api'])
@pytest.mark.parametrize('stem', ['Vehicle-Log', 'bus.v2'])
@pytest.mark.parametrize('fuzzer', [False, True])
def test_output_name(tmp_path, namespace, stem, fuzzer):
    output = tmp_path / 'nested' / 'output'
    argv = ['cantools', 'generate_c_source', '--file-name', stem,
            '-o', str(output), 'tests/files/dbc/motohawk.dbc']
    if namespace is not None:
        argv.extend(['--database-name', namespace])
    if fuzzer:
        argv.append('--generate-fuzzer')
    with patch('sys.argv', argv):
        cantools._main()

    prefix = namespace or 'motohawk'
    names = [stem + '.c', stem + '.h']
    if fuzzer:
        names.extend([stem + '_fuzzer.c', stem + '_fuzzer.mk'])
    assert sorted(p.name for p in output.iterdir()) == sorted(names)
    header = (output / (stem + '.h')).read_text()
    source = (output / (stem + '.c')).read_text()
    assert '#ifndef ' + prefix.upper() + '_H' in header
    assert prefix + '_example_message_pack(' in header
    assert prefix + '_example_message_pack(' in source
    assert '#include "' + stem + '.h"' in source
    assert not (output / (prefix + '.c')).exists()
    if fuzzer:
        fuzzer_source = (output / (stem + '_fuzzer.c')).read_text()
        makefile = (output / (stem + '_fuzzer.mk')).read_text()
        assert '#include "' + stem + '.h"' in fuzzer_source
        assert prefix + '_example_message_pack(' in fuzzer_source
        assert stem + '.c' in makefile
        assert stem + '_fuzzer.c' in makefile


@pytest.mark.parametrize('stem', ['', '.', '..', '../escape', '/absolute',
                                  r'..\escape', 'two words', 'bad"name',
                                  'bad$name', 'bad\nname', '-option'])
def test_invalid_output_name(tmp_path, capsys, stem):
    output = tmp_path / 'output'
    argv = ['cantools', 'generate_c_source', '--file-name=' + stem,
            '-o', str(output), 'missing-input.dbc']
    with patch('sys.argv', argv), pytest.raises(SystemExit) as error:
        cantools._main()
    assert error.value.code == 2
    assert 'file-name' in capsys.readouterr().err
    assert not output.exists()


def test_output_directory_abbreviation(tmp_path):
    # --output was an accepted abbreviation before the filename option existed.
    argv = ['cantools', 'generate_c_source', '--output', str(tmp_path),
            'tests/files/dbc/motohawk.dbc']
    with patch('sys.argv', argv):
        cantools._main()
    assert sorted(p.name for p in tmp_path.iterdir()) == ['motohawk.c', 'motohawk.h']
