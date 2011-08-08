CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( Tessellation )

FILE( GLOB GLSW lib/glsw/*.c lib/glsw/*.h )
FILE( GLOB VECTORMATH lib/vectormath/*.h )

INCLUDE_DIRECTORIES(
    lib/glsw
    lib/libpez
    ../../Source
)

ADD_LIBRARY( PezEcosystem
    ${GLSW}
    ${VECTORMATH}
    ${PEZ}
)

FIND_PACKAGE(GLEW REQUIRED)
INCLUDE_DIRECTORIES( lib/vectormath )
ADD_EXECUTABLE( Geodesic ${CONSOLE_SYSTEM} Geodesic.cpp Geodesic.glsl )
TARGET_LINK_LIBRARIES( Geodesic PezEcosystem ${PLATFORM_LIBS} ${GLEW_LIBRARY} C3)

install(TARGETS Geodesic RUNTIME DESTINATION ${PROJECT_BUILD_PATH})
install(FILES Geodesic.glsl DESTINATION ${PROJECT_BUILD_PATH})