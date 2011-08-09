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

SET_TARGET_PROPERTIES(FullScreenTest PROPERTIES
  INSTALL_RPATH ${PROJECT_BUILD_PATH}
  INSTALL_RPATH_USE_LINK_PATH true
)

install(TARGETS FullScreenTest RUNTIME DESTINATION ${PROJECT_BUILD_PATH})