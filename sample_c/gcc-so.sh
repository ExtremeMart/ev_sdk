g++ -std=c++11 -c -fpic -I. ji.cpp
g++ -shared -o libji.so ji.o
