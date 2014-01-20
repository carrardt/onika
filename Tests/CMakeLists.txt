file(GLOB TEST_SOURCE_FILES "*.cpp")

foreach(TSTSRC ${TEST_SOURCE_FILES})
	get_filename_component(TSTSRCNODIR ${TSTSRC} NAME)
	get_filename_component(TSTSRCNAME ${TSTSRCNODIR} NAME_WE)
	add_executable(${TSTSRCNAME} ${TSTSRC})
endforeach(TSTSRC)
