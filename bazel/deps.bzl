# pip deps
load("@rules_python//python:pip.bzl", "pip3_import")

def pip_deps(**kwargs):
    pip3_import(
        name = "cantools_py",
        requirements = "@cantools_repo//:requirements.txt",
    )

def cantools_deps(**kwargs):
    pip_deps()
