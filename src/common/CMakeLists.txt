add_library(ipc_common STATIC
    ipc.c
)

target_include_directories(ipc_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(ipc_common PRIVATE bsp freertos_iface)
