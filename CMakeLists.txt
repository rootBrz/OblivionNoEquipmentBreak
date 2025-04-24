cmake_minimum_required(VERSION 3.10)
project(OblivionEquipmentNoBreak C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_FILES *.cpp *.h)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -O3 -flto -ffunction-sections -fdata-sections)
target_link_options(${PROJECT_NAME} PRIVATE -Wl,--gc-sections)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
    -static
    dinput8
)