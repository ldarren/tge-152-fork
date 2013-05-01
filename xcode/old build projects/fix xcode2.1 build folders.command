#!/bin/bash
################################################################################
# Applies a work around fix for Xcode2.1+, wherein an extra dir is appended to
# the end of the build products path. We us a symbolic link ( alias ) to trick
# Xcode into playing ball.
################################################################################


# Different versions of macosx set the working directory to different places.
# We try to cope, by finding the working dir of this .command file.

dir=`dirname "$0"`
echo 'started in directory:' `pwd`
if [ -e $dir ]; then
   cd $dir
fi


# We create the build folder if it does not exist, or move it aside if it does.
if [ -e build ]; then
	mv build build-old-`date "+%m.%d.%C%y-%H.%M.%S"`
fi
mkdir build

# Finally, we make the symbolic links

cd build
ln -s ../../example Default
ln -s ../../example Development
ln -s ../../example Deployment

