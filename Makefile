Compiler_pp=g++
Compiler_c=gcc


md5: md5_single.cpp
	$(Compiler_pp) md5_single.cpp -Ofast -fomit-frame-pointer -mtune=native -m64 -msse4.2 -funroll-loops -std=c++0x -Wall -Wextra -Wno-deprecated-register -Wno-unused-parameter -o run/md5.run
	$(Compiler_c) md5_opencl.c -Ofast -fomit-frame-pointer -mtune=native -m64 -msse4.2 -funroll-loops -std=c11 -framework OpenCL -Wall -Wextra -Wno-unused-parameter -o run/md5_cl.run
	$(Compiler_c) md5_opencl_py.c -Ofast -fomit-frame-pointer -mtune=native -m64 -msse4.2 -funroll-loops -std=c11 -framework OpenCL -Wall -Wextra -Wno-unused-parameter -o run/md5_cl_py.run

clean:
	rm -f run/md5.run

