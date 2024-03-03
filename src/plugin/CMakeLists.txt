file(GLOB SRC_FILE *.cpp *.c)
file(GLOB HEADER_FILE *.hpp *.h)

set(SRC_FILE ${SRC_FILE})
set(HEADER_FILE ${HEADER_FILE})

find_package(Threads)

add_library(plugin ${SRC_FILE} ${HEADER_FILE})

target_include_directories(plugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

target_link_libraries(plugin Threads::Threads)

target_compile_definitions(plugin INTERFACE)
