set(CLANG_COMPILER "/usr/local/bin/clang" CACHE STRING "clang compiler")

file(GLOB JIT_SOURCE_FILES "*.cxx")
message("JIT_SOURCE_FILES = ${JIT_SOURCE_FILES}")
add_custom_target(generate_jit_sources ALL)

foreach(JITSRC ${JIT_SOURCE_FILES})
	get_filename_component(JITSRCNAME ${JITSRC} NAME_WE)
	set(${JITSRCNAME}_TARGET ${JITSRCNAME})
	set(${JITSRCNAME}_PATH ${CMAKE_CURRENT_BINARY_DIR}/${JITSRCNAME}_jitpp.cxx)
	set(${JITSRCNAME}_CFLAGS "-D${JITSRCNAME}_PATH=${JITSRCNAME}")
	message("adding target ${${JITSRCNAME}_PATH}")
	add_custom_target(${${JITSRCNAME}_PATH} COMMAND ${CLANG_COMPILER} -x c++ -std=c++11 -E -i ${JITSRC} -o ${${JITSRCNAME}_PATH})
	add_depedencies(generate_jit_sources ${${JITSRCNAME}_PATH})
endforeach(JITSRC)


