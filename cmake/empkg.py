#!/usr/bin/env python

from __future__ import print_function
import os
import os.path
import sys
import shutil
import subprocess

def main():
    if len(sys.argv) < 2:
        print('usage: empkg.py <data file>', file=sys.stderr)
        sys.exit(1)

    targetdir = os.path.dirname(sys.argv[1])
    data = os.path.basename(sys.argv[1])
    html = os.path.splitext(data)[0] + '.html'
    preload = os.path.splitext(data)[0] + '.preload.js'
    embeddir = 'embed'

    if not os.path.isdir(os.path.join(targetdir, embeddir)):
        return

    if not os.path.isfile(os.path.join(targetdir, html)):
        print('no .html file found next to .data', file=sys.stderr)
        sys.exit(1)

    EMSCRIPTEN = os.getenv('EMSCRIPTEN')

    binary = os.path.join(EMSCRIPTEN, 'tools', 'file_packager.py')

    files = [os.path.join(embeddir, name) + '@' + name for name in os.listdir(os.path.join(targetdir, embeddir))]
    print(files)

    if os.getenv('verbose') != '0':
        print(' '.join(['python', binary, data, '--js-output=' + preload, '--preload'] + files))

    subprocess.call(['python', binary, data, '--js-output=' + preload, '--preload'] + files, cwd=targetdir)

    shutil.rmtree(os.path.join(targetdir, embeddir))

    content = ""

    with open(os.path.join(targetdir, html), "r") as f:
        content = f.read()
    with open(os.path.join(targetdir, html), "w") as f:
        script = '<script type="text/javascript" async src="' + os.path.basename(preload) + '"></script>'
        f.write(content.replace('{{{ PRELOAD }}}', script))

if __name__ == '__main__':
    main()
