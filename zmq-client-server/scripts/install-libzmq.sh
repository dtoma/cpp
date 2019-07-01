git clone https://github.com/zeromq/libzmq
cd libzmq/
mkdir cmake-build
cd cmake-build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j2
sudo make install
