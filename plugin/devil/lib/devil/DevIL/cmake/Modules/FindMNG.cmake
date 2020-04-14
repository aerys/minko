# Denton: Modified from the libsquish one
FIND_PATH(MNG_INCLUDE_DIR mng.h PATHS . mng .. ../mng DOC "Directory containing libmng headers")
FIND_LIBRARY(MNG_LIBRARY NAMES mng libmng PATHS . mng .. ../mng PATH_SUFFIXES lib lib64 release minsizerel relwithdebinfo DOC "Path to libmng library")

SET(MNG_LIBRARIES ${MNG_LIBRARY})

IF (MNG_LIBRARY AND MNG_INCLUDE_DIR)
   SET(MNG_FOUND TRUE)
   MESSAGE(STATUS "Found libmng: ${MNG_LIBRARY}")
ENDIF (MNG_LIBRARY AND MNG_INCLUDE_DIR)
