# Tutorial to compile Loque on Mac

1. Open terminal, then type the following:

``` sh
git clone https://github.com/University-of-Utah-CS3505/edu-app-unescaped-characters.git
git clone https://github.com/fallahn/tmxlite.git

cd tmxlite/tmxlite
cmake ./
make

sudo cp -r include/tmxlite /usr/local/include
sudo cp libtmxlite.dylib /usr/local/lib

brew install sfml

cd ..
cd edu-app-unescaped-characters
# Here you may or may not have to manually specify where tmxlite is located within cmake.
# The `sudo cp ...` commands above show where all the necessary files are.
cmake ./
make
./loque
```
Done!
