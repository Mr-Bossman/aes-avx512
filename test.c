#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <wmmintrin.h>  //for intrinsics for AES-NI
#include "aes.h"
#include "immintrin.h"
#include <stdalign.h>
#define MIX4(s0, s1, s2, s3) \
  tmp  = _mm_unpacklo_epi32(s0, s1); \
  s0 = _mm_unpackhi_epi32(s0, s1); \
  s1 = _mm_unpacklo_epi32(s2, s3); \
  s2 = _mm_unpackhi_epi32(s2, s3); \
  s3 = _mm_unpacklo_epi32(s0, s2); \
  s0 = _mm_unpackhi_epi32(s0, s2); \
  s2 = _mm_unpackhi_epi32(s1, tmp); \
  s1 = _mm_unpacklo_epi32(s1, tmp);




static void phex(uint8_t* str,uint8_t len)
{

    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%x, ", str[i]);
    printf("\n");
}

int main() {
	alignas(64) uint8_t a[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	alignas(64) uint8_t b[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	alignas(64) uint8_t c[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	alignas(64) uint8_t key[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
 __m512i s = _mm512_load_epi32 (b);
 __m512i i = _mm512_load_epi32 (key);

	__m128i k = _mm_loadu_si128((__m128i *) key);
	__m128i test = _mm_loadu_si128((__m128i *) b);
        __m128i m =_mm_aesenc_si128    (test, k);
   	 _mm_storeu_si128((__m128i *) a,m);
	aes((_512_state*)c,(_512_key*)key);
  s = aes2(s,i);
  _mm512_store_epi32(b,s);
  phex(b,64);
  phex(a,64);
  phex(c, 64);
	//haraka512(b,a);
//phex(b);
}

