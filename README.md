# cppCommon

useful c++ functions for file time log factory string and random.


# create static lib

git clone https://github.com/codeArtistDbs/cppCommon.git

cd cppCommon 
mkdir lib && mkdir build && cd build 
cmake ../src/Common/ && make -j
ln -s libCommon.a ../../lib/libCommon.a
