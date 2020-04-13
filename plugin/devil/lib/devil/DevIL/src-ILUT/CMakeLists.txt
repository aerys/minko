cmake_minimum_required(VERSION 2.6)

project(ILUT)

file(GLOB ILUT_SRCS src/*.cpp)
file(GLOB ILUT_INC include/*.h ../include/IL/devil_internal_exports.h ../include/IL/ilut.h)
file(GLOB ILUT_RSRC)

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
	add_definitions(-DALLEGRO_MSVC)
	add_definitions(-DIL_USE_PRAGMA_LIBS)
	if(BUILD_SHARED_LIBS)
		add_definitions(-DILUT_DLL)
        set(ILUT_SRCS ${ILUT_SRCS} msvc/ilut.def)
		if(UNICODE)
			set(ILUT_RSRC ${ILUT_RSRC} "msvc/resources/IL Logo.ico" "msvc/ILUT Unicode.rc")
		else(UNICODE)
			set(ILUT_RSRC ${ILUT_RSRC} "msvc/resources/IL Logo.ico" "msvc/ILUT.rc")
		endif(UNICODE)
		set(CMAKE_C_FLAGS_RELEASE "/MT /O2")
	else(BUILD_SHARED_LIBS)
		add_definitions(-DIL_STATIC_LIB)
	endif(BUILD_SHARED_LIBS)
endif(WIN32)

source_group("Source Files" FILES src/*.cpp)
source_group("Header Files" FILES ${ILUT_INC} )
source_group("Resource Files" FILES ${ILUT_RSRC} )

# Remove SHARED to create a static library
add_library(ILUT SHARED ${ILUT_SRCS} ${ILUT_INC} ${ILUT_RSRC})

## add link sub library info
target_link_libraries(ILUT
	IL
	ILU
)

# generate pkg-config file
configure_file( pkgconfig/ILUT.pc.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ILUT.pc @ONLY)

# Sets the output folders
set(LIBDIR "../lib/")
set(LIBDIR "${LIBDIR}${EX_PLATFORM_NAME}")
if(UNICODE)
	set(LIBDIR "${LIBDIR}/unicode")
endif(UNICODE)
set_target_properties(ILUT PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBDIR})
set_target_properties(ILUT PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBDIR})
set_target_properties(ILUT PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${LIBDIR})

if(UNICODE)
	MESSAGE("Compiling ILUT Unicode")
	add_definitions(-DUNICODE -D_UNICODE)
endif(UNICODE)

# Installation
install (TARGETS ILUT
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install (FILES ../include/IL/ilut.h DESTINATION include/IL)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ILUT.pc
	DESTINATION lib/pkgconfig
)

