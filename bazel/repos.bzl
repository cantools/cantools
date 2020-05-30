load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_repos(**kwargs):
    http_archive(
        name = "rules_python",
        urls = ["https://github.com/bazelbuild/rules_python/archive/a0fbf98d4e3a232144df4d0d80b577c7a693b570.zip"],
        strip_prefix = "rules_python-a0fbf98d4e3a232144df4d0d80b577c7a693b570",
        sha256 = "98c9b903f6e8fe20b7e56d19c4822c8c49a11b475bd4ec0ca6a564e8bc5d5fa2",
    )

def googletest_repos(**kwargs):
    http_archive(
        name = "com_google_googletest",
        urls = [
            "https://github.com/google/googletest/archive/e9d5f427b56ae62a18efafa8ec631ad177a5a83c.zip",
        ],
        strip_prefix = "googletest-e9d5f427b56ae62a18efafa8ec631ad177a5a83c",
        sha256 = "4ab3932e8621c6bf7b4cd2bf503e4fb1d35300d2464d1d7832a38b4b7b23505a",
    )

def abseil_repos(**kwargs):
    http_archive(
        name = "com_google_absl",
        urls = [
            "https://github.com/abseil/abseil-cpp/archive/20200225.2.tar.gz",
        ],
        strip_prefix = "abseil-cpp-20200225.2",
        sha256 = "f41868f7a938605c92936230081175d1eae87f6ea2c248f41077c8f88316f111",
    )

def cantools_repos(**kwargs):
    bazel_repos()
    googletest_repos()
    abseil_repos()
