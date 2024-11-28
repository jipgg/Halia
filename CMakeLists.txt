cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(runtimes_dir ${CMAKE_SOURCE_DIR}/runtimes)
set(resource_dir ${CMAKE_SOURCE_DIR}/resources)
set(luau_library_dir ${resource_dir}/luau_library)
set(resource_output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
set(core_dir ${CMAKE_SOURCE_DIR}/core)
set(common_dir ${CMAKE_SOURCE_DIR}/common)
project(Halia LANGUAGES CXX C)

find_package(blaze REQUIRED)
find_package(Boost REQUIRED COMPONENTS process)
add_subdirectory(luau)
#common
file (GLOB_RECURSE common_files ${common_dir}/*.cpp)
add_library(Halia_common ${common_files})
target_include_directories(Halia_common PRIVATE ${common_dir} ${common_dir}/include/common)
target_include_directories(Halia_common PUBLIC ${common_dir}/include)
target_link_libraries(Halia_common PUBLIC Luau.VM Luau.Compiler Luau.CodeGen Luau.Config blaze::blaze Boost::process)
#core
file (GLOB_RECURSE core_files ${core_dir}/*.cpp)
add_library(Halia_core SHARED ${core_files})
target_include_directories(Halia_core PRIVATE ${core_dir} ${core_dir}/include/Halia)
target_include_directories(Halia_core PUBLIC ${core_dir}/include)
target_link_libraries(Halia_core PUBLIC Halia_common)
#runtime
file(GLOB_RECURSE rt_console_sources ${runtimes_dir}/console/*.cpp)
add_executable(Halia ${rt_console_sources})
target_link_libraries(Halia PUBLIC Halia_core)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${resource_dir}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")
