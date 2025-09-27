# CMakeList.txt : CMake project for SuperShiny, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SuperShiny")

find_package(SDL2 REQUIRED)

# Add source to this project's executable.
add_executable (SuperShiny "SuperShiny.cpp" "SuperShiny.h" "Program.cpp" "Program.h" "Engine.cpp" "Engine.h" "WindowManager.cpp" "WindowManager.h" "SceneManager.cpp" "SceneManager.h" "InputManager.cpp" "InputManager.h" "StepContext.cpp" "StepContext.h" "DrawContext.cpp" "DrawContext.h" "Factory.cpp" "Factory.h" "Scene.cpp" "Scene.h" "GameWorld.cpp" "GameWorld.h" "Game.cpp" "Game.h" "EntityManager.cpp" "EntityManager.h" "Entity.cpp" "Entity.h")

target_link_libraries(SuperShiny SDL2::SDL2 SDL2::SDL2main)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD 17)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

# TODO: Add tests and install targets if needed.
