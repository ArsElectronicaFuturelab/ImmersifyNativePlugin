cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

project(ImmersifyNativePlugin CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME ImmersifyUnityPlugin)

################################################################################
# Source groups
################################################################################
set(Source
    "../ImmersifyUnityPlayer/src/ImmersifyUnityPlugin.cpp"
)
source_group("Source" FILES ${Source})

set(Unity__Header
    "../ImmersifyUnityPlayer/src/Unity/IUnityGraphics.h"
    "../ImmersifyUnityPlayer/src/Unity/IUnityGraphicsD3D11.h"
    "../ImmersifyUnityPlayer/src/Unity/IUnityGraphicsD3D12.h"
    "../ImmersifyUnityPlayer/src/Unity/IUnityGraphicsD3D9.h"
    "../ImmersifyUnityPlayer/src/Unity/IUnityInterface.h"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/PlatformBase.h"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI.h"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_OpenGLCore.h"
)
source_group("Unity\\Header" FILES ${Unity__Header})

set(Unity__Source
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_D3D11.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_D3D12.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_D3D9.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_OpenGL2.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_OpenGLCore.cpp"
    "../ImmersifyUnityPlayer/src/Unity/Rendering/RenderAPI_OpenGLCoreES.cpp"
)
source_group("Unity\\Source" FILES ${Unity__Source})

set(ALL_FILES
    ${Source}
    ${Unity__Header}
    ${Unity__Source}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE RenderingPlugin)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}_d"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../src;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/samples/common;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyCore/src/Header;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyUnityPlayer/src"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../src;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/samples/common;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyCore/src/Header;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyUnityPlayer/src"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "GLEW_STATIC;"
        "_WIN32;"
        "_WINDOWS;"
        "_USRDLL;"
        "RENDERINGPLUGIN_EXPORTS;"
        "_MBCS;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_WINSOCK_DEPRECATED_NO_WARNINGS;"
        "WIN32;"
        "__WINDOWS_DS__;"
        "__WINDOWS_ASIO__;"
        "_CONSOLE;"
        "NOMINMAX;"
        "WIN32_LEAN_AND_MEAN;"
        "_WIN32_WINNT=NTDDI_WIN7;"
        "WIN64"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "GLEW_STATIC;"
        "_WIN32;"
        "_WINDOWS;"
        "_USRDLL;"
        "RENDERINGPLUGIN_EXPORTS;"
        "_MBCS;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_WINSOCK_DEPRECATED_NO_WARNINGS;"
        "WIN32;"
        "__WINDOWS_DS__;"
        "__WINDOWS_ASIO__;"
        "_CONSOLE;"
        "NOMINMAX;"
        "WIN32_LEAN_AND_MEAN;"
        "_WIN32_WINNT=NTDDI_WIN7;"
        "WIN64"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Release>:
                /MP;
                /O2;
                /Oi;
                /Gy;
                /Zi
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /openmp;
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Release>:
                /MP;
                /O2;
                /Oi;
                /Gy;
                /Zi
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /openmp;
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /NODEFAULTLIB:libcmt;
            /DEBUG;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /NODEFAULTLIB:libcmt;
            /DEBUG;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> copy /Y "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\bin\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>\\ImmersifyUnityPlugin_d.dll" "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\UnityImmersifyPlayer\\Assets\\Immersify\\Plugins\\x86_64\\ImmersifyUnityPlugin_d.dll"
        COMMAND $<CONFIG:Release> copy /Y "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\bin\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>\\ImmersifyUnityPlugin.dll" "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\UnityImmersifyPlayer\\Assets\\Immersify\\Plugins\\x86_64\\ImmersifyUnityPlugin.dll"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> copy /Y "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\bin\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>\\ImmersifyUnityPlugin_d.dll" "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\UnityImmersifyPlayer\\Assets\\Immersify\\Plugins\\x86_64\\ImmersifyUnityPlugin_d.dll"
        COMMAND $<CONFIG:Release> copy /Y "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\bin\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>\\ImmersifyUnityPlugin.dll" "$<SHELL_PATH:${CMAKE_CURRENT_SOURCE_DIR}/>..\\..\\UnityImmersifyPlayer\\Assets\\Immersify\\Plugins\\x86_64\\ImmersifyUnityPlugin.dll"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    ImmersifyCore
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "opengl32;"
        "glew64sd;"
        "glu32;"
        "spinlib_rms;"
        "ImmersifyCore;"
        "avutil;"
        "avformat;"
        "avcodec"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "opengl32;"
        "glew64sd;"
        "glu32;"
        "spinlib_rms;"
        "ImmersifyCore;"
        "avutil;"
        "avformat;"
        "avcodec"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows_redist;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows_redist/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows_redist;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows_redist/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>"
    )
endif()



























set(PROJECT_NAME ImmersifyCore)

################################################################################
# Source groups
################################################################################
set(Header
    "../ImmersifyCore/src/Header/BaseTextureAccess.h"
    "../ImmersifyCore/src/Header/Decoder.h"
    "../ImmersifyCore/src/Header/DxTextureAccess.h"
    "../ImmersifyCore/src/Header/glext.h"
    "../ImmersifyCore/src/Header/glTextureAccess.h"
    "../ImmersifyCore/src/Header/Sequencer.h"
    "../ImmersifyCore/src/Header/TextureFormats.h"
    "../ImmersifyCore/src/Header/Timer.h"
    "../ImmersifyCore/src/Header/Utils.h"
)
source_group("Header" FILES ${Header})

set(Source
    "../ImmersifyCore/src/Decoder.cpp"
    "../ImmersifyCore/src/DxTextureAccess.cpp"
    "../ImmersifyCore/src/glTextureAccess.cpp"
    "../ImmersifyCore/src/Sequencer.cpp"
    "../ImmersifyCore/src/Timer.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Header}
    ${Source}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE ImmersifyCore)

################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../libs/immcore/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyCore/src/Header;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/common;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glut/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../ImmersifyCore/src/Header;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/common;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glut/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
        )
    endif()
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glut/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/spinsdk/libs/windows/$<CONFIG>;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glew/lib;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../libs/glut/lib"
    )
endif()

