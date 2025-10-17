cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SuperShiny")

# Find SDL2 and SDL2_image using CMAKE_PREFIX_PATH
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Add source to this project's executable.
add_executable (SuperShiny
  "main.cpp" 
  "Program.cpp" "Program.h"
  "Engine.cpp" "Engine.h"
  "WindowManager.cpp" "WindowManager.h"
  "SceneManager.cpp" "SceneManager.h"
  "InputManager.cpp" "InputManager.h"
  "StepContext.cpp" "StepContext.h"
  "DrawContext.cpp" "DrawContext.h"
   
  "Scene.cpp" "Scene.h"
  "GameWorld.cpp" "GameWorld.h"
  "Game.cpp" "Game.h"
  "EntityManager.cpp" "EntityManager.h"
  "Entity.cpp" "Entity.h"
  "SplashScreen.cpp" "SplashScreen.h"
  "InputPad.cpp" "InputPad.h"
  "InputBinding.h" "InputSet.h"
  "Shiny.cpp" "Shiny.h"
  "Sprite.cpp" "Sprite.h"
  "SdlTexture.h"
  "Level.cpp" "Level.h"
  "Utilities.h" "Utilities.cpp"
  "PassKey.h"
  "Accessor.h" "Accessor.cpp"
 "Orb.cpp" "Orb.h")

# Link against SDL2 and SDL2_image
# SDL2main is needed on Windows to provide a main() wrapper
# SDL2_image is needed for image loading support

target_link_libraries(SuperShiny
  SDL2::SDL2
  SDL2::SDL2main
  SDL2_image::SDL2_image
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD 17)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

# TODO: Add tests and install targets if needed.

# --- Copy SDL2 and SDL2_image DLLs after build ---
add_custom_command(TARGET SuperShiny POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying SDL2 runtime DLLs..."
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2::SDL2>"
    "$<TARGET_FILE_DIR:SuperShiny>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2_image::SDL2_image>"
    "$<TARGET_FILE_DIR:SuperShiny>"
)