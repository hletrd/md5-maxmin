#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif
#ifdef _WIN32
	#include <time.h>
#else
	#include <sys/time.h>
#endif

#define num_hashes 62*62*20
#define len_postfix 7
#define OUT stdout
#define num_global 8192
#define num_local 1

int len, len_tmp;
unsigned char *input;
unsigned int result[8];
int M, m;
int iter;

const char *OpenCL_kernel = "\n\
#define cs(a,b) (((a)<<(b)) | ((a)>>(32-(b))));\n\
#define bswap(a) (((a)>>24) | (((a)<<8)&0xff0000) | (((a)>>8)&0xff00) | ((a)<<24));\n\
\n\
#define F(a, b, c) (c ^ (a & (b ^ c))) \n\
#define G(a, b, c) (((a ^ b) & c) ^ b)\n\
#define H(a, b, c) (a ^ b ^ c)\n\
#define I(a, b, c) (b ^ (a | (~c)))\n\
\n\
#define A 0x67452301\n\
#define B 0xefcdab89\n\
#define C 0x98badcfe\n\
#define D 0x10325476\n\
\n\
#define basestr \"01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ\"\n\
\n\
__kernel void run(__global unsigned int *m_global, __global unsigned int *output, __global unsigned char *original, __global unsigned char *original_bit, __global int *bM, __global int *bm, __global int *tM, __global int *tm, __global int *base, int length) {\n\
	unsigned int a, b, c, d;\n\
	unsigned int result[4];\n\
	int poptmp;\n\
	\n\
	m_global += get_global_id(0)*16;\n\
	unsigned int m[16];\n\
	for(int i = 0; i < 16; i++) {\n\
		m[i] = m_global[i];\n\
	}\n\
	unsigned char *input = (unsigned char*) m;\n\
	output += get_global_id(0)*28;\n\
	original += get_global_id(0)*320;\n\
	original_bit += get_global_id(0)*128;\n\
	bM += get_global_id(0);\n\
	bm += get_global_id(0);\n\
	tM += get_global_id(0);\n\
	tm += get_global_id(0);\n\
	base += get_global_id(0);\n\
	unsigned char *result_b = (unsigned char*) result;\n\
	for(int i1 = 0; i1 < 62; ++i1) {\n\
		input[0] = basestr[i1];\n\
		for(int i2 = 0; i2 < 62; ++i2) {\n\
			input[1] = basestr[i2];\n\
			for(int i3 = 0; i3 < 20; ++i3) {\n\
				input[2] = basestr[i3];\n\
				a = B + cs((m[ 0] + 0xd76aa477),  7);\n\
				d = a + cs(((C ^ (a & 0x77777777)) + m[ 1] + 0xf8fa0bcc), 12);\n\
				c = d + cs((F(d, a, B) + m[ 2] + 0xbcdb4dd9), 17);\n\
				b = c + cs((F(c, d, a) + m[ 3] + 0xb18b7a77), 22);\n\
				a = b + cs((a + F(b, c, d) + m[ 4] + 0xf57c0faf),  7);\n\
				d = a + cs((d + F(a, b, c) + m[ 5] + 0x4787c62a), 12);\n\
				c = d + cs((c + F(d, a, b) + m[ 6] + 0xa8304613), 17);\n\
				b = c + cs((b + F(c, d, a) + 0xfd469501), 22);\n\
				a = b + cs((a + F(b, c, d) + 0x698098d8),  7);\n\
				d = a + cs((d + F(a, b, c) + 0x8b44f7af), 12);\n\
				c = d + cs((c + F(d, a, b) + 0xffff5bb1), 17);\n\
				b = c + cs((b + F(c, d, a) + 0x895cd7be), 22);\n\
				a = b + cs((a + F(b, c, d) + 0x6b901122),  7);\n\
				d = a + cs((d + F(a, b, c) + 0xfd987193), 12);\n\
				c = d + cs((c + F(d, a, b) + m[14] + 0xa679438e), 17);\n\
				b = c + cs((b + F(c, d, a) + m[15] + 0x49b40821), 22);\n\
				\n\
				a = b + cs((a + G(b, c, d) + m[ 1] + 0xf61e2562),  5);\n\
				d = a + cs((d + G(a, b, c) + m[ 6] + 0xc040b340),  9);\n\
				c = d + cs((c + G(d, a, b) + 0x265e5a51), 14);\n\
				b = c + cs((b + G(c, d, a) + m[ 0] + 0xe9b6c7aa), 20);\n\
				a = b + cs((a + G(b, c, d) + m[ 5] + 0xd62f105d),  5);\n\
				d = a + cs((d + G(a, b, c) + 0x02441453),  9);\n\
				c = d + cs((c + G(d, a, b) + m[15] + 0xd8a1e681), 14);\n\
				b = c + cs((b + G(c, d, a) + m[ 4] + 0xe7d3fbc8), 20);\n\
				a = b + cs((a + G(b, c, d) + 0x21e1cde6),  5);\n\
				d = a + cs((d + G(a, b, c) + m[14] + 0xc33707d6),  9);\n\
				c = d + cs((c + G(d, a, b) + m[ 3] + 0xf4d50d87), 14);\n\
				b = c + cs((b + G(c, d, a) + 0x455a14ed), 20);\n\
				a = b + cs((a + G(b, c, d) + 0xa9e3e905),  5);\n\
				d = a + cs((d + G(a, b, c) + m[ 2] + 0xfcefa3f8),  9);\n\
				c = d + cs((c + G(d, a, b) + 0x676f02d9), 14);\n\
				b = c + cs((b + G(c, d, a) + 0x8d2a4c8a), 20);\n\
				\n\
				a = b + cs((a + H(b, c, d) + m[ 5] + 0xfffa3942),  4);\n\
				d = a + cs((d + H(a, b, c) + 0x8771f681), 11);\n\
				c = d + cs((c + H(d, a, b) + 0x6d9d6122), 16);\n\
				b = c + cs((b + H(c, d, a) + m[14] + 0xfde5380c), 23);\n\
				a = b + cs((a + H(b, c, d) + m[ 1] + 0xa4beea44),  4);\n\
				d = a + cs((d + H(a, b, c) + m[ 4] + 0x4bdecfa9), 11);\n\
				c = d + cs((c + H(d, a, b) + 0xf6bb4b60), 16);\n\
				b = c + cs((b + H(c, d, a) + 0xbebfbc70), 23);\n\
				a = b + cs((a + H(b, c, d) + 0x289b7ec6),  4);\n\
				d = a + cs((d + H(a, b, c) + m[ 0] + 0xeaa127fa), 11);\n\
				c = d + cs((c + H(d, a, b) + m[ 3] + 0xd4ef3085), 16);\n\
				b = c + cs((b + H(c, d, a) + m[ 6] + 0x04881d05), 23);\n\
				a = b + cs((a + H(b, c, d) + 0xd9d4d039),  4);\n\
				d = a + cs((d + H(a, b, c) + 0xe6db99e5), 11);\n\
				c = d + cs((c + H(d, a, b) + m[15] + 0x1fa27cf8), 16);\n\
				b = c + cs((b + H(c, d, a) + m[ 2] + 0xc4ac5665), 23);\n\
				\n\
				a = b + cs((a + I(b, c, d) + m[ 0] + 0xf4292244),  6);\n\
				d = a + cs((d + I(a, b, c) + 0x432aff97), 10);\n\
				c = d + cs((c + I(d, a, b) + m[14] + 0xab9423a7), 15);\n\
				b = c + cs((b + I(c, d, a) + m[ 5] + 0xfc93a039), 21);\n\
				a = b + cs((a + I(b, c, d) + 0x655b59c3),  6);\n\
				d = a + cs((d + I(a, b, c) + m[ 3] + 0x8f0ccc92), 10);\n\
				c = d + cs((c + I(d, a, b) + 0xffeff47d), 15);\n\
				b = c + cs((b + I(c, d, a) + m[ 1] + 0x85845dd1), 21);\n\
				a = b + cs((a + I(b, c, d) + 0x6fa87e4f),  6);\n\
				d = a + cs((d + I(a, b, c) + m[15] + 0xfe2ce6e0), 10);\n\
				c = d + cs((c + I(d, a, b) + m[ 6] + 0xa3014314), 15);\n\
				b = c + cs((b + I(c, d, a) + 0x4e0811a1), 21);\n\
				a = b + cs((a + I(b, c, d) + m[ 4] + 0xf7537e82),  6);\n\
				d = a + cs((d + I(a, b, c) + 0xbd3af235), 10);\n\
				c = d + cs((c + I(d, a, b) + m[ 2] + 0x2ad7d2bb), 15);\n\
				b = B + c + cs((b + I(c, d, a) + 0xeb86d391), 21);\n\
				\n\
				result[0] = bswap(a+A);\n\
				result[1] = bswap(b);\n\
				result[2] = bswap(c+C);\n\
				result[3] = bswap(d+D);\n\
				if (output[0] <= result[0] && (output[0] < result[0] || (output[0] == result[0] && output[1] < result[1]) || (output[0] == result[0] && output[1] == result[1] && output[2] < result[2]) /*|| (output[0] == result[0] && output[1] == result[1] && output[2] == result[2] && output[3] < result[3])*/)) {\n\
					output[0] = result[0];\n\
					output[1] = result[1];\n\
					output[2] = result[2];\n\
					output[3] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original[j] = input[j];\n\
					}\n\
				} else if (output[4] >= result[0] && (output[4] > result[0] || (output[4] == result[0] && output[5] > result[1]) || (output[4] == result[0] && output[5] == result[1] && output[6] > result[2]) /*|| (output[4] == result[0] && output[5] == result[1] && output[6] == result[2] && output[7] > result[3])*/)) {\n\
					output[4] = result[0];\n\
					output[5] = result[1];\n\
					output[6] = result[2];\n\
					output[7] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original[j+64] = input[j];\n\
					}\n\
					\n\
				}\n\
				poptmp = popcount(result[0]) + popcount(result[1]) + popcount(result[2]) + popcount(result[3]);\n\
				if (poptmp > *bM) {\n\
					bM[0] = poptmp;\n\
					output[8] = result[0];\n\
					output[9] = result[1];\n\
					output[10] = result[2];\n\
					output[11] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original_bit[j] = input[j];\n\
					}\n\
				} else if (poptmp < *bm) { \n\
					bm[0] = poptmp;\n\
					output[12] = result[0];\n\
					output[13] = result[1];\n\
					output[14] = result[2];\n\
					output[15] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original_bit[j+64] = input[j];\n\
					}\n\
				}\n\
				poptmp = result_b[0] + result_b[1] + result_b[2] + result_b[3] + result_b[4] + result_b[5] + result_b[6] + result_b[7] + result_b[8] + result_b[9] + result_b[10] + result_b[11] + result_b[12] + result_b[13] + result_b[14] + result_b[15];\n\
				if (poptmp > *tM) {\n\
					tM[0] = poptmp;\n\
					output[16] = result[0];\n\
					output[17] = result[1];\n\
					output[18] = result[2];\n\
					output[19] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original[j+128] = input[j];\n\
					}\n\
				} else if (poptmp < *tm) { \n\
					tm[0] = poptmp;\n\
					output[20] = result[0];\n\
					output[21] = result[1];\n\
					output[22] = result[2];\n\
					output[23] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original[j+192] = input[j];\n\
					}\n\
				}\n\
				poptmp = popcount(result[0] ^ 0x01234567) + popcount(result[1] ^ 0x89abcdef) + popcount(result[2] ^ 0xfedcba98) + popcount(result[3] ^ 0x76543210);\n\
				if (poptmp < *base) {\n\
					base[0] = poptmp;\n\
					output[24] = result[0];\n\
					output[25] = result[1];\n\
					output[26] = result[2];\n\
					output[27] = result[3];\n\
					for(int j = 0; j < 32; j++) {\n\
						original[j+256] = input[j];\n\
					}\n\
				}\n\
			}\n\
		}\n\
	}\n\
	for (int i = 0; i < 16; i++) {\n\
		m_global[i] = m[i];\n\
	}\n\
}";

//max length supported: 27 bytes
int main(int argc, char *argv[]) {
	unsigned char *input = (unsigned char*)calloc(sizeof(char)*64*num_global*num_local, 1);
	unsigned char *original = (unsigned char*)malloc(sizeof(char)*320*num_global*num_local);
	unsigned char *original_bit = (unsigned char*)malloc(sizeof(char)*128*num_global*num_local);
	unsigned int *output = (unsigned int*)malloc(sizeof(int)*28*num_global*num_local);
	len = strlen(argv[1]) + 3;
	unsigned char *basestr = (unsigned char*)"01234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-=_+[]{};':\",./<>?|\\";
	int *bM = (int*)malloc(sizeof(int)*num_global*num_local);
	int *bm = (int*)malloc(sizeof(int)*num_global*num_local);
	int *tM = (int*)malloc(sizeof(int)*num_global*num_local);
	int *tm = (int*)malloc(sizeof(int)*num_global*num_local);
	int *base = (int*)malloc(sizeof(int)*num_global*num_local);
	int bM_all = 0, bm_all = 512;
	int tM_all = 0, tm_all = 4096;
	int base_all = 512;
	int strcnt[64];

	for(int i = 0; i < 8; i++) {
		result[i] = 0x7FFFFFFF;
	}

	for(int i = 0; i < 64; i++) {
		strcnt[i] = 0;
	}

	for(int j = 0; j < num_global*num_local; j++) {
		input[64*j+3] = basestr[j / 93 / 93];
		input[64*j+4] = basestr[(j / 93) % 93];
		input[64*j+5] = basestr[j % 93];
		for(int i = 0; i < len; i++) {
			input[6+i + 64*j] = argv[1][i];
		}
		input[len + 64*j] = '-';
		input[len+1 + 64*j] = 'H';
		input[len+2 + 64*j] = 'L';
		input[len+3 + 64*j] = 'E';
		input[len+4 + 64*j] = 'T';
		input[len+5 + 64*j] = 'R';
		input[len+6 + 64*j] = 'D';

		len_tmp = (len + len_postfix) * 8;
		input[len + len_postfix + 64*j] = 0x80;
		memcpy(input+56 + 64*j, &len_tmp, 4);
		for(int i = 0; i < 4; i++) {
			output[i + 28*j] = 0xFFFF0000;
		}
		for(int i = 4; i < 8; i++) {
			output[i + 28*j] = 0x0000FFFF;
		}
		bM[j] = 0;
		bm[j] = 512;
		tM[j] = 0;
		tm[j] = 4096;
		base[j] = 512;
	}

	cl_context context;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
	cl_mem input_m, output_m, original_m, original_m_bit, bM_m, bm_m, tM_m, tm_m, base_m;
	cl_uint nplatforms = 0, ndevices = 0;
	cl_platform_id *pid;
	cl_device_id *did;
	size_t global, local;
	int pid_in, did_in;
	int err;

	err = clGetPlatformIDs(0, NULL, &nplatforms);
	if (err == CL_SUCCESS) {
		fprintf(stderr, "Detected OpenCL platforms: %d\n", nplatforms);
	} else {
		fprintf(OUT, "Error: Failed to get Platform count: %d\n", err);
		return EXIT_FAILURE;
	}

	pid = (cl_platform_id*)malloc(sizeof(cl_platform_id)*nplatforms);
	err = clGetPlatformIDs(nplatforms, pid, NULL);
	if (err != CL_SUCCESS) {
		fprintf(OUT, "Error: Failed to get Platform IDs: %d\n", err);
		return EXIT_FAILURE;
	}

	char vendor[128], name[128], version[128];
	for (int i = 0; i < (int)nplatforms; i++) {
		err |= clGetPlatformInfo(pid[i], CL_PLATFORM_VENDOR, 128, vendor, NULL);
		err |= clGetPlatformInfo(pid[i], CL_PLATFORM_NAME, 128, name, NULL);
		err |= clGetPlatformInfo(pid[i], CL_PLATFORM_VERSION, 128, version, NULL);

		fprintf(stderr, "Platform %d: %s %s %s\n", i, vendor, name, version);
	}

	fprintf(stderr, "Select platform: ");
	fscanf(stdin, "%d", &pid_in);

	err = clGetDeviceIDs(pid[pid_in], CL_DEVICE_TYPE_ALL, 0, NULL, &ndevices);
	did = (cl_device_id*)malloc(sizeof(cl_device_id) * ndevices);
	err |= clGetDeviceIDs(pid[pid_in], CL_DEVICE_TYPE_ALL, ndevices, did, NULL);
	if (err != CL_SUCCESS) {
		fprintf(OUT, "Error: Failed to initialize OpenCL device group: %d\n", err);
		return EXIT_FAILURE;
	}
	char dname[128];

	for (int i = 0; i < (int)ndevices; i++) {
		err = clGetDeviceInfo(did[i], CL_DEVICE_NAME, 128, dname, NULL);
		if (err != CL_SUCCESS) {
			fprintf(OUT, "Error: Failed to get OpenCL device info: %d\n", err);
			return EXIT_FAILURE;
		} else {
			fprintf(stderr, "OpenCL device %d: %s\n", i, dname);
		}
	}

	fprintf(stderr, "Select device: ");
	fscanf(stdin, "%d", &did_in);

	context = clCreateContext(0, 1, &did[did_in], NULL, NULL, &err);
	if (!context) {
		fprintf(OUT, "Error: Failed to create a OpenCL compute context: %d\n", err);
		return EXIT_FAILURE;
	}
	commands = clCreateCommandQueue(context, did[did_in], 0, &err);
	if (!commands) {
		fprintf(OUT, "Error: Failed to create a OpenCL commands: %d\n", err);
		return EXIT_FAILURE;
	}
	program = clCreateProgramWithSource(context, 1, (const char **)&OpenCL_kernel, NULL, &err);
	if (!program) {
		fprintf(OUT, "Error: Failed to create OpenCL program: %d\n", err);
		return EXIT_FAILURE;
	}
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
		size_t lenn;
		char buffer[2048];

		fprintf(OUT, "Error: Failed to build program executable: %d\n", err);
		clGetProgramBuildInfo(program, did[did_in], CL_PROGRAM_BUILD_LOG, 2048, buffer, &lenn);
		fprintf(OUT, "%s\n", buffer);
		return EXIT_FAILURE;
	}

	kernel = clCreateKernel(program, "run", &err);
	if (!kernel || err != CL_SUCCESS) {
		fprintf(OUT, "Error: Failed to create OpenCL kernel: %d\n", err);
		return EXIT_FAILURE;
	}

	input_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char)*64*num_global*num_local, NULL, NULL);
	output_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*28*num_global*num_local, NULL, NULL);
	original_m = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(char)*320*num_global*num_local, NULL, NULL);
	original_m_bit = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(char)*128*num_global*num_local, NULL, NULL);
	bM_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_global*num_local, NULL, NULL);
	bm_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_global*num_local, NULL, NULL);
	tM_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_global*num_local, NULL, NULL);
	tm_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_global*num_local, NULL, NULL);
	base_m = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)*num_global*num_local, NULL, NULL);

	if (!input_m || !output_m || !original_m || !original_m_bit || !bM_m || !bm_m || !tM_m || !tm_m || !base_m) {
		fprintf(OUT, "Error: Failed to allocate OpenCL device memory: %d\n", err);
		return EXIT_FAILURE;
	}

	global = (size_t)num_global;
	local = (size_t)num_local;

	#ifdef _WIN32
	time_t start;
	time(&start);
	#else
	struct timeval t0, t1;
	gettimeofday(&t0, 0);
	#endif
	while(1) {
		err = clEnqueueWriteBuffer(commands, input_m, CL_TRUE, 0, sizeof(char)*64*num_global*num_local, input, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, output_m, CL_TRUE, 0, sizeof(int)*28*num_global*num_local, output, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, bM_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, bM, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, bm_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, bm, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, tM_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, tM, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, tm_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, tm, 0, NULL, NULL);
		err = clEnqueueWriteBuffer(commands, base_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, base, 0, NULL, NULL);
		if (err != CL_SUCCESS) {
			fprintf(OUT, "Error: Failed to write to OpenCL source array: %d\n", err);
			return EXIT_FAILURE;
		}
		err = 0;
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_m);
		err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_m);
		err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &original_m);
		err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &original_m_bit);
		err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &bM_m);
		err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &bm_m);
		err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &tM_m);
		err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &tm_m);
		err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &base_m);
		err |= clSetKernelArg(kernel, 9, sizeof(int), &len);
		if (err != CL_SUCCESS) {
			fprintf(OUT, "Error: Failed to set OpenCL kernel arguments: %d\n", err);
			return EXIT_FAILURE;
		}
		err = clGetKernelWorkGroupInfo(kernel, did[did_in], CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
		if (err != CL_SUCCESS) {
			fprintf(OUT, "Error: Failed to retrieve OpenCL kernel work group info: %d\n", err);
			return EXIT_FAILURE;
		}
		err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
		if (err) {
			fprintf(OUT, "Error: Failed to execute OpenCL kernel: %d\n", err);
			return EXIT_FAILURE;
		}
		clFinish(commands);
		err |= clEnqueueReadBuffer(commands, input_m, CL_TRUE, 0, sizeof(char)*64*num_global*num_local, input, 0, NULL, NULL);
		err = clEnqueueReadBuffer(commands, output_m, CL_TRUE, 0, sizeof(int)*28*num_global*num_local, output, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, original_m, CL_TRUE, 0, sizeof(char)*320*num_global*num_local, original, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, original_m_bit, CL_TRUE, 0, sizeof(char)*128*num_global*num_local, original_bit, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, bM_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, bM, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, bm_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, bm, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, tM_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, tM, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, tm_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, tm, 0, NULL, NULL);
		err |= clEnqueueReadBuffer(commands, base_m, CL_TRUE, 0, sizeof(int)*num_global*num_local, base, 0, NULL, NULL);
		if (err != CL_SUCCESS) {
			fprintf(OUT, "Error: Failed to read OpenCL output. %d\n", err);
			exit(1);
		}
		fflush(OUT);
		strcnt[9]++;
		for (int i = 9; i < len; i++) {
			if (strcnt[i] > 92) {	
				strcnt[i] = 0;
				strcnt[i+1]++;
			} else {
				break;
			}
		}
		for(int i = 0; i < num_global * num_local; i++) {
			for(int j = 9; j < len; j++) {
				input[i*64+j] = basestr[strcnt[j]];
			}
		}
		for(int j = 0; j < num_global * num_local; j++) {
			if (output[j*28 + 0] > result[0] || (output[j*28 + 0] == result[0] && output[j*28 + 1] > result[1]) || (output[j*28 + 0] == result[0] && output[j*28 + 1] == result[1] && output[j*28 + 2] > result[2])) {
				result[0] = output[j*28 + 0];
				result[1] = output[j*28 + 1];
				result[2] = output[j*28 + 2];
				result[3] = output[j*28 + 3];
				original[len+7+j*320] = 0;
				fprintf(OUT, "MAX-string: %s\n", original + 320*j);
				fprintf(OUT, "MAX: %08x %08x %08x %08x\n", result[0], result[1], result[2], result[3]);
			} else if (output[j*28 + 4] < result[4] || (output[j*28 + 4] == result[4] && output[j*28 + 5] < result[5]) || (output[j*28 + 4] == result[4] && output[j*28 + 5] == result[5] && output[j*28 + 6] < result[6])) {
				result[4] = output[j*28 + 4];
				result[5] = output[j*28 + 5];
				result[6] = output[j*28 + 6];
				result[7] = output[j*28 + 7];
				original[len+7+64+j*320] = 0;
				fprintf(OUT, "min-string: %s\n", original + 320*j + 64);
				fprintf(OUT, "min: %08x %0x %08x %08x\n", result[4], result[5], result[6], result[7]);
			}
			if (bM[j] > bM_all) {
				bM_all = bM[j];
				original_bit[len+7+j*128] = 0;
				fprintf(OUT, "popcnt MAX-string: %s (%d)\n", original_bit + 128*j, bM_all);
				fprintf(OUT, "popcnt MAX: %08x %08x %08x %08x\n", output[j*28 + 8], output[j*28 + 9], output[j*28 + 10], output[j*28 + 11]);
			} else if (bm[j] < bm_all) {
				bm_all = bm[j];
				original_bit[len+7+j*128+64] = 0;
				fprintf(OUT, "popcnt min-string: %s (%d)\n", original_bit + 128*j + 64, bm_all);
				fprintf(OUT, "popcnt min: %08x %08x %08x %08x\n", output[j*16 + 12], output[j*28 + 13], output[j*28 + 14], output[j*28 + 15]);
			}
			if (tM[j] > tM_all) {
				tM_all = tM[j];
				original[len+7+128+j*320] = 0;
				fprintf(OUT, "bytesum MAX-string: %s (%d)\n", original + 128 + 320*j, tM_all);
				fprintf(OUT, "bytesum MAX: %08x %08x %08x %08x\n", output[j*28 + 16], output[j*28 + 17], output[j*28 + 18], output[j*28 + 19]);
			} else if (tm[j] < tm_all) {
				tm_all = tm[j];
				original[len+7+192+j*320] = 0;
				fprintf(OUT, "bytesum min-string: %s (%d)\n", original + 192 + 320*j, tm_all);
				fprintf(OUT, "bytesum min: %08x %08x %08x %08x\n", output[j*28 + 20], output[j*28 + 21], output[j*28 + 22], output[j*28 + 23]);
			}
			if (base[j] < base_all) {
				base_all = base[j];
				original[len+7+256+j*320] = 0;
				fprintf(OUT, "base-string: %s (%d)\n", original + 256 + 320*j, base_all);
				fprintf(OUT, "base: %08x %08x %08x %08x\n", output[j*28 + 24], output[j*28 + 25], output[j*28 + 26], output[j*28 + 27]);
			}
		}
		#ifdef _WIN32
		iter++;
		time_t end;
		time(&end);
		fprintf(stderr, "%.1lf secs elapsed, processed %lld hashes, %.0lf hashes / sec\n", difftime(end, start), (long long int)iter*num_global*num_hashes, (double)iter*num_global*num_hashes/(double)difftime(end, start));
		#else
		iter++;
		gettimeofday(&t1, 0);
		fprintf(stderr, "%.1lf secs elapsed, processed %lld hashes, %.0lf hashes / sec\n", (((double)(t1.tv_sec - t0.tv_sec)*1000000 + (t1.tv_usec - t0.tv_usec))/1000000), (long long int)iter*num_global*num_hashes, (double)iter*num_global*num_hashes/(((double)(t1.tv_sec - t0.tv_sec)*1000000 + (t1.tv_usec - t0.tv_usec))/1000000));
		#endif
		fflush(OUT);
	}

	clReleaseMemObject(input_m);
	clReleaseMemObject(output_m);
	clReleaseMemObject(original_m);
	clReleaseMemObject(original_m_bit);
	clReleaseMemObject(bM_m);
	clReleaseMemObject(bm_m);
	clReleaseMemObject(tM_m);
	clReleaseMemObject(tm_m);
	clReleaseMemObject(base_m);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
}