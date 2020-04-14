cmake_minimum_required(VERSION 2.6)

project(ILU)

file(GLOB ILU_SRCS src/*.cpp)
file(GLOB ILU_INC include/*.h ../include/IL/devil_internal_exports.h ../include/IL/ilu.h)
file(GLOB ILU_RSRC)

# From http://stackoverflow.com/questions/17317350/compiling-32-and-64-bit
MESSAGE(" ")
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "64 bit compiler detected" )
    SET( EX_PLATFORM 64 )
    SET( EX_PLATFORM_NAME "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
    MESSAGE( "32 bit compiler detected" )
    SET( EX_PLATFORM 32 )
    SET( EX_PLATFORM_NAME "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

include_directories( include ../include )

# for windows add the .def and .rc files to the source list
if(WIN32)
    #add_definitions(-DIL_USE_PRAGMA_LIBS)
	if(BUILD_SHARED_LIBS)
		add_definitions(-DILU_DLL)
        set(ILU_SRCS ${ILU_SRCS} msvc/ilu.def)
		if(UNICODE)
			set(ILU_RSRC ${ILU_RSRC} "msvc/resources/IL Logo.ico" "msvc/ILU Unicode.rc")
		else(UNICODE)
			set(ILU_RSRC ${ILU_RSRC} "msvc/resources/IL Logo.ico" "msvc/ILU.rc")
		endif(UNICODE)
		set(CMAKE_C_FLAGS_RELEASE "/MT /O2")
	else(BUILD_SHARED_LIBS)
		add_definitions(-DIL_STATIC_LIB)
	endif(BUILD_SHARED_LIBS)
endif(WIN32)


source_group("Source Files" FILES src/*.cpp)
source_group("Header Files" FILES ${ILU_INC} )
source_group("Resource Files" FILES ${ILU_RSRC} )

# Remove SHARED to create a static library
add_library(ILU SHARED ${ILU_SRCS} ${ILU_INC} ${ILU_RSRC})


## ILU requires IL
target_link_libraries(ILU
	IL
)

# generate pkg-config file
# TODO: add Requires.private or Libs.private
# (needed to support static linking?)
configure_file( pkgconfig/ILU.pc.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ILU.pc @ONLY)

# Sets the output folders
set(LIBDIR "../lib/")
set(LIBDIR "${LIBDIR}${EX_PLATFORM_NAME}")
if(UNICODE)
	set(LIBDIR "${LIBDIR}/unicode")
endif(UNICODE)
set_target_properties(ILU PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBDIR})
set_target_properties(ILU PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBDIR})
set_target_properties(ILU PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${LIBDIR})

if(UNICODE)
	MESSAGE("Compiling ILU Unicode")
	add_definitions(-DUNICODE -D_UNICODE)
endif(UNICODE)

# Installation
install (TARGETS ILU
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install (FILES ../include/IL/ilu.h DESTINATION include/IL)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ILU.pc
	DESTINATION lib/pkgconfig
)

