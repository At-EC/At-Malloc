add_library(kernel_include INTERFACE)

include_directories(${KERNEL_PATH}/include)

target_sources(kernel_include
	PUBLIC
	${KERNEL_PATH}/include/mem_malloc.h
)
