# Pure C COBS encoder/decoder — no FreeRTOS or hardware dependencies.
# Consumers #include "cobs/cobs.h" — the PUBLIC include root is lib/ (this dir's parent).
add_library(cobs STATIC cobs.c)
target_include_directories(cobs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
