#!/bin/bash

if [[ ! -f sdk.lua ]]; then
	echo "error: cannot clean from outside the Minko SDK" >/dev/stderr
fi

for DIR in framework plugins tests examples; do
	find ${DIR} -type d -name bin -print -exec rm -rf "{}" \; 2>/dev/null
	find ${DIR} -type d -name obj -print -exec rm -rf "{}" \; 2>/dev/null
done

echo "doc"
rm -rf doc
