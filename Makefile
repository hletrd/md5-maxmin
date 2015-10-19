Compiler=g++


md5: md5_single.cpp
	$(Compiler) md5_single.cpp -Ofast -fomit-frame-pointer -mtune=native -m64 -msse4.2 -funroll-loops -std=c++0x -o run/md5.run

clean:
	rm -f run/md5.run

