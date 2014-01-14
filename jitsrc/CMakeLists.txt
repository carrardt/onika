set(CLANG_COMPILER "/usr/local/bin/clang" CACHE STRING "clang compiler")

file(GLOB JIT_SOURCE_FILES "*.cxx")
add_custom_target(generate_jit_sources ALL)

foreach(JITSRC ${JIT_SOURCE_FILES})
	get_filename_component(JITSRCNODIR ${JITSRC} NAME)
	get_filename_component(JITSRCNAME ${JITSRCNODIR} NAME_WE)
	set(${JITSRCNAME}_TARGET ${JITSRCNAME}-jit-preprocessed-cxx)
	set(${JITSRCNAME}_PATH ${CMAKE_CURRENT_BINARY_DIR}/${JITSRCNAME}-jitpp.cxx)
	set(${JITSRCNAME}_CFLAGS -D${JITSRCNAME}_PATH=\"${${JITSRCNAME}_PATH}\")
	set(${JITSRCNAME}_CFLAGS -D${JITSRCNAME}_PATH=\"${${JITSRCNAME}_PATH}\" PARENT_SCOPE)
	message("${JITSRCNAME}_CFLAGS = ${${JITSRCNAME}_CFLAGS}")
	add_custom_target(${${JITSRCNAME}_TARGET} COMMAND ${CLANG_COMPILER} ${CMAKE_CXX_FLAGS} -I${ONIKA_INCLUDE_DIR} -E ${JITSRC} -o ${${JITSRCNAME}_PATH})
	add_dependencies(generate_jit_sources ${${JITSRCNAME}_TARGET})
endforeach(JITSRC)


