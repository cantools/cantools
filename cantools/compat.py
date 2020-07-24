class fopen(object):

    def __init__(self, filename, mode, encoding, newline=None):
        self._filename = filename
        self._mode = mode
        self._encoding = encoding
        self._newline = newline
        self._fp = None

    def __enter__(self):
        self._fp = open(self._filename,
                        self._mode,
                        encoding=self._encoding,
                        newline=self._newline,
                        errors='replace')

        return self._fp

    def __exit__(self, exc_type, exc_value, traceback):
        self._fp.close()
