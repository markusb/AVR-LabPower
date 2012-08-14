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
    git commit -a -m "Commiting from makefile for new branch $BRANCH"
    git branch $BUILD
fi
