lib_LTLIBRARIES = libILU.la

libILU_la_SOURCES = ilu_alloc.c       ilu_internal.c  ilu_noise.c \
                    ilu_scale.c       ilu_error.c     ilu_main.c \
                    ilu_rotate.c      ilu_scaling.c   ilu_filter.c \
                    ilu_manip.c       ilu_scale2d.c   ilu_states.c \
                    ilu_filter_rcg.c  ilu_mipmap.c    ilu_scale3d.c \
                    ilu_utilities.c	  ilu_region.c

AM_CFLAGS = @LIBILU_CFLAGS@

libILU_la_LDFLAGS = @WIN32_EXTRA_LDFLAGS@ @VERSIONINFO@ \
                    $(top_srcdir)/src-IL/src/libIL.la

INCLUDES = -I$(top_builddir)/include -I$(top_builddir)/src-ILU/include
