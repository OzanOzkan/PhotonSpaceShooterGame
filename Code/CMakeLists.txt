cmake_minimum_required (VERSION 3.6.2)
set(CRYENGINE_DIR "F:/CRYENGINE Launcher/Crytek/CRYENGINE_5.4")
set(TOOLS_CMAKE_DIR "${CRYENGINE_DIR}/Tools/CMake")

set(PROJECT_BUILD 1)
set(PROJECT_DIR "F:/CRYENGINE Launcher/_PROJECTS/My Project")

include("${TOOLS_CMAKE_DIR}/CommonOptions.cmake")

add_subdirectory("${CRYENGINE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CRYENGINE")

include("${TOOLS_CMAKE_DIR}/Configure.cmake")
start_sources()

sources_platform(ALL)
add_sources("Code_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Root"
		"GamePlugin.cpp"
		"StdAfx.cpp"
		"GamePlugin.h"
		"StdAfx.h"
)
add_sources("Components_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Components"
		"Components/Bullet.cpp"
		"Components/Player.cpp"
		"Components/Ship.cpp"
		"Components/ShipPropulsion.cpp"
		"Components/Weapon.cpp"
		"Components/Bullet.h"
		"Components/Player.h"
		"Components/Ship.h"
		"Components/ShipPropulsion.h"
		"Components/Weapon.h"
		"Components/TestEntity.cpp"
		"Components/TestEntity.h"
)
add_sources("ComponentsPhoton_uber.cpp"
    PROJECTS Game
    SOURCE_GROUP "Photon"
		"Components/Photon/PhotonComponent.cpp"
		"Components/Photon/PhotonComponent.h"
)

end_sources()

CryEngineModule(Game PCH "StdAfx.cpp" SOLUTION_FOLDER "Project")

target_include_directories(${THIS_PROJECT}
PRIVATE 
    "${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
    "${CRYENGINE_DIR}/Code/CryEngine/CryAction"
	"${CRYENGINE_DIR}/Code/CryEngine/CrySchematyc/Core/Interface"
	"${CRYENGINE_DIR}/Code/CryPlugins/CryDefaultEntities/Module"
)

# Set StartUp project in Visual Studio

add_library(GameLauncher STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameLauncher PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameLauncher "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"F:/CRYENGINE Launcher/_PROJECTS/My Project/Game.cryproject\"")
endif()

add_library(Sandbox STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(Sandbox PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(Sandbox "${CRYENGINE_DIR}/bin/win_x64/Sandbox.exe" "-project \"F:/CRYENGINE Launcher/_PROJECTS/My Project/Game.cryproject\"")
endif()

add_library(GameServer STATIC "${CRYENGINE_DIR}/Code/CryEngine/CryCommon/CryCore/Platform/platform.h")
set_target_properties(GameServer PROPERTIES LINKER_LANGUAGE CXX)
if (WIN32)
    set_visual_studio_debugger_command(GameServer "${CRYENGINE_DIR}/bin/win_x64/Game_Server.exe" "-project \"F:/CRYENGINE Launcher/_PROJECTS/My Project/Game.cryproject\"")
endif()

set_solution_startup_target(GameLauncher)

if (WIN32)
    set_visual_studio_debugger_command( ${THIS_PROJECT} "${CRYENGINE_DIR}/bin/win_x64/GameLauncher.exe" "-project \"F:/CRYENGINE Launcher/_PROJECTS/My Project/Game.cryproject\"" )
endif()

#BEGIN-CUSTOM
# Make any custom changes here, modifications outside of the block will be discarded on regeneration.
set(PHOTON_DIR "${PROJECT_DIR}/solutions/win64/libs/Photon/")

include_directories("${PHOTON_DIR}")
link_directories("${PHOTON_DIR}/Common-cpp/lib/" "${PHOTON_DIR}/Photon-cpp/lib/" "${PHOTON_DIR}/LoadBalancing-cpp/lib/" "${PHOTON_DIR}/Chat-cpp/lib/")
link_libraries("${PHOTON_DIR}/Common-cpp/lib/Common-cpp_debug_windows_md_x64.lib")
link_libraries("${PHOTON_DIR}/Photon-cpp/lib/Photon-cpp_debug_windows_md_x64.lib")
link_libraries("${PHOTON_DIR}/LoadBalancing-cpp/lib/LoadBalancing-cpp_debug_windows_md_x64.lib")
link_libraries("${PHOTON_DIR}/Chat-cpp/lib/Chat-cpp_debug_windows_md_x64.lib")
#END-CUSTOM