cmake_minimum_required(VERSION 3.15)
project(asbr_mod_manager)
set(CMAKE_CXX_STANDARD 17)

# Borealis como submodulo: git submodule add https://github.com/xfangfang/borealis library/borealis
set(PLATFORM_SWITCH ON CACHE BOOL "" FORCE)
add_subdirectory(library/borealis)

add_executable(${PROJECT_NAME}
    src/main.cpp src/main_activity.cpp src/mod_manager.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE borealis)

# Los XML de la UI deben quedar en romfs
file(COPY resources/ DESTINATION ${CMAKE_BINARY_DIR}/romfs/resources)
file(COPY library/borealis/resources/ DESTINATION ${CMAKE_BINARY_DIR}/romfs/resources/borealis)

# Genera el .nro (macros de devkitPro Switch.cmake)
nx_generate_nacp(${PROJECT_NAME}.nacp NAME "ASBR Mod Manager" AUTHOR "TuNombre" VERSION "0.1.0")
nx_create_nro(${PROJECT_NAME} NACP ${PROJECT_NAME}.nacp ROMFS ${CMAKE_BINARY_DIR}/romfs)
