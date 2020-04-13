lib_LTLIBRARIES = libILUT.la

libILUT_la_SOURCES = ilut_allegro.c     ilut_main.c \
                     ilut_sdlsurface.c  ilut_states.c \
                     ilut_internal.c    ilut_opengl.c \
                     ilut_win32.c       ilut_directx.c \
					 ilut_directx9.c    ilut_x11.c

AM_CFLAGS = @LIBILUT_CFLAGS@

libILUT_la_LDFLAGS = @WIN32_EXTRA_LDFLAGS@ @VERSIONINFO@ \
                     $(top_builddir)/src-IL/src/libIL.la \
                     $(top_builddir)/src-ILU/src/libILU.la \
                     @LIBSDL@ @LIBALLEGRO@ @LIBILUT_WIN32LIBPATH@

libILUT_la_LIBADD = @LIBGL@ @LIBGDI32@ @X_LIBS@ @LIBX11@

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src-ILUT/include \
           @HSDL@ @HALLEGRO@
