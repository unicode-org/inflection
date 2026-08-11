#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
add_custom_target(docs
	COMMAND
	doxygen ../../docs/Doxyfile
	VERBATIM)
add_dependencies(docs inflection-headers)
