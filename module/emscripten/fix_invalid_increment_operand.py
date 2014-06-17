import tempfile, shutil, os, sys

file = sys.argv[1]
temp_file = tempfile.mktemp()
shutil.copyfile(file, temp_file)

with open(file, "wt") as fout:
    with open(temp_file, "rt") as fin:
        for line in fin:
            fout.write(line.replace('++0', '1'))
