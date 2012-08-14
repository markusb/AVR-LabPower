#/bin/bash
#
# Generate a date based build code and commit with this code to git when it changes
#

BUILD=$(date +%y%m%d)
echo "Build: $BUILD"
