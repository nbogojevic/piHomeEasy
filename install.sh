#!/bin/sh
############

# Compile
./build.sh

RETVAL=$?
if [ $RETVAL -eq 0 ]
then
	# Make executable
	cp piHomeEasy /usr/local/bin
  echo "Installed."
fi



