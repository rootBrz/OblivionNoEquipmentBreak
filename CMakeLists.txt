cmake_minimum_required(VERSION 3.10)
project(OblivionEquipmentNoBreak C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# DLL Details
SET(CMAKE_RC_COMPILER "windres")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.res)
add_custom_command(OUTPUT ${RES_FILE}
    COMMAND ${CMAKE_RC_COMPILER} -O coff -i ${RC_FILE} -o ${RES_FILE}
    MAIN_DEPENDENCY ${RC_FILE}
)
set(VERSION_RESOURCE ${RES_FILE})

file(GLOB SOURCE_FILES *.cpp *.h)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
 
# DLL Details
set_source_files_properties(${VERSION_RESOURCE} PROPERTIES EXTERNAL_OBJECT TRUE)
target_sources(${PROJECT_NAME} PRIVATE ${VERSION_RESOURCE})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -O3 -flto -ffunction-sections -fdata-sections)
target_link_options(${PROJECT_NAME} PRIVATE -static -Wl,--gc-sections)

set(OBR_PATH $ENV{OBR_PATH})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        "${OBR_PATH}/OblivionEquipmentNoBreak.asi"
    COMMENT "Copying ASI to Oblivion directory"
)