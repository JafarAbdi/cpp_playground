cmake_minimum_required(VERSION 3.5)
project(cpp_playground)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)
find_package(magic_enum REQUIRED)
find_package(range-v3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

# list(APPEND CMAKE_PREFIX_PATH "$ENV{WORKSPACE_DIR}/atum/install") # atum::atum
# find_package(atum REQUIRED)

add_executable(traits_playground src/traits_playground.cpp)
target_link_libraries(traits_playground fmt::fmt pthread)

add_executable(generator src/generator.cpp)
target_link_libraries(generator PRIVATE fmt::fmt)

add_executable(design_patterns src/lambda_playground.cpp)
target_link_libraries(design_patterns PRIVATE fmt::fmt)

add_executable(fixed_string src/fixed_string.cpp)
target_link_libraries(fixed_string PRIVATE fmt::fmt)
find_path(CTRE_INCLUDE_DIRS "ctll.hpp")
target_include_directories(fixed_string PRIVATE ${CTRE_INCLUDE_DIRS})
# target_compile_options(fixed_string PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address)
# target_link_options(fixed_string PRIVATE -g -fno-omit-frame-pointer -fsanitize=undefined,address)

add_executable(serialization_any src/serialization_any.cpp)
target_link_libraries(serialization_any PRIVATE fmt::fmt Boost::boost boost_serialization)

add_executable(serialization_variant src/serialization_variant.cpp)
target_link_libraries(serialization_variant PRIVATE fmt::fmt Boost::boost boost_serialization)

add_executable(overloader src/overloader.cpp)
target_link_libraries(overloader PRIVATE fmt::fmt)

add_executable(playground src/value_iteration.cpp)
target_link_libraries(playground magic_enum::magic_enum fmt::fmt)
