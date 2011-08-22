SET(FullScreenTest_SRC
	main.cpp
)


INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	../../../Source
)

ADD_EXECUTABLE(FullScreenTest ${FullScreenTest_SRC})

target_link_libraries(
  FullScreenTest
  C3
)

install(TARGETS FullScreenTest RUNTIME DESTINATION ${CMAKE_BINARY_DIR})