load("@cantools_py//:requirements.bzl", cantools_pip_install = "pip_install")

def cantools_pips(**kwargs):
    cantools_pip_install()
