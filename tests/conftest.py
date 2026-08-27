import traceback

import pytest

from cantools.database.can.formats import arxml, dbc, kcd, sym
from cantools.database.diagnostics.formats import cdd
from cantools.database.errors import Error

LOADERS = [arxml, dbc, kcd, sym, cdd]


@pytest.fixture(autouse=True)
def loaders_raise_only_cantools_errors(request, monkeypatch):
    """Fail the test if a format loader raises anything but a cantools
    Error (ParseError, or Error for the checks of strict mode) or
    NotImplementedError.

    """

    leaks = set()

    def checked(module):
        load_string = module.load_string

        def wrapper(*args, **kwargs):
            try:
                return load_string(*args, **kwargs)
            except (Error, NotImplementedError):
                raise
            except Exception as e:
                frames = traceback.extract_tb(e.__traceback__)
                origin = ([f for f in frames if 'cantools' in f.filename]
                          or frames)[-1]
                leaks.add(f'{module.__name__}: {type(e).__name__}: {e} '
                          f'(raised at {origin.filename}:{origin.lineno})')
                raise

        return wrapper

    for module in LOADERS:
        monkeypatch.setattr(module, 'load_string', checked(module))

    yield

    if leaks and request.node.get_closest_marker('loader_may_raise') is None:
        pytest.fail('A loader raised something other than a cantools Error:\n'
                    + '\n'.join(sorted(leaks)))
