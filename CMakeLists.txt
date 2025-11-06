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
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# --- minimal: point CMake at your Source/ tree ---
set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/Source")

# If you still list files manually, comment this next line out.
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c" "${SRC_DIR}/*.h"
)

# If you use SRC_FILES, make sure your target uses it:
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link against SDL2 and SDL2_image
# SDL2main is needed on Windows to provide a main() wrapper
# SDL2_image is needed for image loading support

target_link_libraries(SuperShiny
  SDL2::SDL2
  SDL2::SDL2main
  SDL2_image::SDL2_image
  SDL2_ttf::SDL2_ttf
  SDL2_mixer::SDL2_mixer
)

# --- Per-config export directory for the final binary ---
# Put this right after target_link_libraries(SuperShiny ...) and before any POST_BUILD commands.

# Derive a platform tag the way you want it shown on disk
if (WIN32)
  if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(SSGE_PLATFORM "Windows11_x64")
    else()
      set(SSGE_PLATFORM "Windows11_Win32")
    endif()
  elseif (MINGW)
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
      set(SSGE_PLATFORM "WindowsXP_x86")
    else()
      set(SSGE_PLATFORM "Windows_x64")
    endif()
  endif()
else()
  set(SSGE_PLATFORM "${CMAKE_SYSTEM_NAME}")
endif()

# Route target outputs into Export/SuperShiny_<platform>_<config>
foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
  set(outdir "${CMAKE_SOURCE_DIR}/Export/SuperShiny_${SSGE_PLATFORM}_${OUTPUTCONFIG}")
  set_target_properties(SuperShiny PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${outdir}"
    LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${outdir}"
    ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${outdir}"
  )
endforeach()

if (MSVC)
  target_compile_options(SuperShiny PRIVATE
    $<$<CONFIG:Debug>:/Zi>         # full debug info in .obj
  )
  target_link_options(SuperShiny PRIVATE
    $<$<CONFIG:Debug>:/DEBUG:FULL> # generate full .pdb
    $<$<CONFIG:Debug>:/INCREMENTAL> # helpful for JIT debugging
  )
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD 17)
  set_property(TARGET SuperShiny PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

# TODO: Add tests and install targets if needed.

# --- Mirror WithEXE into the target dir (Windows only) ---
#if (WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/WithEXE")
#  add_custom_command(TARGET SuperShiny POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E echo "Mirroring WithEXE -> $<TARGET_FILE_DIR:SuperShiny> ..."
#    COMMAND ${CMAKE_COMMAND} -E env
#      WITHEXE_SRC=${CMAKE_SOURCE_DIR}/WithEXE
#      WITHEXE_DST=$<TARGET_FILE_DIR:SuperShiny>
#      WITHEXE_KEEP=$<TARGET_FILE:SuperShiny>
#      "${CMAKE_SOURCE_DIR}/tools/WithEXE_Mirror.exe"
#  )
#endif()
# --- Copy SDL2 and SDL2_image DLLs after build ---
add_custom_command(TARGET SuperShiny POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying SDL2 runtime DLLs..."
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2::SDL2>"
    "$<TARGET_FILE_DIR:SuperShiny>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2_image::SDL2_image>"
    "$<TARGET_FILE_DIR:SuperShiny>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2_ttf::SDL2_ttf>"
    "$<TARGET_FILE_DIR:SuperShiny>"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2_mixer::SDL2_mixer>"
    "$<TARGET_FILE_DIR:SuperShiny>"
)
