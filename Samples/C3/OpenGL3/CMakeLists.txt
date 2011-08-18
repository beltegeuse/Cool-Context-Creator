SET(OpenGL3Test_SRC
	main.cpp
	
	### Gle
	glload/gll_gl_core.c  
	glload/gll_gl_ext.c  
	glload/gll.cpp
)

IF(WIN32)
SET(OpenGL3Test_SRC
	${OpenGL3Test_SRC}
	glload/wgll_ext.c
)
ELSE(WIN32)
SET(OpenGL3Test_SRC 
	${OpenGL3Test_SRC}
	glload/glxl_ext.c
)
ENDIF(WIN32)


INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	../../../Source
)

ADD_EXECUTABLE(OpenGL3Test ${OpenGL3Test_SRC})

target_link_libraries(
  OpenGL3Test
  C3
)

SET_TARGET_PROPERTIES(OpenGL3Test PROPERTIES
  INSTALL_RPATH ${PROJECT_BUILD_PATH}
  INSTALL_RPATH_USE_LINK_PATH true
)

install(TARGETS OpenGL3Test RUNTIME DESTINATION ${PROJECT_BUILD_PATH})