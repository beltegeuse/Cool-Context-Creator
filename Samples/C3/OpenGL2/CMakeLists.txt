SET(OpenGL2Test_SRC
	main.cpp
)


INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	../../../Source
)

ADD_EXECUTABLE(OpenGL2Test ${OpenGL2Test_SRC})

target_link_libraries(
  OpenGL2Test
  C3
)

install(TARGETS OpenGL2Test RUNTIME DESTINATION ${C3_BINARY_DIR})