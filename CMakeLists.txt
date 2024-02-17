cmake_minimum_required(VERSION 3.22.1)
set(PROJECT_NAME cityBuilderGame)

project(${PROJECT_NAME} VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Create DEBUG definition")

    add_compile_definitions(DEBUG)
endif()

include_directories(./include/)

if(WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/MP)
    endif()

    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_LIST_DIR}/lib)

    # install raylib
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(lib/raylib)

    set(RAYLIB_INCLUDE_DIRS lib/raylib/src HINTS ${CMAKE_CURRENT_LIST_DIR})

     set(RAYLIB_LIBRARIES raylib)

    add_subdirectory(lib/pugixml)

    # include entt
    set(ENTT_INCLUDE_DIRS lib/entt/src HINTS ${CMAKE_CURRENT_LIST_DIR})

    # include stb
    include_directories(${CMAKE_CURRENT_LIST_DIR}/lib/stb)

else()
    find_package(pugixml REQUIRED)
    if (PUGIXML_FOUND)
        message("pugixml found")
    endif()

    find_package(EnTT REQUIRED)
    if (ENTT_FOUND)
        message("entt found")
    endif()

    include_directories(/usr/include/stb)

    set(FREETYPE_LIBRARY libfreetype.so)
endif()


file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)

add_executable(cityBuilderGame ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${ENTT_INCLUDE_DIRS}
    ${RAYLIB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${RAYLIB_LIBRARIES}
    pugixml
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(RESOURCE_PATH ${CMAKE_CURRENT_LIST_DIR}/res)
if (WIN32)
    # copy resource directory to build dir
    file(CREATE_LINK ${CMAKE_CURRENT_LIST_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res SYMBOLIC)
endif()
