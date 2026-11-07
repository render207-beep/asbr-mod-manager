cmake_minimum_required(VERSION 3.13)

# Solo compilamos para Switch
set(PLATFORM_SWITCH ON CACHE BOOL "" FORCE)

set(BOREALIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/borealis)
set(BOREALIS_LIBRARY ${BOREALIS_DIR}/library)

include(${BOREALIS_LIBRARY}/cmake/commonOption.cmake)
include(${BOREALIS_LIBRARY}/cmake/toolchain.cmake)

project(asbr_mod_manager)
set(PROJECT_AUTHOR "asbr")
set(APP_VERSION "0.1.0")
set(PROJECT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(BRLS_RESOURCES_DIR ".")

file(GLOB_RECURSE MAIN_SRC src/*.cpp)
list(APPEND MAIN_SRC ${BOREALIS_LIBRARY}/lib/platforms/switch/switch_wrapper.c)

# Solo la libreria de Borealis (no su demo)
add_subdirectory(${BOREALIS_LIBRARY})

program_target(${PROJECT_NAME} "${MAIN_SRC}")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# Recursos: primero los de Borealis (fuentes, estilos), luego los nuestros encima
file(COPY ${BOREALIS_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources)

add_custom_target(${PROJECT_NAME}.nro
    DEPENDS ${PROJECT_NAME}
    COMMAND ${NX_NACPTOOL_EXE} --create ${PROJECT_NAME} ${PROJECT_AUTHOR} ${APP_VERSION} ${PROJECT_NAME}.nacp
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_RESOURCES} ${CMAKE_BINARY_DIR}/resources
    COMMAND ${NX_ELF2NRO_EXE} ${PROJECT_NAME}.elf ${PROJECT_NAME}.nro
            --nacp=${PROJECT_NAME}.nacp --romfsdir=${CMAKE_BINARY_DIR}/resources
    ALL
)

target_include_directories(${PROJECT_NAME} PRIVATE src ${APP_PLATFORM_INCLUDE})
target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections ${APP_PLATFORM_OPTION})
target_link_libraries(${PROJECT_NAME} PRIVATE borealis ${APP_PLATFORM_LIB})
target_link_options(${PROJECT_NAME} PRIVATE ${APP_PLATFORM_LINK_OPTION})
