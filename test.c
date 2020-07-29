#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <wmmintrin.h>  //for intrinsics for AES-NI
#include <stdalign.h>
#include "aes.h"




static void phex(uint8_t* str)
{
    uint8_t len = 16;

    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}
int main(void)
{

	alignas(64) uint8_t a[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	alignas(64) uint8_t b[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	alignas(64) uint8_t key[64] = {0x6b, 0x2e, 0xe9, 0x73, 0xc1, 0x40, 0x3d, 0x93, 0xbe, 0x9f, 0x7e, 0x17, 0xe2, 0x96, 0x11, 0x2a};
	__m128i k = _mm_loadu_si128((__m128i *) key);
	__m128i test = _mm_loadu_si128((__m128i *) b);
        __m128i m =_mm_aesenc_si128    (test, k);
   	 _mm_storeu_si128((__m128i *) a,m);
	aes((_512_state*)b,(_512_key*)key);
	phex(b);
	phex(a);
	
    return 0;
}

