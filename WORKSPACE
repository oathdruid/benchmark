workspace(name = 'oathdruid_benchmark')

load('@bazel_tools//tools/build_defs/repo:http.bzl', 'http_archive')

http_archive(
  name = 'com_baidu_babylon',
  urls = ['https://github.com/baidu/babylon/archive/refs/tags/v1.1.3.tar.gz'],
  strip_prefix = 'babylon-1.1.3',
  sha256 = '626c30f7df34dee8b735d8cabd8779ceb1a9274fd73285ec850f522ad36aa7eb',
)

http_archive(
  name = 'com_google_absl',
  urls = ['https://github.com/abseil/abseil-cpp/archive/refs/tags/20230802.1.tar.gz'],
  strip_prefix = 'abseil-cpp-20230802.1',
  sha256 = '987ce98f02eefbaf930d6e38ab16aa05737234d7afbab2d5c4ea7adbe50c28ed',
)

http_archive(
  name = 'com_google_protobuf',
  urls = ['https://github.com/protocolbuffers/protobuf/archive/refs/tags/v25.3.tar.gz'],
  strip_prefix = 'protobuf-25.3',
  sha256 = 'd19643d265b978383352b3143f04c0641eea75a75235c111cc01a1350173180e',
)
load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

http_archive(
  name = 'com_google_protobuf_patch',
  urls = ['https://github.com/protocolbuffers/protobuf/archive/refs/tags/v25.3.tar.gz'],
  strip_prefix = 'protobuf-25.3',
  sha256 = 'd19643d265b978383352b3143f04c0641eea75a75235c111cc01a1350173180e',
  patches = ['@com_baidu_babylon//:patches/protobuf-25.3.patch'],
  patch_args = ['-p1'],
)

http_archive(
  name = 'com_github_nelhage_rules_boost',
  urls = ['https://github.com/nelhage/rules_boost/archive/4ab574f9a84b42b1809978114a4664184716f4bf.tar.gz'],
  strip_prefix = 'rules_boost-4ab574f9a84b42b1809978114a4664184716f4bf',
  sha256 = '2215e6910eb763a971b1f63f53c45c0f2b7607df38c96287666d94d954da8cdc',
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()
