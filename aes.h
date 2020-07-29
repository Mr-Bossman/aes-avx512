#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h> // CBC mode, for memset
#include <stdalign.h>
#include <immintrin.h>

typedef uint8_t _512_key [4][16]  __attribute((aligned(64)));
typedef uint8_t _512_state [4][4][4] __attribute((aligned(64)));
extern void aes(_512_state* state, _512_key* key);
extern __m512i aes2(__m512i s,__m512i key);
#endif // _AES_H_
