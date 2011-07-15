PROJECT(Cool-Context-Creator)

#------- Configuration ------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Config/CMake ${CMAKE_MODULE_PATH})

#------- Specials Uses ------------
SET ( DEBUG_TRACE OFF CACHE BOOL
        "If C3 will create some trace."
)


# -- Dependancies
FIND_PACKAGE(OpenGL REQUIRED)

# * -- To install project ---
SET(PROJECT_BUILD_PATH ${CMAKE_SOURCE_DIR}/Build)

ADD_SUBDIRECTORY(Source)
ADD_SUBDIRECTORY(Samples)
