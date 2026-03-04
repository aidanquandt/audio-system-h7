# Generated sources (RPC shims, etc.). Keep hand-written code in src/.
add_library(generated STATIC rpc.c)

target_include_directories(generated
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..   # consumers: #include "generated/rpc.h"
)

target_link_libraries(generated PRIVATE project_includes)
