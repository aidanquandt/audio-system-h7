# Placeholder generated library.
add_library(generated STATIC)

target_include_directories(generated
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(generated PRIVATE project_includes)
