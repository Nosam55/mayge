cmake_minimum_required(VERSION 3.10)

project(mayge VERSION 0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define the parts of the library (part == part.{cpp,hpp})
set(MAYGE_PARTS common image audio gui window actor app)

foreach(part IN ITEMS ${MAYGE_PARTS})
    list(APPEND MAYGE_SRC_CPP "${part}.cpp")
    list(APPEND MAYGE_SRC_HPP "${part}.hpp")
endforeach()

# Add header-only files
list(APPEND MAYGE_SRC_HPP "sdl_exception.hpp")

# Transform the header builtfiles list into a LF-seperated list of #include directives
foreach(hdr IN ITEMS ${MAYGE_SRC_HPP})
    set(MAYGE_HEADER_CONTENT "${MAYGE_HEADER_CONTENT}\n#include \"mayge.d/${hdr}\"")
endforeach()

# Add line for generated file
set(MAYGE_HEADER_CONTENT "${MAYGE_HEADER_CONTENT}\n#include \"mayge.d/mayge_config.h\"")

option(MAYGE_VENDORED "Use vendored libraries" OFF)
option(BUILD_SHARED_LIBS "Build dynamic libs (.dll/.so) vs static libs (.lib/.a)" ON)

if(MAYGE_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/sdl_image EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/sdl_ttf EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

    # 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

    find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image)

    find_package(SDL2_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL2_ttf)

    if(!BUILD_SHARED_LIBS)
        find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2-static)
        find_package(SDL2_image REQUIRED CONFIG REQUIRED COMPONENTS SDL2_image-static)
        find_package(SDL2_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL2_ttf-static)
    endif()
endif()

add_library(mayge ${MAYGE_SRC_CPP})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(mayge PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
if(BUILD_SHARED_LIBS)
    target_link_libraries(mayge PRIVATE SDL2::SDL2)
    target_link_libraries(mayge PRIVATE SDL2_image::SDL2_image)
    target_link_libraries(mayge PRIVATE SDL2_ttf::SDL2_ttf)
else()
    target_link_libraries(mayge PRIVATE SDL2::SDL2-static)
    target_link_libraries(mayge PRIVATE SDL2_image::SDL2_image-static)
    target_link_libraries(mayge PRIVATE SDL2_ttf::SDL2_ttf-static)
endif()


# Define generated header files
configure_file(mayge_config.in mayge_config.h)
configure_file(mayge.in mayge)
target_include_directories(mayge PUBLIC "${PROJECT_BINARY_DIR}")

set_target_properties(mayge PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})

install(TARGETS mayge DESTINATION lib)
install(FILES ${MAYGE_SRC_HPP} "${PROJECT_BINARY_DIR}/mayge_config.h" DESTINATION include/mayge.d)
install(FILES "${PROJECT_BINARY_DIR}/mayge" DESTINATION include)
