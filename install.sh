#!/bin/bash

SUDO=
if [ ! $(whoami) == "root" ] && [ ! $LIB_PREFIX ] ; then
	echo "To install the library in /usr/lib you must be root. This is why I am asking for sudo."
	echo "To install as non-root please specify \$LIB_PREFIX so I know where to install the library"
	SUDO=sudo
fi

if [ ! $LIB_PREFIX ] ; then
	LIB_PREFIX="/usr/lib/"
fi

$SUDO cp -v libvimradiation.so.* /tmp/ &&
$SUDO rm -rfi $LIB_PREFIX/libvimradiation.so.* &&
$SUDO mv -v /tmp/libvimradiation.so.* $LIB_PREFIX/ &&
$SUDO ln -svf $LIB_PREFIX/libvimradiation.so.* $LIB_PREFIX/libvimradiation.so
$SUDO chmod -v 755 $LIB_PREFIX/libvimradiation.so.*

VIM_DIR=$HOME/.vim/
PATHOGEN_DIR=$VIM_DIR/bundle
INSTALL_DIR=$VIM_DIR

if [[ -d $PATHOGEN_DIR ]] ; then
	echo "Pathogen install detected"
	INSTALL_DIR=$PATHOGEN_DIR/radiation/

	if [[ -d $INSTALL_DIR ]] ; then
		echo -n "Remove existing install (at $INSTALL_DIR)? [y/N] "
		read i
		if [[ $i == "y" ]] ; then
			rm -rfv $INSTALL_DIR
		fi
	fi
fi

mkdir -p $INSTALL_DIR

cd assets
	find . -type d -exec mkdir -v $INSTALL_DIR/{} \;
	find . -type f -not -name '.*' -not -name "vim.py" -exec cp -v {} $INSTALL_DIR/{} \;
cd ..
