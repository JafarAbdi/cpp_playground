add_executable(functional1 functional1.cpp)
target_link_libraries(functional1 PRIVATE fmt::fmt range-v3 range-v3-meta range-v3::meta range-v3-concepts)
