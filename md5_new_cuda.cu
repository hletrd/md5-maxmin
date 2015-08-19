#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

#define num_hashes 10000000
#define len 20
#define len_bit 160
#define OUT stdout

#define num_thread 64
#define num_block 384

unsigned char input[10000000];
unsigned int result[400000];

#define cs(a,b) ((a<<b) | (a>>(32-b)));
#define bswap(a) ((a>>24) | ((a<<8)&0xff0000) | ((a>>8)&0xff00) | (a<<24));

#define F(a, b, c) (c ^ (a & (b ^ c))) 
#define G(a, b, c) (((a ^ b) & c) ^ b)
#define H(a, b, c) (a ^ b ^ c)
#define I(a, b, c) (b ^ (a | (~c)))

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

__global__
void md5_gpu(unsigned char *input_gpu, unsigned int *result_gpu) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int m[16] = {0,};
	input_gpu[idx * 64 + len] = 0x80;
	input_gpu[idx * 64 + 56] = len_bit;
	memcpy(m, input_gpu, 64);
	
	a = B + cs((A + F(B, C, D) + m[ 0] + 0xd76aa478),  7);
	d = a + cs((D + F(a, B, C) + m[ 1] + 0xe8c7b756), 12);
	c = d + cs((C + F(d, a, B) + m[ 2] + 0x242070db), 17);
	b = c + cs((B + F(c, d, a) + m[ 3] + 0xc1bdceee), 22);
	a = b + cs((a + F(b, c, d) + m[ 4] + 0xf57c0faf),  7);
	d = a + cs((d + F(a, b, c) + m[ 5] + 0x4787c62a), 12);
	c = d + cs((c + F(d, a, b) + m[ 6] + 0xa8304613), 17);
	b = c + cs((b + F(c, d, a) /*+ m[ 7]*/ + 0xfd469501), 22);
	a = b + cs((a + F(b, c, d) /*+ m[ 8]*/ + 0x698098d8),  7);
	d = a + cs((d + F(a, b, c) /*+ m[ 9]*/ + 0x8b44f7af), 12);
	c = d + cs((c + F(d, a, b) /*+ m[10]*/ + 0xffff5bb1), 17);
	b = c + cs((b + F(c, d, a) /*+ m[11]*/ + 0x895cd7be), 22);
	a = b + cs((a + F(b, c, d) /*+ m[12]*/ + 0x6b901122),  7);
	d = a + cs((d + F(a, b, c) /*+ m[13]*/ + 0xfd987193), 12);
	c = d + cs((c + F(d, a, b) + m[14] + 0xa679438e), 17);
	b = c + cs((b + F(c, d, a) + m[15] + 0x49b40821), 22);
	
	a = b + cs((a + G(b, c, d) + m[ 1] + 0xf61e2562),  5);
	d = a + cs((d + G(a, b, c) + m[ 6] + 0xc040b340),  9);
	c = d + cs((c + G(d, a, b) /*+ m[11]*/ + 0x265e5a51), 14);
	b = c + cs((b + G(c, d, a) + m[ 0] + 0xe9b6c7aa), 20);
	a = b + cs((a + G(b, c, d) + m[ 5] + 0xd62f105d),  5);
	d = a + cs((d + G(a, b, c) /*+ m[10]*/ + 0x02441453),  9);
	c = d + cs((c + G(d, a, b) + m[15] + 0xd8a1e681), 14);
	b = c + cs((b + G(c, d, a) + m[ 4] + 0xe7d3fbc8), 20);
	a = b + cs((a + G(b, c, d) /*+ m[ 9]*/ + 0x21e1cde6),  5);
	d = a + cs((d + G(a, b, c) + m[14] + 0xc33707d6),  9);
	c = d + cs((c + G(d, a, b) + m[ 3] + 0xf4d50d87), 14);
	b = c + cs((b + G(c, d, a) /*+ m[ 8]*/ + 0x455a14ed), 20);
	a = b + cs((a + G(b, c, d) /*+ m[13]*/ + 0xa9e3e905),  5);
	d = a + cs((d + G(a, b, c) + m[ 2] + 0xfcefa3f8),  9);
	c = d + cs((c + G(d, a, b) /*+ m[ 7]*/ + 0x676f02d9), 14);
	b = c + cs((b + G(c, d, a) /*+ m[12]*/ + 0x8d2a4c8a), 20);
	
	a = b + cs((a + H(b, c, d) + m[ 5] + 0xfffa3942),  4);
	d = a + cs((d + H(a, b, c) /*+ m[ 8]*/ + 0x8771f681), 11);
	c = d + cs((c + H(d, a, b) /*+ m[11]*/ + 0x6d9d6122), 16);
	b = c + cs((b + H(c, d, a) + m[14] + 0xfde5380c), 23);
	a = b + cs((a + H(b, c, d) + m[ 1] + 0xa4beea44),  4);
	d = a + cs((d + H(a, b, c) + m[ 4] + 0x4bdecfa9), 11);
	c = d + cs((c + H(d, a, b) /*+ m[ 7]*/ + 0xf6bb4b60), 16);
	b = c + cs((b + H(c, d, a) /*+ m[10]*/ + 0xbebfbc70), 23);
	a = b + cs((a + H(b, c, d) /*+ m[13]*/ + 0x289b7ec6),  4);
	d = a + cs((d + H(a, b, c) + m[ 0] + 0xeaa127fa), 11);
	c = d + cs((c + H(d, a, b) + m[ 3] + 0xd4ef3085), 16);
	b = c + cs((b + H(c, d, a) + m[ 6] + 0x04881d05), 23);
	a = b + cs((a + H(b, c, d) /*+ m[ 9]*/ + 0xd9d4d039),  4);
	d = a + cs((d + H(a, b, c) /*+ m[12]*/ + 0xe6db99e5), 11);
	c = d + cs((c + H(d, a, b) + m[15] + 0x1fa27cf8), 16);
	b = c + cs((b + H(c, d, a) + m[ 2] + 0xc4ac5665), 23);
	
	a = b + cs((a + I(b, c, d) + m[ 0] + 0xf4292244),  6);
	d = a + cs((d + I(a, b, c) /*+ m[ 7]*/ + 0x432aff97), 10);
	c = d + cs((c + I(d, a, b) + m[14] + 0xab9423a7), 15);
	b = c + cs((b + I(c, d, a) + m[ 5] + 0xfc93a039), 21);
	a = b + cs((a + I(b, c, d) /*+ m[12]*/ + 0x655b59c3),  6);
	d = a + cs((d + I(a, b, c) + m[ 3] + 0x8f0ccc92), 10);
	c = d + cs((c + I(d, a, b) /*+ m[10]*/ + 0xffeff47d), 15);
	b = c + cs((b + I(c, d, a) + m[ 1] + 0x85845dd1), 21);
	a = b + cs((a + I(b, c, d) /*+ m[ 8]*/ + 0x6fa87e4f),  6);
	d = a + cs((d + I(a, b, c) + m[15] + 0xfe2ce6e0), 10);
	c = d + cs((c + I(d, a, b) + m[ 6] + 0xa3014314), 15);
	b = c + cs((b + I(c, d, a) /*+ m[13]*/ + 0x4e0811a1), 21);
	a = b + cs((a + I(b, c, d) + m[ 4] + 0xf7537e82),  6);
	d = a + cs((d + I(a, b, c) /*+ m[11]*/ + 0xbd3af235), 10);
	c = d + cs((c + I(d, a, b) + m[ 2] + 0x2ad7d2bb), 15);
	b = B + c + cs((b + I(c, d, a) /*+ m[ 9]*/ + 0xeb86d391), 21);
	result_gpu[4*idx+0] = bswap(a+A);
	result_gpu[4*idx+1] = bswap(b);
	result_gpu[4*idx+2] = bswap(c+C);
	result_gpu[4*idx+3] = bswap(d+D);
	result_gpu[0] = 4324234;
}

//max length: 55 bytes

int main(int argc, char *argv[]) {
	int workernum = atoi(argv[1]);
	char *inp = argv[2];

	char *basestr = inp; //"!\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	for (int i = 0; i < num_block; i++) {
		for (int j = 0; j < num_thread; j++) {
			input[(i*num_thread+j)*64] = basestr[i];
			input[(i*num_thread+j)*64+1] = basestr[j / 64];
			input[(i*num_thread+j)*64+2] = basestr[j % 64];
			for(int k = 3; k < len; k++) {
				input[(i*num_thread+j)*64+k] = inp[k];
			}
		}
	}

	long long int cnt = 0;
	time_t tstart = time(NULL);
	clock_t start;

	unsigned int Mr[4] = {0,};
	unsigned int mr[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	char Ms[100] = {0,};
	char ms[100] = {0,};

	int popcntM = 0;
	int popcntm = 128;
	char pMs[100] = {0,};
	char pms[100] = {0,};
	int popcnt_tmp;

	int base_tmp;
	unsigned int basem = 128;
	char bms[100] = {0,};

	unsigned int t_tmp;
	unsigned int tmax = 0;
	unsigned int tmin = 0x1000;
	char tMs[100] = {0,};
	char tms[100] = {0,};

	int SIZE_RESULT_GPU = 4 * num_block * num_thread;
	int SIZE_INPUT_GPU = 64 * num_block * num_thread;

	unsigned int *_result_gpu;
	unsigned char *_input_gpu;

	//while(1) {
		cnt++;
		start = clock();

		cudaMalloc((void**)&_result_gpu, SIZE_RESULT_GPU);
		cudaError_t err;
		if ((err = cudaGetLastError()) != cudaSuccess) {
			printf("%s\n", cudaGetErrorString(err));
			return 0;
		}
		cudaMalloc((void**)&_input_gpu, SIZE_INPUT_GPU);
		cudaMemcpy(_input_gpu, input, SIZE_INPUT_GPU, cudaMemcpyHostToDevice);
		md5_gpu<<<num_block, num_thread>>>(_input_gpu, _result_gpu);
		cudaMemcpy(result, _result_gpu, SIZE_RESULT_GPU, cudaMemcpyDeviceToHost);
		cudaMemcpy(input, _input_gpu, SIZE_INPUT_GPU, cudaMemcpyDeviceToHost);
		cudaFree(_result_gpu);
		cudaFree(_input_gpu);
		input[64] = 0;
		fprintf(OUT, "%d%d%d%d (%s)", result[0], result[1], result[2], result[3], input);
		//fprintf(OUT, "9||%.0lf||%.2lf||%lld||\n", num_hashes * 11.0 / (clock() - start) * CLOCKS_PER_SEC, (1.0 * len * num_hashes * 1.0 / (clock() - start) * CLOCKS_PER_SEC), cnt * num_hashes);
		fflush(OUT);
	//}
}