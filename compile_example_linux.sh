git clone https://github.com/ReneNyffenegger/cpp-base64
git clone https://github.com/juanmv94/JJSON
g++ -c -O3 cpp-base64/base64.cpp
g++ -c -O3 JJSON/c++/JJSON.cpp
g++ -I. -c -O3 src/miYoigo.cpp
g++ -I. -c -O3 src/example.cpp
g++ base64.o JJSON.o miYoigo.o example.o -lcurl -o example.out
