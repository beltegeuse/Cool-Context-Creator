
#
# Compilation Flags
#
IF(CMAKE_BUILD_TYPE MATCHES debug)
	ADD_DEFINITIONS("-Wall -W -D_DEBUG -pg") #Pour afficher les warning lors de la compilation
    #SET(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} " -D_DEBUG -ansi -Wall -Wextra -Wfatal-errors -pedantic -Wmain -Wswitch-default -Wunreachable-code -Winit-self -Wshadow -Wwrite-strings -Wuninitialized") #Pour afficher les warning lors de la compilation
ENDIF(CMAKE_BUILD_TYPE MATCHES debug)

#
# Multi-platform sources
#
SET(LIB_PCM_SRCS
	Exceptions.cpp
	Window.cpp
	WindowImpl.cpp
)

#
# Specific platform sources
#
IF(WIN32)
SET(LIB_PCM_SRCS
	${LIB_PCM_SRCS}
	Win32/WindowImplWin32.cpp
)
ELSE()
SET(LIB_PCM_SRCS
	${LIB_PCM_SRCS}
	Linux/WindowImplLinux.cpp
)
ENDIF()

#
# Lib configuration
#
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_LIBRARY(PCM SHARED ${LIB_PCM_SRCS})

target_link_libraries(
  PCM
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
)

install(TARGETS PCM 
  RUNTIME DESTINATION ${PROJECT_BUILD_PATH}
  LIBRARY DESTINATION ${PROJECT_BUILD_PATH}
  ARCHIVE DESTINATION ${PROJECT_BUILD_PATH}
)