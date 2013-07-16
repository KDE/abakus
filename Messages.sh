#! /usr/bin/env bash
# $EXTRACTRC `find . -name \*.rc -o -name \*.ui` >> rc.cpp || exit 11
$XGETTEXT `find . -name \*.qml -or -name \*.cpp -or -name \*.yy` -o $podir/abakus.pot
rm -f rc.cpp
