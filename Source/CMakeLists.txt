
#
# Compilation Flags
#
IF(CMAKE_BUILD_TYPE MATCHES debug)
	ADD_DEFINITIONS("-Wall -W -D_DEBUG") #Pour afficher les warning lors de la compilation
	# Add profiling on Linux (Mingw doesn't support this feature)
	IF(LINUX)
		ADD_DEFINITIONS("-pg")
	ENDIF()
	IF(DEBUG_TRACE)
		ADD_DEFINITIONS("-D_TRACE")
	ENDIF()
ENDIF(CMAKE_BUILD_TYPE MATCHES debug)

#
# Multi-platform sources
#
SET(LIB_C3_SRCS
	C3/Exceptions.cpp
	C3/Window.cpp
	C3/WindowImpl.cpp
	C3/Clock.cpp
)

SET(LIB_C3_H
	C3/Events.h
	C3/OpenGLContextSettings.h
	C3/Window.h
	C3/WindowImpl.h
	C3/WindowMode.h
	C3/Clock.h
	C3/Config.h
	C3/WindowHandle.h
)

#
# Specific platform sources
#
IF(WIN32)
SET(LIB_C3_SRCS
	${LIB_C3_SRCS}
	${LIB_C3_H}
	C3/Win32/WindowImplWin32.cpp
	C3/Win32/WindowImplWin32.h
)
ELSE()
SET(LIB_C3_SRCS
	${LIB_C3_SRCS}
	C3/Linux/WindowImplLinux.cpp
)
ENDIF()

#
# Headers Installation
#
INSTALL(FILES
	${LIB_C3_H}
DESTINATION ${C3_BINARY_DIR}/include/C3)

#
# Lib configuration
#
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
)

IF(MSVC)
	ADD_LIBRARY(C3 STATIC ${LIB_C3_SRCS})
ELSE()
	ADD_LIBRARY(C3 SHARED ${LIB_C3_SRCS})
ENDIF()

target_link_libraries(
  C3
  ${OPENGL_LIBRARY}
  ${GLEW_LIBRARY}
  ${X11_Xrandr_LIB}
)

install(TARGETS C3 
  RUNTIME DESTINATION ${CMAKE_BINARY_DIR}
  LIBRARY DESTINATION ${CMAKE_BINARY_DIR}
  ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}
)