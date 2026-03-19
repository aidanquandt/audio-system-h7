# Nanopb runtime (from submodule)
add_library(nanopb STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/nanopb/pb_encode.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/nanopb/pb_decode.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/nanopb/pb_common.c
)
target_include_directories(nanopb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/nanopb)

# Pre-generated proto (from `make codegen`)
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/messaging_protocol/messaging_protocol.pb.c)
  message(FATAL_ERROR "Generated proto files not found. Run: make codegen")
endif()

add_library(messaging_protocol_proto STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/messaging_protocol/messaging_protocol.pb.c
)
target_include_directories(messaging_protocol_proto PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/messaging_protocol
)
target_link_libraries(messaging_protocol_proto PUBLIC nanopb)

# Facade for consumers
add_library(generated INTERFACE)
target_include_directories(generated INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_SOURCE_DIR}/messaging_protocol
)
target_link_libraries(generated INTERFACE project_includes messaging_protocol_proto)
