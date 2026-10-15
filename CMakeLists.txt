cmake_minimum_required(VERSION 3.20)

project(
    TwinArraySort
    VERSION 1.0.0
    DESCRIPTION "Reference C++20 implementation of TwinArray Sort"
    HOMEPAGE_URL "https://doi.org/10.3390/electronics15030609"
    LANGUAGES CXX
)

set(TWINARRAY_IS_TOP_LEVEL OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(TWINARRAY_IS_TOP_LEVEL ON)
endif()

option(
    TWINARRAY_BUILD_TESTS
    "Build the TwinArray Sort test suite"
    ${TWINARRAY_IS_TOP_LEVEL}
)

option(
    TWINARRAY_BUILD_EXAMPLES
    "Build the TwinArray Sort example"
    ${TWINARRAY_IS_TOP_LEVEL}
)

option(
    TWINARRAY_BUILD_BENCHMARKS
    "Build the TwinArray Sort benchmark"
    OFF
)

add_library(twinarray_sort INTERFACE)
add_library(twinarray::twinarray_sort ALIAS twinarray_sort)

target_include_directories(
    twinarray_sort
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(
    twinarray_sort
    INTERFACE
        cxx_std_20
)

function(twinarray_set_strict_warnings target_name)
    if(MSVC)
        target_compile_options(
            ${target_name}
            PRIVATE
                /W4
                /permissive-
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            ${target_name}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wsign-conversion
        )
    endif()
endfunction()

if(TWINARRAY_BUILD_EXAMPLES)
    add_executable(
        twinarray_example
        examples/example.cpp
    )
    target_link_libraries(
        twinarray_example
        PRIVATE
            twinarray::twinarray_sort
    )
    twinarray_set_strict_warnings(twinarray_example)
endif()

if(TWINARRAY_BUILD_TESTS)
    include(CTest)

    add_executable(
        twinarray_tests
        tests/test_twinarray_sort.cpp
    )
    target_link_libraries(
        twinarray_tests
        PRIVATE
            twinarray::twinarray_sort
    )
    twinarray_set_strict_warnings(twinarray_tests)

    add_test(
        NAME twinarray_tests
        COMMAND twinarray_tests
    )
endif()

if(TWINARRAY_BUILD_BENCHMARKS)
    add_executable(
        twinarray_benchmark
        benchmarks/benchmark.cpp
    )
    target_link_libraries(
        twinarray_benchmark
        PRIVATE
            twinarray::twinarray_sort
    )
    twinarray_set_strict_warnings(twinarray_benchmark)
endif()

include(GNUInstallDirs)

install(
    FILES include/twinarray_sort.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
