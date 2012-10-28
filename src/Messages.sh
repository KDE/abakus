#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.rc -o -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.cpp` -o $podir/abakus.pot
rm -f rc.cpp
