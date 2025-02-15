target_sources(malloc_kernel
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/mem_malloc.c
)

target_include_directories(malloc_kernel
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(malloc_kernel kernel_include)
