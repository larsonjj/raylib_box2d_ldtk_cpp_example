cmake_minimum_required(VERSION 3.25)
project(raylib_box2d_ldtk_cpp_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

##########################################################################################
# Add dependencies with FetchContent
##########################################################################################

function(add_git_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
        GIT_REPOSITORY ${gitURL}
        GIT_TAG        ${gitTag}
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
    )
    FetchContent_MakeAvailable(${libName})
    target_compile_options(${libName} PRIVATE "-w")
endfunction()

# Add Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
add_git_dependency(
    raylib
    https://github.com/raysan5/raylib.git
    4.5.0
)

# Add LDtkLoader
add_git_dependency(
    LDtkLoader
    https://github.com/Madour/LDtkLoader.git
    1.2.2
)

# Add Box2d
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
add_git_dependency(
    box2d
    https://github.com/erincatto/box2d.git
    v2.4.1
)

# Add {fmt} library
add_git_dependency(
    fmt
    https://github.com/fmtlib/fmt.git
    10.0.0
)

##########################################################################################
# Project executable setup
##########################################################################################

# Adding our source files
# Define PROJECT_SOURCES as a list of all source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp")

# Define PROJECT_INCLUDE to be the path to the include directory of the project
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/")

# Declaring our executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE LDtkLoader)
target_link_libraries(${PROJECT_NAME} PRIVATE box2d)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)

##########################################################################################
# Project build settings
##########################################################################################

add_definitions( -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )

# Directory that contain resources (i.e. images, fonts, etc)
set(resource_dir assets)

# Build list of resource files
set(resource_files)
file(GLOB resource ${resource_dir}/*)
list(APPEND resource_files ${resource})

# Setting ASSETS_PATH
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -DDEBUG")

    # Set the asset path macro to the absolute path on the dev machine
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${resource_dir}/")
else()
    # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./${resource_dir}/")
endif()

# Set common compiler flags
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wswitch")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an .html file.
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Os")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sUSE_GLFW=3 -sASSERTIONS=1 -sWASM=1 -Os -Wall -sTOTAL_MEMORY=67108864 -sFORCE_FILESYSTEM=1 --preload-file ${resource_dir} --shell-file ../sources/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="/${resource_dir}/") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# Ensure that hot-reload is enabled for VS
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
endif()

# Copy all of the resource files to the destination
file(COPY ${resource_files} DESTINATION "${resource_dir}/")
