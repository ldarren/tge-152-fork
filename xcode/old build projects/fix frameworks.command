#!/bin/bash

################################################################################
# Fixes damaged frameworks after they have been checked out of CVS.
# This is neccesary because CVS does not deal well with symbolic links.
################################################################################


# Different versions of macosx set the working directory to different places.
# We try to cope, by finding the working dir of this .command file.

dir=`dirname "$0"`
echo 'started in directory:' `pwd`
if [ -e $dir ]; then
   cd $dir
fi

# Ok, we're now pretty sure we're in the /.../torque/pb directory.
# Move up one dir, and save it off for future use.
cd ..
basedir=`pwd`
echo "moving to directory: $basedir"


# We set up some convinience variables here...
openaldir="lib/openal/macosx"
xiphdir="lib/xiph/macosx"
version='A'
current='Versions/Current'
headers='Headers'
resources='Resources'


for frameworkname in "$openaldir/OpenAL" "$xiphdir/Ogg" "$xiphdir/Vorbis" "$xiphdir/Theora" ; do
   framework="$frameworkname.framework"
   frameworkname=`basename "$frameworkname"`
   echo "-------- fixing " `basename "$framework"` " ---------"
   echo "entering $framework"
   cd $framework
   if [ ! -h $current ]; then
      rm  $current
      ln -s "$version/" $current
      echo "fixing $framework/$current"
   fi
   if [ ! -h $headers ]; then
      rm $headers
      ln -s "$current/$headers/" $headers
      echo "fixing $framework/$headers"
   fi
   if [ ! -h $resources ]; then
      rm $resources
      ln -s "$current/$resources/" $resources
      echo "fixing $framework/$resources"
   fi
   if [ ! -x "$current/$frameworkname" ]; then
      chmod 755 "$current/$frameworkname"
   fi
   if [ ! -h $frameworkname ]; then
      rm $frameworkname
      ln -s "$current/$frameworkname" $frameworkname
      echo "fixing $framework/$frameworkname"
   fi 
   cd $basedir
done
