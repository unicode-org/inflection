#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
add_custom_target(docs
	COMMAND
	doxygen ${CMAKE_SOURCE_DIR}/docs/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	VERBATIM)
add_dependencies(docs inflection-headers)
