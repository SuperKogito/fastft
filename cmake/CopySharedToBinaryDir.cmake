function(CopySharedToBinaryDir target_name dir)
	if(MSVC OR MINGW)
		# Find all DLL files in the 'dir' directory
		file(GLOB DLL_FILES "${dir}/*.dll")
		# Post-build step to copy DLLs
		foreach(DLL_FILE ${DLL_FILES})
			add_custom_command(
				TARGET ${target_name} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy ${DLL_FILES} $<TARGET_FILE_DIR:${target_name}>
			)
		endforeach()
	endif()
endfunction()