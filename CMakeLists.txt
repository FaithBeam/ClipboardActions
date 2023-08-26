cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SYSTEM_VERSION 10.0)
set(PROJECT_NAME ClipboardActions)
if(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
        set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()


project(ClipboardActions VERSION 0.1.0)

set(Header_Files
        "src/main.h"
        "src/clipboard_handler.hpp"
        "src/config_functions.hpp"
        "src/framework.h"
        "src/gui_helpers.hpp"
        "src/parsed_application.hpp"
        "src/parsed_apps.hpp"
        "src/registry_functions.hpp"
        "src/resource.h"
        "src/string_helpers.hpp"
        "src/targetver.h"
        "src/toast_handler.hpp")
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
        "src/main.cpp"
        "src/clipboard_handler.cpp"
        "src/config_functions.cpp"
        "src/gui_helpers.cpp"
        "src/parsed_application.cpp"
        "src/parsed_apps.cpp"
        "src/registry_functions.cpp"
        "src/string_helpers.cpp"
        "src/toast_handler.cpp")
source_group("Source Files" FILES ${Source_Files})

set(Misc_Files
        "src/ClipboardActions.rc")
source_group("Misc Files" FILES ${Misc_Files})

set(ALL_FILES
        ${Header_Files}
        ${Source_Files}
        ${Misc_Files})

find_package(nlohmann_json CONFIG REQUIRED)
add_executable(${PROJECT_NAME} WIN32 ${ALL_FILES})
target_compile_definitions(${PROJECT_NAME} PRIVATE 
        "$<$<CONFIG:Debug>:"
                "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
                "NDEBUG"
        ">"
        "WIN32;"
        "_WINDOWS;"
        "UNICODE;"
        "_UNICODE")
target_compile_options(${PROJECT_NAME} PRIVATE 
        "$<$<CONFIG:Release>:"
                "/Ois"
                "/Gy"
        ">"
        "/permissive-;"
        "/sdl;"
        "/W4")
target_link_options(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Release>:"
                "/OPT:REF;"
                "/OPT:ICF"
        ">"
        "/DEBUG;"
        "/SUBSYSTEM:WINDOWS;"
        "/ENTRY:wWinMainCRTStartup")
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE")
target_sources(${PROJECT_NAME} PRIVATE src/ClipboardActions.manifest)