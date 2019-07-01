git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=`pwd`/depot_tools:"$PATH"

mkdir breakpad
cd breakpad/
fetch breakpad
cd src/
./configure
make -j2
sudo make install
