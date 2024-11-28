cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(source_dir ${CMAKE_SOURCE_DIR}/src)
set(include_dir ${CMAKE_SOURCE_DIR}/include)
set(resource_dir ${CMAKE_SOURCE_DIR}/resources)
set(luau_library_dir ${resource_dir}/luau_library)
set(resource_output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
project(LuwawRT LANGUAGES CXX C)

find_package(blaze REQUIRED)
add_subdirectory(extern/luau)

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
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${resource_dir}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")
