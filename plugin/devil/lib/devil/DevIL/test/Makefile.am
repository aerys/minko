all_include = $(srcdir)/../include
AM_CPPFLAGS = -I $(all_include)

check_PROGRAMS = testil
check_SCRIPTS = format_test/format_checks.sh 

if BUILD_ILU
check_SCRIPTS += format_test/ilu_checks.sh
endif #BUILD_ILU

testil_SOURCES = format_test/testil.c
testil_LDADD =
if BUILD_ILU
testil_LDADD += ../lib/libILU.la
else
testil_LDADD += ../lib/libIL.la
endif

TESTS_ENVIRONMENT = /bin/bash
TESTS = $(check_SCRIPTS)
