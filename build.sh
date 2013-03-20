#!/bin/sh
############

# Compile
g++ piHomeEasy.cpp -o piHomeEasy -lwiringPi

RETVAL=$?
if [ $RETVAL -eq 0 ]
then
	# Make executable
	chmod 775 piHomeEasy
  exit 0
else
	echo "Build failed."
  exit 1
fi



