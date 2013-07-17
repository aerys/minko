#!/bin/sh

BIN_MAKEFILES="examples/Makefile"
LIB_MAKEFILES="framework/Makefile plugins/jpeg/Makefile plugins/png/Makefile plugins/webgl/Makefile"
ALL_MAKEFILES="$BIN_MAKEFILES $LIB_MAKEFILES"

for F in $LIB_MAKEFILES; do
	sed -i $F -r -e 's/-rcs/rk/g'
done

for F in $BIN_MAKEFILES; do
	sed -i $F -r -e 's/^.*TARGET\s*=.*$/&.bc/'
done
