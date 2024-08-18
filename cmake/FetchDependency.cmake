# Include the FetchContent module
include(FetchContent)

# FetchDependency - automatically handle external dependency:
# 1. Fetch content if needed
# 2. Show if the dependency was found or needed to be downloaded
# Note: No checks are performed if the found dependency is compatible with the specified version (git_tag)!
function(FetchDependency msg_prefix lib_name git_url git_tag)
    FetchContent_Declare(${lib_name}
        GIT_REPOSITORY ${git_url}
        GIT_TAG ${git_tag}
    )
   
    # Check if the dependency was previously populated by checking the cache
	# Convert the input variable to uppercase
    string(TOUPPER "${lib_name}" lib_name_uppercase)
    set(is_fetched_var "FETCHCONTENT_SOURCE_DIR_${lib_name_uppercase}")
	
    if(NOT DEFINED ${is_fetched_var})
        message(NOTICE "${msg_prefix}: ${lib_name} not found, downloading tag ${git_tag} from ${git_url}...")
    else()
        message(STATUS "${msg_prefix}: ${lib_name} found")
    endif()
	FetchContent_MakeAvailable(${lib_name})

endfunction()
