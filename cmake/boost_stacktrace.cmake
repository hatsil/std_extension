include(FetchContent)

FetchContent_Declare(
    boost_config
    GIT_REPOSITORY https://github.com/boostorg/config.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    boost_stacktrace
    GIT_REPOSITORY https://github.com/boostorg/stacktrace.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    container_hash
    GIT_REPOSITORY https://github.com/boostorg/container_hash.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    core
    GIT_REPOSITORY https://github.com/boostorg/core.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    describe
    GIT_REPOSITORY https://github.com/boostorg/describe.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    assert
    GIT_REPOSITORY https://github.com/boostorg/assert.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    mp11
    GIT_REPOSITORY https://github.com/boostorg/mp11.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    predef
    GIT_REPOSITORY https://github.com/boostorg/predef.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
  static_assert
  GIT_REPOSITORY https://github.com/boostorg/static_assert.git
  GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    winapi
    GIT_REPOSITORY https://github.com/boostorg/winapi.git
    GIT_TAG boost-1.86.0
)

FetchContent_Declare(
    throw_exception
    GIT_REPOSITORY https://github.com/boostorg/throw_exception.git
    GIT_TAG boost-1.86.0
)

FetchContent_MakeAvailable(boost_config throw_exception static_assert winapi predef mp11 assert core describe container_hash boost_stacktrace)