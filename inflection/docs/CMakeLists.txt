#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
add_custom_target(docs
	COMMAND
	doxygen -u ../../docs/Doxyfile
	VERBATIM)
