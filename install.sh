#!/bin/bash

if [ ! $(whoami) == "root" ] && [ ! $LIB_PREFIX ] ; then
	echo "To install, you must either be root, or you need to specify a \$LIB_PREFIX variable to tell me where to install the library to."
	exit 1
fi

if [ ! $LIB_PREFIX ] ; then
	LIB_PREFIX="/usr/lib/"
fi

cp -v libvimradiation.so.* /tmp/ &&
rm -rf $LIB_PREFIX/libvimradiation.so.* &&
mv -v /tmp/libvimradiation.so.* $LIB_PREFIX/ &&
ln -svf $LIB_PREFIX/libvimradiation.so.* $LIB_PREFIX/libvimradiation.so
 
# do some vim python stuff here
