#! /bin/bash

cd /usr/local/ev_sdk/bin

./test -f 1 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg
./test -f 2 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg
./test -f 3 -i ../data/test_sub.jpg -o ../data/test_sub_out.jpg
./test -f 4 -i ../data/test_sub.mp4 -o ../data/test_sub_out.mp4

echo "finished."
