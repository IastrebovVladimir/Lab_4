cmake_minimum_required(VERSION 3.22)
project(Lab_4)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_4
        menu/main_menu_lazy.cpp
        Sequence/types.cpp
)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(LazyTests
        tests/testMain.cpp
        tests/testLazy.cpp
        tests/testGenerator.cpp
        tests/testStreams.cpp
        tests/testMachines.cpp
        tests/testTransfinite.cpp
)

target_link_libraries(LazyTests
        PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LazyTests)
