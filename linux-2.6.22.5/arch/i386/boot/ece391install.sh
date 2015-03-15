#!/bin/sh
#
# Created by Andrew Ofisher for ECE 391 - Fall 2007
#
# This script copies bzImage and vmlinux to 
# /workdir/source/linux-$(KERNEL-VERSION)/ for use by gdb and QEMU
#
# Arguments:
#      $1 - bzImage file
#      $2 - vmlinux file
#

verify () {
	if [ ! -f "$1" ]; then
		echo ""                                                   1>&2
		echo " *** Missing file: $1"                              1>&2
		echo ' *** You need to run "make" before "make install".' 1>&2
		echo ""                                                   1>&2
		exit 1
 	fi
}

# Make sure the files actually exist
verify "$1"
verify "$2"

cat $1 > /workdir/source/linux-2.6.22.5/bzImage
cat $2 > /workdir/source/linux-2.6.22.5/vmlinux
