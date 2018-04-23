import os
import sys
import string
import shutil

#Generate the file paths to traverse, or a single path if a file name was given
def getfiles(path):
    if os.path.isdir(path):
        for root, dirs, files in os.walk(path):
            for name in files:
                yield os.path.join(root, name)
    else:
        yield path

destination = "data/"
fromdir = "sentences"
for f in getfiles(fromdir):
    filename = f.split( '/')[-1]
    if os.path.isfile(destination+filename):
        filename = f.replace(fromdir,"",1).replace("/","_")
    #os.rename(f, destination+filename)
    shutil.copy(f, destination+filename)