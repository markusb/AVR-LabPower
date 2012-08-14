#/bin/bash
#
# Generate a date based build code and commit with this code to git when it changes
#

BUILD=$(date +%y%m%d-%H)

if git branch | grep -q "$BUILD"
then
    echo "Build: $BUILD already in git"
else
    echo "Build: $BUILD added to git"
    echo >  build.c '#include <avr/pgmspace.h>'
    echo >> build.c "char * build = \"$BUILD\";"
    avr-gcc -g -Os -mmcu=atxmega16a4 -D F_CPU=32000000 -c -o branch.o branch.c
    git commit -a -m "Commiting from makefile for new branch $BRANCH"
    git branch $BUILD
fi
