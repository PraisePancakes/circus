cmake_minimum_required(VERSION 3.14)  # FetchContent is best with 3.14+

set(PROJECT_NAME CIRCUS)

project(${PROJECT_NAME} VERSION 0.0.1 DESCRIPTION "Circus : C++ serialization library" LANGUAGES CXX)

message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
  message("${i}")
endforeach()

# Main project executable
add_executable(${PROJECT_NAME} 
    main.cpp
)

message("INFO :")
message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")
message("  Source: ${PROJECT_SOURCE_DIR}")
message("  Binary: ${PROJECT_BINARY_DIR}")
message("  Version: ${PROJECT_VERSION}")
message("  Version (alt): ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror )
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# ----------------------------
# Add Doctest test executable
# ----------------------------

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG        v2.4.11 # or latest stable tag
)
FetchContent_MakeAvailable(doctest)

add_executable(testing
  test/test_main.cpp
  test/testing_components/test_deserializer.cpp
  test/testing_components/test_lexer.cpp
  test/testing_components/test_utils.cpp
  test/testing_components/test_parser.cpp
)
target_link_libraries(testing PRIVATE doctest)
target_compile_features(testing PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(testing PRIVATE /W4 /WX)
else()
  target_compile_options(testing PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
# Optional: enable CTest
enable_testing()
add_test(NAME test_all COMMAND testing)

add_custom_command(TARGET testing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/example
        $<TARGET_FILE_DIR:testing>/example
)
