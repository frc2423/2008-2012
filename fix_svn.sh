#!/bin/bash
#
#	Author: Dustin Spicuzza
#
#	Recursively goes through an SVN repo and sets useful properties, and
#	fixes errors that Windows might cause...
#

function fixdir 
{
	cd $1

	for i in `ls`; do
	
		if [ -d "$i" ] && [ "$i" != "WPILib" ]; then
			fixdir $i
		fi
	done

	echo In $1
	
	for i in `ls *.cpp *.c *.h *.hpp 2> /dev/null`; do
		svn propdel svn:executable $i
		svn propset svn:keywords "Id Author Date Rev" $i
	done
	
	cd ..
}


fixdir .

