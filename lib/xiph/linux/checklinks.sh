#! /bin/bash

FILES="libogg.so.0.5.2
libtheora.so.0.1.0
libvorbisenc.so.2.0.0
libvorbisfile.so.3.1.0
libvorbis.so.0.3.0"

DIR=`dirname $0`
CWD=`pwd`

cd ${DIR}
for file in $FILES
do
  # Check if library file and its shared counter part exists.
  link=`ls ${file} | awk -F. '{printf $1"."$2 "\n" }'`
  if [ -e ${file} -a ! -e ${link} ]
  then      
    	echo "creating link from TARGET ${DIR}/${file} to LINK_NAME ${DIR}/${link}";
	ln -sf ${file} ${link}
	ln -sf ${file} ${link}.0
    continue                # On to next.
   fi

done  

cd ${CWD}

exit 0
