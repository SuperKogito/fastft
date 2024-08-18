# Include the FetchContent module
include(FetchContent)

# FetchFFTW - automatically handle FFTW3
function(FetchFFTW target_name)
	set(FFTW3_BIN_DIR "${CMAKE_BINARY_DIR}/fftw3")
	set(FFTW3_DIR "${FFTW3_BIN_DIR}/src/fftw3")
	# Determine the FFTW tarball URL based on the compiler
	if(MSVC OR MINGW)
		set(FETCH_VALID 1)
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(FFTW_URL "https://fftw.org/pub/fftw/fftw-3.3.5-dll64.zip")
			set(FFTW_TAR_FILENAME "fftw-3.3.5-dll64.zip")
			set(PLATFORM_SUFFIX "X64")
		elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(FFTW_URL "https://fftw.org/pub/fftw/fftw-3.3.5-dll32.zip")
			set(FFTW_TAR_FILENAME "fftw-3.3.5-dll32.zip")
			set(PLATFORM_SUFFIX "X86")
		endif()
		set(FFTW_DEF_FILE "${FFTW3_DIR}/libfftw3f-3.def")
		set(FFTW_DLL_FILE "${FFTW3_DIR}/libfftw3f-3.dll")
	else()
		set(FETCH_VALID 0)
		message(WARNING "${target_name}: Unsupported platform for automatic FFTW 3.3.5 fetching, please satisfy the dependency manually!")
	endif()

	

	if(MSVC)
		set(FFTW_LIB_FILE "${FFTW3_DIR}/fftw3f.lib")
	elseif(MINGW)
		set(FFTW_LIB_FILE "${FFTW3_DIR}/libfftw3f.a")
	endif()

	if(FETCH_VALID)
		message(STATUS "${target_name}: FFTW 3.3.5 can be fetched automatically")
		# Extract the tarball
		include(ExternalProject)
		ExternalProject_Add(
			fftw3
			PREFIX ${FFTW3_BIN_DIR}
			URL ${FFTW_URL}
			#URL_HASH SHA256=<checksum>  # ToDo
			CONFIGURE_COMMAND ""
			DOWNLOAD_EXTRACT_TIMESTAMP TRUE
			BUILD_COMMAND ""         
			INSTALL_COMMAND ""          
			BUILD_BYPRODUCTS ""
			LOG_DOWNLOAD ON
		)
	endif()
	
	if(FETCH_VALID)
		if(MSVC)
			add_custom_target(generate_fftw_import_library
				COMMAND ${CMAKE_COMMAND} -E echo "${GEN_IMPORT_LIB_CMD}"
				COMMAND lib.exe /MACHINE:${PLATFORM_SUFFIX} /def:${FFTW_DEF_FILE} /out:${FFTW_LIB_FILE}
				DEPENDS fftw3
				VERBATIM
				COMMENT "Generating fftw import library for ${CMAKE_CXX_COMPILER_ID}-${PLATFORM_SUFFIX}"
			)

		elseif(MINGW)
			add_custom_target(generate_fftw_import_library
				COMMAND ${CMAKE_COMMAND} -E echo "${GEN_IMPORT_LIB_CMD}"
				COMMAND dlltool -d ${FFTW_DEF_FILE} -l ${FFTW_LIB_FILE} -l ${FFTW_DLL_FILE}
				DEPENDS fftw3
				VERBATIM
				COMMENT "Generating fftw import library for ${CMAKE_CXX_COMPILER_ID}-${PLATFORM_SUFFIX}"
			)
		endif()
	endif()	
	
	if(FETCH_VALID)
		# Ensure that import library generation runs after FFTW is processed
		add_dependencies(generate_fftw_import_library fftw3)
		link_directories(${FFTW3_DIR})
		add_dependencies(${target_name} generate_fftw_import_library fftw3)
		set(fftw_SOURCE_DIR ${FFTW3_DIR} PARENT_SCOPE)
		set(fftw_BINARY_DIR ${FFTW3_DIR} PARENT_SCOPE)
	endif()
endfunction()