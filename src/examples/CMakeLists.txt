add_executable(basic_usage basic_usage.c)
target_include_directories(basic_usage PUBLIC ${lookup_SOURCE_DIR}/include)
target_link_libraries(basic_usage lookup)

add_executable(pushing_after_lookup pushing_after_lookup.c)
target_include_directories(pushing_after_lookup PUBLIC ${lookup_SOURCE_DIR}/include)
target_link_libraries(pushing_after_lookup lookup)

add_executable(custom_deallocator custom_deallocator.c)
target_include_directories(custom_deallocator PUBLIC ${lookup_SOURCE_DIR}/include)
target_link_libraries(custom_deallocator lookup)