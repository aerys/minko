#!/usr/bin/env python

from __future__ import print_function
import os
import os.path
import sys
import subprocess

def main():
    if len(sys.argv) < 2:
        print('usage: empkg.py <data file>', file=sys.stderr)
        sys.exit(1)

    data = sys.argv[1]
    html = os.path.splitext(data)[0] + '.html'
    preload = os.path.splitext(data)[0] + '.preload'
    js = preload + '.js'

    if not os.path.isfile(html):
        print('no .html file found next to .data', file=sys.stderr)
        sys.exit(1)

    EMSCRIPTEN = os.getenv('EMSCRIPTEN')

    binary = os.path.join(EMSCRIPTEN, 'tools', 'file_packager.py')

    files = []

    if os.path.isfile(preload):
        with open(preload, 'r') as f:
            files += f.read().splitlines()

    if os.getenv('verbose') != '0':
        print(' '.join(['python', binary, data, '--js-output=' + js, '--preload'] + files))

    subprocess.call(['python', binary, data, '--js-output=' + js, '--preload'] + files)

    content = ""

    with open(html, "r") as f:
        content = f.read()
    with open(html, "w") as f:
        script = '<script type="text/javascript" async src="' + os.path.basename(js) + '"></script>'
        f.write(content.replace('{{{ PRELOAD }}}', script))

if __name__ == '__main__':
    main()
