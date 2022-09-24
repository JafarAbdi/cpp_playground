add_executable(abstract_factory abstract_factory.cpp)
target_link_libraries(abstract_factory PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta range-v3-concepts)

add_executable(factory_method factory_method.cpp)
target_link_libraries(factory_method PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta range-v3-concepts)

add_executable(external_poly external_poly.cpp)
target_link_libraries(external_poly PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta range-v3-concepts)
