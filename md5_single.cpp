#include <cstdio>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <cstdint>

#define num_hashes 10000000
#define len 20
#define len_postfix 7
#define OUT stdout

uint32_t m[16];
uint32_t len_tmp;
uint32_t a, b, c, d;

unsigned char input[600];
uint32_t result[4];

#define cs(a,b) (((a)<<(b)) | ((a)>>((32-b))));
#define bswap(a) (((a)>>24) | (((a)<<8)&0xff0000) | (((a)>>8)&0xff00) | ((a)<<24));

#define F(a, b, c) (c ^ (a & (b ^ c))) 
#define G(a, b, c) (((a ^ b) & c) ^ b)
#define H(a, b, c) (a ^ b ^ c)
#define I(a, b, c) (b ^ (a | (~c)))

#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

//max length: 55 bytes
int main(int argc, char *argv[]) {
	char *inp = argv[2];

	for(int i = 0; i < len; i++) {
		input[i] = inp[i];
	}
	input[len] = '-';
	input[len+1] = 'H';
	input[len+2] = 'L';
	input[len+3] = 'E';
	input[len+4] = 'T';
	input[len+5] = 'R';
	input[len+6] = 'D';

	uint64_t cnt = 0;
	time_t tstart = time(NULL);
	clock_t start;

	uint32_t Mr[4] = {0x80000000,};
	uint32_t mr[4] = {0x80000000,};
	char Ms[100] = {0,};
	char ms[100] = {0,};

	uint32_t popcntM = 64;
	uint32_t popcntm = 64;
	char pMs[100] = {0,};
	char pms[100] = {0,};
	uint32_t popcnt_tmp;
	uint64_t *p = (uint64_t*) result;

	uint32_t basem = 64;
	char bms[100] = {0,};

	uint32_t t_tmp;
	uint32_t tmax = 0x800;
	uint32_t tmin = 0x800;
	char tMs[100] = {0,};
	char tms[100] = {0,};
	unsigned char *t = (unsigned char*) result;

	len_tmp = (len + len_postfix) * 8;
	input[len + len_postfix] = 0x80;
	memcpy(input+56, &len_tmp, 4);

//	#pragma omp parallel
	while(1) {
		cnt++;
		start = clock();

		for (int l = 0; l < num_hashes; l++){
			memcpy(m, input, 64);
			
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

			result[0] = bswap(a+A);
			result[1] = bswap(b);
			result[2] = bswap(c+C);
			result[3] = bswap(d+D);

			if (Mr[0] <= result[0] && (Mr[0] < result[0] || (Mr[0] == result[0] && Mr[1] < result[1]) || (Mr[0] == result[0] && Mr[1] == result[1] && Mr[2] < result[2]) /*|| (Mr[0] == result[0] && Mr[1] == result[1] && Mr[2] == result[2] && Mr[3] < result[3])*/)) {
				memcpy(Ms, input, len);
				memcpy(Mr, result, 16);
				fprintf(OUT, "0||%08x%08x%08x%08x||%s-HLETRD||\n", Mr[0], Mr[1], Mr[2], Mr[3], Ms);
				fflush(OUT);
			} else if (mr[0] >= result[0] && (mr[0] > result[0] || (mr[0] == result[0] && mr[1] > result[1]) || (mr[0] == result[0] && mr[1] == result[1] && mr[2] > result[2]) /*|| (mr[0] == result[0] && mr[1] == result[1] && mr[2] == result[2] && mr[3] > result[3])*/)) {
				memcpy(ms, input, len);
				memcpy(mr, result, 16);
				fprintf(OUT, "1||%08x%08x%08x%08x||%s-HLETRD||\n", mr[0], mr[1], mr[2], mr[3], ms);
				fflush(OUT);
			}

			popcnt_tmp = __builtin_popcountll(p[0]) + __builtin_popcountll(p[1]);
			if (popcnt_tmp > popcntM) {
				memcpy(pMs, input, len);
				popcntM = popcnt_tmp;
				fprintf(OUT, "2||%08x%08x%08x%08x||%s-HLETRD||%d||\n", result[0], result[1], result[2], result[3], pMs, popcnt_tmp);
				fflush(OUT);
			} else if (popcnt_tmp < popcntm) {
				memcpy(pms, input, len);
				popcntm = popcnt_tmp;
				fprintf(OUT, "3||%08x%08x%08x%08x||%s-HLETRD||%d||\n", result[0], result[1], result[2], result[3], pms, popcnt_tmp);
				fflush(OUT);
			}

			popcnt_tmp = __builtin_popcountll(p[0] ^ 0xefcdab8967452301ll) + __builtin_popcountll(p[1] ^ 0x1032547698badcfell);
			if (popcnt_tmp < basem) {
				memcpy(bms, input, len);
				basem = popcnt_tmp;
				fprintf(OUT, "4||%08x%08x%08x%08x||%s-HLETRD||%u||\n", result[0], result[1], result[2], result[3], bms, basem);
				fflush(OUT);
			}

			t_tmp = (uint32_t)t[0] + t[1] + t[2] + t[3] + t[4] + t[5] + t[6] + t[7] + t[8] + t[9] + t[10] + t[11] + t[12] + t[13] + t[14] + t[15];
			if (t_tmp > tmax) {
				memcpy(tMs, input, len);
				tmax = t_tmp;
				fprintf(OUT, "5||%08x%08x%08x%08x||%s-HLETRD||%u||\n", result[0], result[1], result[2], result[3], tMs, tmax);
				fflush(OUT);
			} else if (t_tmp < tmin) {
				memcpy(tms, input, len);
				tmin = t_tmp;
				fprintf(OUT, "6||%08x%08x%08x%08x||%s-HLETRD||%u||\n", result[0], result[1], result[2], result[3], tms, tmin);
				fflush(OUT);
			}
			std::next_permutation(input, input+len);
		}

		fprintf(OUT, "9||%.0lf||%.2lf||%lld||\n", (double)num_hashes * 1.0 / (clock() - start) * CLOCKS_PER_SEC, (1.0 * len * num_hashes * 1.0 / (clock() - start) * CLOCKS_PER_SEC), cnt * num_hashes);
		fprintf(OUT, "t||%.0lf||\n", (num_hashes * 1.0 * cnt) / (time(NULL) - tstart));
		fflush(OUT);
	}
}