# Header-only library for project-wide includes.
# Any target that needs headers from include/ links against this instead of
# adding the path string manually. Adding a new header here automatically
# makes it visible to all consumers.
add_library(project_includes INTERFACE)
target_include_directories(project_includes INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
