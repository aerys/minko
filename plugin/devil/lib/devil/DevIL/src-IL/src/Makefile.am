lib_LTLIBRARIES = libIL.la

libIL_la_SOURCES = il_alloc.c     il_doom.c      il_stack.c    il_pcx.c        il_rle.c \
                   il_bits.c      il_endian.c    il_jpeg.c     il_pic.c        il_sgi.c \
                   il_bmp.c       il_error.c     il_lif.c      il_png.c        il_states.c \
                   il_convbuff.c  il_files.c     il_main.c     il_pnm.c        il_targa.c \
                   il_convert.c   il_gif.c       il_manip.c    il_profiles.c   il_tiff.c \
                   il_cut.c       il_header.c    il_mdl.c      il_quantizer.c  il_utility.c \
                   il_dcx.c       il_icon.c      il_mng.c      il_raw.c        il_wal.c \
                   il_dds.c       il_internal.c  il_pal.c      il_rawdata.c    il_devil.c \
                   il_io.c        il_pcd.c       il_register.c il_psd.c        il_fastconv.c \
                   il_neuquant.c  il_dds-save.c  il_psp.c      il_pix.c        il_pxr.c \
                   il_xpm.c       il_hdr.c       il_icns.c     il_jp2.c        il_vtf.c \
                   il_wbmp.c      il_exr.cpp     il_nvidia.cpp il_squish.cpp   il_iff.c \
                   il_size.c      il_tpl.c       il_fits.c     il_iwi.c        il_blp.c \
                   il_ftx.c       il_dicom.c     il_rot.c	   il_ilbm.c \
                   altivec_common.c altivec_typeconversion.c

AM_CFLAGS = @LIBIL_CFLAGS@
libIL_la_LDFLAGS = @WIN32_EXTRA_LDFLAGS@ @VERSIONINFO@
libIL_la_LIBADD = @LIBPNG@ @LIBJPG@ @LIBTIFF@ @LIBMNG@ @LIBLCMS@ @LIBZ@

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src-IL/include -I$(top_builddir)/include @LIBPNG_INCLUDE@
