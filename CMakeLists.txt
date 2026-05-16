cmake_minimum_required(VERSION 3.24...3.30)
project(MiniGamesInC)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 6.0)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
  carena
  GIT_REPOSITORY https://github.com/mirsahebali/carena.h.git
  GIT_TAG        origin/main
  GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(carena)

set(PROJECT_GAMES flappy-bird
                  jetpack-joyride
                  river-raid
                  indy-500
                  pacman
                  super-mario-bros
                  worms
                  crash-bandicoot
                  portal
                  doom)

add_subdirectory(src/core)

foreach(GAME ${PROJECT_GAMES})
  add_executable(${GAME})
  add_subdirectory(src/games/${GAME})

  set_target_properties(${GAME} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${GAME})

  set_property(TARGET ${GAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${GAME}>)

  if ("${PLATFORM}" STREQUAL "Web")
      add_custom_command(
          TARGET ${GAME} PRE_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/${GAME}/resources $<TARGET_FILE_DIR:${GAME}>/../resources
      )
      #DEPENDS ${PROJECT_NAME}
  else()
      add_custom_command(
          TARGET ${GAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${GAME}>/resources
      )
      #DEPENDS ${PROJECT_NAME}
  endif()

  target_include_directories(${GAME} PRIVATE ${carena_SOURCE_DIR})

  if(RUN_TESTS)
    target_compile_definitions(${GAME} PRIVATE RUN_TESTS)
  endif()

  #set(raylib_VERBOSE 1)
  target_link_libraries(${GAME} PRIVATE ${PROJECT_NAME}-core)

  # Web Configurations
  if ("${PLATFORM}" STREQUAL "Web")
      # Tell Emscripten to build an example.html file.
      set_target_properties(${GAME} PROPERTIES SUFFIX ".html")
      target_link_options(${GAME} PUBLIC -sUSE_GLFW=3 PUBLIC --preload-file resources)
  endif()

  # Checks if OSX and links appropriate frameworks (Only required on MacOS)
  if (APPLE)
      target_link_libraries(${GAME} "-framework IOKit")
      target_link_libraries(${GAME} "-framework Cocoa")
      target_link_libraries(${GAME} "-framework OpenGL")
  endif()

  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(
      CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} -g -Wextra -Wall  -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer"
    )
    target_link_options(${GAME}
      BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address
    )
  endif()
  
endforeach()
