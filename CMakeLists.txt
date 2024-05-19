cmake_minimum_required(VERSION 3.16)
project(phong_shading LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(phong_shading src/main.cpp
        src/vector_utils.cpp
        src/phong.cpp
        src/drawing.cpp
        src/consts.h)
target_link_libraries(phong_shading PRIVATE sfml-graphics)
target_compile_features(phong_shading PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET phong_shading
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:CMakeSFMLProject>
        VERBATIM)
endif()

install(TARGETS phong_shading)
