cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp src/resource.rc)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(main PROPERTIES WIN32_EXECUTABLE YES)
endif()

# Define the output dir for assets
set(ASSET_SRC_DIR ${CMAKE_SOURCE_DIR}/src/sounds)
set(ASSET_DST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sounds)

# Copy sounds directory at build time
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ASSET_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SRC_DIR} ${ASSET_DST_DIR}
    COMMENT "Copying sound assets to output directory..."
)

# Set paths
set(FONT_SRC_DIR ${CMAKE_SOURCE_DIR}/src/fonts)
set(FONT_DST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fonts)

# Copy fonts after build
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FONT_DST_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${FONT_SRC_DIR} ${FONT_DST_DIR}
    COMMENT "Copying font assets to output directory..."
)

target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio SFML::Network SFML::System SFML::Window)