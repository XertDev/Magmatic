add_subdirectory(device_enumeration)
add_subdirectory(first_test)
add_subdirectory(sound_test)
add_subdirectory(model_info)
add_subdirectory(simple_ecs_example)

add_custom_target(copy-example-files ALL
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/examples/resources ${CMAKE_BINARY_DIR}/examples/resources
		)

add_dependencies(sound_test copy-example-files)
add_dependencies(main-test copy-example-files)
add_dependencies(model_info copy-example-files)