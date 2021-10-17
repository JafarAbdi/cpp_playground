cmake_minimum_required(VERSION 3.5)
project(cpp_playground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
  -Wall
  -Wextra
  -Wwrite-strings
  -Wunreachable-code
  -Wpointer-arith
  -Wredundant-decls
  -Wcast-qual)

find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)
find_package(range-v3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

# list(APPEND CMAKE_PREFIX_PATH "$ENV{WORKSPACE_DIR}/atum/install") # atum::atum
# find_package(atum REQUIRED)

add_library(fixed-string INTERFACE)
target_include_directories(fixed-string INTERFACE include/)

add_executable(traits_playground src/traits_playground.cpp)
target_link_libraries(traits_playground fmt::fmt pthread)

add_executable(generator src/generator.cpp)
target_link_libraries(generator PRIVATE fmt::fmt)

add_executable(lambda_playground src/lambda_playground.cpp)
target_link_libraries(lambda_playground PRIVATE fmt::fmt)

add_executable(fixed_string src/testing_fixed_string.cpp)
target_link_libraries(fixed_string PRIVATE fmt::fmt fixed-string)
find_path(CTRE_INCLUDE_DIRS "ctll.hpp")
target_include_directories(fixed_string PRIVATE ${CTRE_INCLUDE_DIRS})
# target_compile_options(fixed_string PRIVATE -g -fno-omit-frame-pointer
# -fsanitize=undefined,address) target_link_options(fixed_string PRIVATE -g
# -fno-omit-frame-pointer -fsanitize=undefined,address)

add_executable(serialization_any src/serialization_any.cpp)
target_link_libraries(serialization_any PRIVATE fmt::fmt Boost::boost
                                                boost_serialization)

add_executable(serialization_variant src/serialization_variant.cpp)
target_link_libraries(serialization_variant PRIVATE fmt::fmt Boost::boost
                                                    boost_serialization)

add_executable(overloader src/overloader.cpp)
target_link_libraries(overloader PRIVATE fmt::fmt)

add_executable(playground src/value_iteration.cpp)
target_link_libraries(playground magic_enum::magic_enum fmt::fmt)

add_executable(concept_t src/concept_t.cpp)
target_link_libraries(concept_t PRIVATE fmt::fmt)
# target_compile_options(concept_t PRIVATE -Xclang -ast-print -fsyntax-only)

add_executable(range_playground src/range_playground.cpp)
target_link_libraries(range_playground PRIVATE fmt::fmt range-v3 range-v3-meta
                                               range-v3::meta range-v3-concepts)
# target_compile_options(range_playground PRIVATE -E)

add_executable(temp_playground src/temp_playground.cpp)
target_link_libraries(temp_playground PRIVATE fmt::fmt range-v3 range-v3-meta
                                              range-v3::meta range-v3-concepts)
target_compile_options(temp_playground PRIVATE -g)

add_executable(namedtuple src/namedtuple.cpp)
target_link_libraries(namedtuple PRIVATE fmt::fmt range-v3 range-v3-meta
                                         range-v3::meta range-v3-concepts)

add_executable(testing_namedtuple src/testing_namedtuple.cpp)
target_link_libraries(
  testing_namedtuple PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta
                             range-v3-concepts fixed-string)

add_executable(static_map src/static_map.cpp)
target_link_libraries(
  static_map PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta
                     range-v3-concepts fixed-string)
#target_compile_options(static_map PRIVATE -Xclang -ast-print -fsyntax-only)

add_subdirectory(src/design_patterns)
