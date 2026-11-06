# All modules are compiled into one static library. The sub-directories only
# organise the sources: several of them depend on each other (process <->
# bounding, utilities <-> static_data), which one library per directory cannot
# express without relying on the link order.
file(GLOB_RECURSE FLOORPLANNER_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/bounding/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/output_data/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/plugin/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/polygon/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/process/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/static_data/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/*.cpp)

add_library(floorplanner_core STATIC ${FLOORPLANNER_SOURCES})
target_include_directories(floorplanner_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(floorplanner_core PUBLIC glpk::glpk Threads::Threads)

add_executable(floorplanner main.cpp)
target_link_libraries(floorplanner PRIVATE floorplanner_core)
