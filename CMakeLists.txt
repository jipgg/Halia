cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(source_dir ${CMAKE_SOURCE_DIR}/src)
set(include_dir ${CMAKE_SOURCE_DIR}/include)
set(resource_dir ${CMAKE_SOURCE_DIR}/resources)
set(luau_library_dir ${resource_dir}/luau_library)
set(resource_output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
project(LuwawRT LANGUAGES CXX C)

find_package(blaze REQUIRED)
find_package(Boost REQUIRED COMPONENTS process)
add_subdirectory(extern/luau)
file (GLOB_RECURSE core_files
    ${source_dir}/common/*.cpp
    ${source_dir}/types/*.cpp
    ${source_dir}/runtime/*.cpp
    ${source_dir}/library/*.cpp
    ${source_dir}/util/*.cpp
    ${source_dir}/util.cpp
)
add_library(LuwawRT.core SHARED ${core_files})
target_include_directories(LuwawRT.core PUBLIC ${include_dir})
target_link_libraries(LuwawRT.core PUBLIC
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    Luau.Config
    blaze::blaze
    Boost::process
)

file(GLOB_RECURSE source_files ${source_dir}/*.cpp)
add_executable(LuwawRT ${source_files})
target_include_directories(LuwawRT PUBLIC ${include_dir})
target_link_libraries(LuwawRT PRIVATE
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    Luau.Config
)
target_link_libraries(LuwawRT PUBLIC blaze::blaze)
target_link_libraries(LuwawRT PUBLIC Boost::process)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${resource_dir}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")
