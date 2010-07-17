#!/bin/sh

# This is a simple script which extracts translation data from all source files of gtatools-gui and generates/updates *.ts files for them.
# This is done via a call to Qt's lupdate tool.
# This script will search for lupdate under various aliases, preferring lupdate-qt4. Alternatively, you can pass the name of the lupdate
# executable as first argument to this script.

GTATOOLSGUI_ROOT_DIR="`dirname "$0"`/src/gtatools-gui/src/"

if [ ! -d "$GTATOOLSGUI_ROOT_DIR" ];
then
	echo 'Error: Directory src/gtatools-gui/src does not exist!'
	exit 1
fi

if [ -n "$1" ];
then
	LUPDATE="$1"
else
	# Search for lupdate under various names. Prefer the tool with '-qt4' suffix to allow coexistance of lupdate for Qt3
	LUPDATE_ALIASES="lupdate-qt4 lupdate"
	
	for cmdname in $LUPDATE_ALIASES;
	do
		if which "$cmdname" > /dev/null;
		then
			LUPDATE="`which $cmdname`"
			break
		fi
	done

	if [ -z "$LUPDATE" ];
	then
		echo 'Error: lupdate not found!'
		echo "Searched in \$PATH for programs with the following names: $LUPDATE_ALIASES"
		exit 2
	fi
fi

echo "Using lupdate executable $LUPDATE"

SRCFILES="`find "$GTATOOLSGUI_ROOT_DIR" -iname '*.cpp' -or -iname '*.c' -or -iname '*.cxx' -or -iname '*.h' -or -iname '*.hpp' -or -iname '*.ui' | sed 's/ /\\ /' | tr '\n' ' '`"

$LUPDATE $SRCFILES -ts "$GTATOOLSGUI_ROOT_DIR/gtatools-gui_en.ts" "$GTATOOLSGUI_ROOT_DIR/gtatools-gui_de.ts"
