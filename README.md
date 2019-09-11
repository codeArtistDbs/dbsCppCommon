## cppCommon
```
useful c++ functions for file time log factory string random and CSemaphore.
```

## create static or shared lib
```
git clone https://github.com/codeArtistDbs/dbsCppCommon.git cppCommon

cd cppCommon 
mkdir lib 
mkdir build 
cd build 
cmake ../src/Common/ && make -j
ln -s $(pwd)/libCommon.a ../lib/libCommon.a
```

##获取已编译好的库ubuntu、tx2
```
git clone -b ubuntu https://github.com/codeArtistDbs/dbsCppCommon.git cppCommon
git clone -b tx2 https://github.com/codeArtistDbs/dbsCppCommon.git cppCommon
git clone -b nano https://github.com/codeArtistDbs/dbsCppCommon.git cppCommon
```