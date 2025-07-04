gcc -O3 -flto=auto -I ./zarchive/include/ zarchive_fuse.c zarchive_wrapper.cpp ./zarchive/src/sha_256.c ./zarchive/src/zarchivereader.cpp -lstdc++ -lfuse3 -lzstd -o fuse-zar
