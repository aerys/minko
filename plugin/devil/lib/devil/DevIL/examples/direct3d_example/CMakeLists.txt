PROJECT(Example_D3D)

FILE(GLOB Example_D3D_SRCS *.c *.cpp *.h )

INCLUDE_DIRECTORIES(

	${CMAKE_CURRENT_SOURCE_DIR}
	${DevIL_SOURCE_DIR}/include
	${DevIL_SOURCE_DIR}/../include
)



#
# for windows add the .def and .rc files to the source list
#
IF(WIN32)
	IF(BUILD_SHARED_LIBS)
	ENDIF(BUILD_SHARED_LIBS)
ENDIF(WIN32)

ADD_EXECUTABLE(Example_D3D Example_D3D_SRCS)

## add link sub library info

TARGET_LINK_LIBRARIES(Example_D3D
	
	DevIL
	DevILU
	DevILUT
	lcms
	libjpeg
	libmng
	libpng
	libtiff
	zlib
)

INCLUDE_DIRECTORIES(

	 ${DevIL_SOURCE_DIR}
	 ${DevILU_SOURCE_DIR}
	 ${DevILUT_SOURCE_DIR}
	 ${LCMS_SOURCE_DIR}
	 ${LIBJPEG_SOURCE_DIR}
	 ${LIBMNG_SOURCE_DIR}
	 ${LIBPNG_SOURCE_DIR}
	 ${LIBTIFF_SOURCE_DIR}
	 ${ZLIB_SOURCE_DIR}
)
