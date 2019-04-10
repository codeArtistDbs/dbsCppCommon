# cppCommon

useful c++ functions for file time log factory string and random.


# create static lib

git clone https://github.com/codeArtistDbs/dbsCppCommon.git

cd dbsCppCommon 
mkdir lib && mkdir build && cd build 
cmake ../src/Common/ && make -j
ln -s $(pwd)/libCommon.a ../lib/libCommon.a
