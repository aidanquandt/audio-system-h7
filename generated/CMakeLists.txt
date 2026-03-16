# Placeholder generated library – header-only / interface for now.
add_library(generated INTERFACE)

target_include_directories(generated
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(generated INTERFACE project_includes)
