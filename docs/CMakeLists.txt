#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
add_custom_target(docs
	COMMAND
	doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	VERBATIM)
add_dependencies(docs inflection-headers)
