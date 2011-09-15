SET(OpenGL1Test_SRC
	main.cpp
)


INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	../../../Source
)

ADD_EXECUTABLE(OpenGL1Test ${OpenGL1Test_SRC})

target_link_libraries(
  OpenGL1Test
  C3
)

install(TARGETS OpenGL1Test RUNTIME DESTINATION ${CMAKE_BINARY_DIR})