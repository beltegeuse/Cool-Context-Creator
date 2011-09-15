project(C3)

#------- Configuration ------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Config/CMake ${CMAKE_MODULE_PATH})

#------- Cache Variables ------------
SET ( DEBUG_TRACE OFF CACHE BOOL "If C3 will create some trace.")
SET( BUILD_SAMPLES_OTHER OFF CACHE BOOL "If you want build samples not related to C3.")
SET( BUILD_SAMPLES_C3 ON CACHE BOOL "If you want build samples related to C3.")


# -- Dependancies
FIND_PACKAGE(OpenGL)
IF(OPENGL_FOUND)
  MESSAGE(STATUS "Find OpenGL")
ELSE(OPENGL_FOUND)
  MESSAGE(FATAL_ERROR "You must install OpenGL")
ENDIF(OPENGL_FOUND)

IF(MSVC)
	ADD_DEFINITIONS("-DMSVC")
ENDIF()

# Parse other files
ADD_SUBDIRECTORY(Source)
ADD_SUBDIRECTORY(Samples)
