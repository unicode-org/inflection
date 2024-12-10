#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
set(morphuntion_compile_definitions MORPHUNTION_DATA_ROOT_DIR=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}\" ENABLE_ALL_FEATURES=1)

# morphuntion and icu4cxx definitions
set(ICU4CXX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(MORPHUNTION_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE MORPHUNTION_SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MORPHUNTION_SOURCES})

file(GLOB_RECURSE TOOLS_SUBSET
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/dictionary/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/exception/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/resources/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/lang/features/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/tokenizer/trie/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/util/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/morphuntion/Object.cpp
)
list(FILTER TOOLS_SUBSET EXCLUDE REGEX "-c.cpp$")

set(MORPHUNTION_OTHER_SOURCES ${MORPHUNTION_SOURCES})
foreach(TOOL_SOURCE IN LISTS TOOLS_SUBSET)
    list(REMOVE_ITEM MORPHUNTION_OTHER_SOURCES ${TOOL_SOURCE})
endforeach()

# Morphuntion tool objects target
if(XCODE)
	add_library(morphuntion_tool_objs INTERFACE)
	target_sources(morphuntion_tool_objs INTERFACE ${TOOLS_SUBSET})
	target_include_directories(morphuntion_tool_objs INTERFACE ${MORPHUNTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
	target_compile_definitions(morphuntion_tool_objs INTERFACE ${morphuntion_compile_definitions})
else()
	add_library(morphuntion_tool_objs OBJECT)
	target_sources(morphuntion_tool_objs PRIVATE ${TOOLS_SUBSET})
	set_property(TARGET morphuntion_tool_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
	target_include_directories(morphuntion_tool_objs PUBLIC ${MORPHUNTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
	target_link_libraries(morphuntion_tool_objs PRIVATE resource_objs marisa_objs CoreFoundation ICU::uc ICU::i18n)
	target_compile_definitions(morphuntion_tool_objs PUBLIC ${morphuntion_compile_definitions})
endif()
add_dependencies(morphuntion_tool_objs generated_src)
#End section

# libmorphuntion target
add_library(morphuntion SHARED ${MORPHUNTION_OTHER_SOURCES})
target_include_directories(morphuntion PUBLIC ${MORPHUNTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
target_link_options(morphuntion PRIVATE $<$<PLATFORM_ID:Darwin>:-dead_strip>)
target_compile_definitions(morphuntion PUBLIC ${morphuntion_compile_definitions})
target_link_libraries(morphuntion
		PRIVATE
			ICU::uc ICU::i18n
			CoreFoundation
			marisa_objs resource_objs morphuntion_tool_objs
)
add_link_options(-no_inits)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set_target_properties(morphuntion PROPERTIES
		BUILD_WITH_INSTALL_RPATH TRUE
		BUILD_WITH_INSTALL_NAME_DIR TRUE
		INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR}
	)
else()
	set_target_properties(morphuntion PROPERTIES
		BUILD_WITH_INSTALL_RPATH TRUE
		BUILD_RPATH_USE_ORIGIN TRUE
		INSTALL_RPATH $ORIGIN
	)
endif()

add_custom_command(
	OUTPUT ${MORPHUNTION_INCLUDE_ROOT}/morphuntion/version.h
	COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DPACKAGE_DIR=${MORPHUNTION_INCLUDE_ROOT} -P ${CMAKE_SOURCE_DIR}/cmake/installPublicHeaders.cmake
	DEPENDS generated_version_header
)
add_custom_target(morphuntion-headers ALL DEPENDS ${MORPHUNTION_INCLUDE_ROOT}/morphuntion/version.h)

set(CXX_CHECK_HEADER_FLAGS "${CXX_STD_LIB_FLAG}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	get_target_property(CF_INCLUDES CoreFoundation INTERFACE_INCLUDE_DIRECTORIES)
	set(CF_INCLUDES "-I${CF_INCLUDES}")
	get_target_property(ICU_INCLUDES ICU::uc INTERFACE_INCLUDE_DIRECTORIES)
	set(ICU_INCLUDES "-I${ICU_INCLUDES}")
endif()

add_custom_target(check-headers
	COMMAND ${CMAKE_COMMAND} -DCPP_COMPILER_BIN=${CMAKE_CXX_COMPILER} -DC_COMPILER_BIN=${CMAKE_C_COMPILER} -DHEADER_DIR=${MORPHUNTION_INCLUDE_ROOT} "-DCXX_CHECK_HEADER_FLAGS=${CXX_CHECK_HEADER_FLAGS}" -DCXX_STANDARD=${CMAKE_CXX_STANDARD} -DCF_INCLUDES=${CF_INCLUDES} -DICU_INCLUDES=${ICU_INCLUDES} -P ${CMAKE_SOURCE_DIR}/cmake/checkCompileHeaders.cmake
	VERBATIM
	DEPENDS morphuntion-headers
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_dependencies(check-headers CoreFoundation_dev)
endif()

set(ARCHS_DEFAULT_VAL "x86_64")
if(DEFINED ENV{ARCHS})
	set(ARCHS_DEFAULT_VAL $ENV{ARCHS})
endif()
set(ARCHS ${ARCHS_DEFAULT_VAL} CACHE STRING "Used in tapi install api" FORCE)
string(REPLACE " " ";" ARCH_LIST ${ARCHS})
foreach(ARCH IN LISTS ARCH_LIST)
	list(APPEND ARCH_TARGET_FLAG_LIST "--target=${ARCH}-$ENV{LLVM_TARGET_TRIPLE_VENDOR}-$ENV{LLVM_TARGET_TRIPLE_OS_VERSION}$ENV{LLVM_TARGET_TRIPLE_SUFFIX}")
endforeach()
list(JOIN ARCH_TARGET_FLAG_LIST " " ARCH_TARGET_FLAGS)
get_target_property(MORPHUNTION_INSTALL_NAME_DIR morphuntion INSTALL_NAME_DIR)
add_custom_target(install_api
	COMMAND
		sh -c "xcrun --sdk ${BUILD_SDKROOT} tapi installapi -dynamiclib -xc++ -std=c++${CMAKE_CXX_STANDARD} ${ARCH_TARGET_FLAGS} -install_name ${MORPHUNTION_INSTALL_NAME_DIR}/libmorphuntion.dylib -current_version 1 -compatibility_version 1 -extra-private-header $ENV{DSTROOT}${CMAKE_INSTALL_FULL_INCLUDEDIR} -extra-public-header $ENV{DSTROOT}${CMAKE_INSTALL_FULL_INCLUDEDIR} -o $ENV{BUILT_PRODUCTS_DIR}/libmorphuntion.tbd $ENV{DSTROOT}"
	VERBATIM
)
add_dependencies(install_api morphuntion-headers)