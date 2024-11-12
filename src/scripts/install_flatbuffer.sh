#!/bin/bash
TEMP_DIR="../temp"

# check if temp directory exists
if [ ! -d "$TEMP_DIR" ]; then
    echo "$TEMP_DIR doesn't exist. Creating."
fi

# clone the repository
cd $TEMP_DIR
if [ ! -d "flatbuffers" ]; then
    echo "Cloning Arrow."
    git clone https://github.com/google/flatbuffers.git
fi

cd flatbuffers
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make