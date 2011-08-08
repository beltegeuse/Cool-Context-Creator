CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( Tessellation )

FILE( GLOB GLSW lib/glsw/*.c lib/glsw/*.h )
FILE( GLOB VECTORMATH lib/vectormath/*.h )

IF( WIN32 )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.windows.cpp
    )

    SOURCE_GROUP( "shaders" FILES Geodesic.glsl )
    SOURCE_GROUP( "glew" FILES ${GLEW} )
    SOURCE_GROUP( "glsw" FILES ${GLSW} )
    SOURCE_GROUP( "vectormath" FILES ${VECTORMATH} )
    SOURCE_GROUP( "pez" FILES ${PEZ} )

    ADD_DEFINITIONS( -DGLEW_STATIC )
    #ADD_DEFINITIONS( /wd4996 /TP )
    SET( PLATFORM_LIBS opengl32 )
    SET( CONSOLE_SYSTEM WIN32 )

ELSEIF( APPLE )

    # Create a nibless project per http://blog.hyperjeff.net/code?id=292
    
    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.cocoa.m
    )

    FIND_LIBRARY( COCOA_LIBRARY Cocoa )
    
    SET( PLATFORM_LIBS
        ${COCOA_LIBRARY}
        /System/Library/Frameworks/OpenGL.framework )

ELSEIF( UNIX )

    FILE( GLOB PEZ
        lib/libpez/pez.h
        lib/libpez/pez.linux.cpp
    )

    SET( PLATFORM_LIBS X11 GL )

ENDIF()

INCLUDE_DIRECTORIES(
    lib/glsw
    lib/libpez
)

ADD_LIBRARY( PezEcosystem
    ${GLSW}
    ${VECTORMATH}
    ${PEZ}
)

FIND_PACKAGE(GLEW REQUIRED)
INCLUDE_DIRECTORIES( lib/vectormath )
ADD_EXECUTABLE( Geodesic ${CONSOLE_SYSTEM} Geodesic.cpp Geodesic.glsl )
TARGET_LINK_LIBRARIES( Geodesic PezEcosystem ${PLATFORM_LIBS} ${GLEW_LIBRARY} )

install(TARGETS Geodesic RUNTIME DESTINATION ${PROJECT_BUILD_PATH})
install(FILES Geodesic.glsl DESTINATION ${PROJECT_BUILD_PATH})