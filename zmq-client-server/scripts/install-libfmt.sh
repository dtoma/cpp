git clone https://github.com/fmtlib/fmt.git
cd fmt/
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j2
sudo make install
