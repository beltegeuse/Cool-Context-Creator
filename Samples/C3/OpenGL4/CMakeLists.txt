# FIND ALL C FILES IN lib/glsw
FILE( GLOB GLSW lib/glsw/*.c )

# This project need Glew
#
FIND_PACKAGE(GLEW REQUIRED)

INCLUDE_DIRECTORIES(
    lib/glsw
    lib/libpez
    lib/vectormath
    ../../../Source
)

ADD_EXECUTABLE( OpenGL4Test Geodesic.cpp Geodesic.glsl ${GLSW} )

TARGET_LINK_LIBRARIES( OpenGL4Test 
	${GLEW_LIBRARY}  
	C3
)

install(TARGETS OpenGL4Test RUNTIME DESTINATION ${C3_BINARY_DIR})
install(FILES Geodesic.glsl DESTINATION ${C3_BINARY_DIR})