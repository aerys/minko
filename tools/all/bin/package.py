#!/usr/bin/env python2.7
# encoding: utf-8
#
# Copyright (c) 2011 Aerys. All rights reserved.
# Created by Warren Seine on Jul 09, 2011.

from __future__ import print_function
from datetime import date
import os
import sys
import subprocess
import shutil
import getopt
import hashlib
import platform


def die(text, code=1):
    print(text, file=sys.stderr)
    sys.exit(code)


def copytree2(src, dst, symlinks=False):
    names = os.listdir(src)
    if not os.path.exists(dst):
        os.makedirs(dst)
    errors = []
    for name in names:
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                copytree2(srcname, dstname, symlinks)
            else:
                shutil.copy2(srcname, dstname)
        except (IOError, os.error), why:
            errors.append((srcname, dstname, str(why)))
        except Exception, err:
            errors.extend(err.args[0])
    try:
        shutil.copystat(src, dst)
    except WindowsError:
        pass
    except OSError, why:
        errors.extend((src, dst, str(why)))
    if errors:
        raise Exception(errors)

shutil.copytree2 = copytree2


def clean(directory, force=False):
    if not force and os.path.exists(directory) and os.listdir(directory):
        r = None
        while r not in ['y', 'n']:
            r = raw_input("Erase '%s' [yn]? " % (directory)).lower()
        if r != 'y':
            die("error: export failed")
    if os.path.exists(directory) and os.listdir(directory):
        shutil.rmtree(directory)


def md5sum(filename):
    f = open(filename, 'rb')
    m = hashlib.md5()
    while True:
        data = f.read(10240)
        if len(data) == 0:
            break
        m.update(data)
    return m.hexdigest()


def main():
    usage = "usage: make-sdk.py [src] [dst]"

    try:
        opts, args = getopt.getopt(sys.argv[1:], "h", ["help"])
        for o, a in opts:
            if o in ("-h", "--help"):
                die(usage, 0)

    except getopt.GetoptError, e:
        print(e, file=sys.stderr)
        die(usage, 2)

    src = "." if len(args) < 1 else args[0]
    dst = "dist" if len(args) < 2 else args[1]
    tools = None

    if not os.path.exists(src):
        die("error: '%s' doesn't exist" % (src))

    # Make the distribution name.
    today = date.today().strftime("%Y%m%d")
    revision = subprocess.Popen(["git", "rev-parse", "--short", "HEAD"], stdout=subprocess.PIPE).communicate()[0][:7]
    system = platform.uname()[0]
    arch = platform.uname()[4]
    tarball = ("orbit" + "-" + today + "-" + revision + "-" + system + "-" + arch).lower()
    distrib = dst
    dst = os.path.join(dst, tarball)

    # Remove any previous build.
    clean(dst, False)

    # Make sure we're in the root of the repository.
    os.chdir(src)

    # Select the tools specific to platform.
    if sys.platform == "win32":
        tools = "tools/win"
    elif sys.platform == "darwin":
        tools = "tools/mac"
    else:
        tools = "tools/lin"

    # C++ compilation
    if sys.platform == "win32":
        print("warning: please compile in VS2010 first")
    else:
        print("Generating build files")
        if subprocess.call([os.path.join(tools, "bin", "premake4"), "gmake"]):
            die("error: build system generation failed")

        print("Building (release)")
        if subprocess.call(["make", "verbose=1", "config=release"]):
            die("error: build failed")

        print("Testing")
        if subprocess.call([os.path.join(tools, "bin", "premake4"), "check"]):
            die("error: test failed")

    # Compile and/or prepare the compiler binary.
    if sys.platform == "win32":
        subprocess.Popen(["python", "script/setup.py", "py2exe"], cwd="compiler").communicate()
        shutil.move(os.path.join("compiler", "dist"), os.path.join("compiler", "bin"))
        shutil.rmtree(os.path.join("compiler", "build"))
    else:
        os.path.exists("compiler/bin") or os.makedirs("compiler/bin")
        shutil.copy(os.path.join("compiler", "src", "orbitc.py"), os.path.join("compiler", "bin", "orbitc"))
        os.chmod(os.path.join("compiler", "bin", "orbitc"), 0755)

    # Compile and/or prepare the daemon binary.
    if sys.platform == "win32":
        subprocess.Popen(["python", "script/setup.py", "py2exe"], cwd="daemon").communicate()
        shutil.move(os.path.join("daemon", "dist"), os.path.join("daemon", "bin"))
        shutil.rmtree(os.path.join("daemon", "build"))
    else:
        os.path.exists("daemon/bin") or os.makedirs("daemon/bin")
        shutil.copy(os.path.join("daemon", "src", "orbitd.py"), os.path.join("daemon", "bin", "orbitd"))
        shutil.copy(os.path.join("daemon", "src", "orbit.sh"), os.path.join("daemon", "bin", "orbit"))
        os.chmod(os.path.join("daemon", "bin", "orbitd"), 0755)
        os.chmod(os.path.join("daemon", "bin", "orbit"), 0755)

    # Copy release build to distriution directory.
    shutil.copytree2(os.path.join("compiler", "bin"), os.path.join(dst, "bin"))
    shutil.copytree2(os.path.join(tools, "bin"), os.path.join(dst, "sbin"))
    shutil.copytree2(os.path.join("daemon", "bin"), os.path.join(dst, "sbin"))
    shutil.copytree2(os.path.join("framework", "c++", "bin"), os.path.join(dst, "lib"))
    shutil.copytree2(os.path.join("compiler", "share"), os.path.join(dst, "share"))
    shutil.copytree2(os.path.join("applications"), os.path.join(dst, "share", "examples"))
    shutil.copytree(os.path.join("framework", "c++", "src"), os.path.join(dst, "include"), ignore=shutil.ignore_patterns("*Impl.hpp", "*.cpp"))

    # Add the public README.
    shutil.copy("README.md", os.path.join(dst, "README.md"))

    # Finalize the distribution.
    os.chdir(distrib)
    shutil.make_archive(tarball, "zip", ".", tarball)
    print("Tarball is ready: %s (%s)" % (dst + ".zip", md5sum(tarball + ".zip")))


if __name__ == "__main__":
    main()
