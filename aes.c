
#include "aes.h"







// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM - 
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

#define getSBoxValue(num) (sbox[(num)])


extern __m512i aes2(__m512i s,__m512i key)
{
	alignas(64) uint8_t state[4][4][4];
	_mm512_store_epi32(state[0][0],s);
	alignas(64) uint8_t a[4][4][4];

	const uint8_t U8_INV_FISRT_BIT_MASK[64] = {[0 ... 63] = 0xFE };//mask2
	const uint32_t COEFF[16] = {[0 ... 15] = 0x1b };//mul
	const uint8_t U8_FISRT_BIT_MASK[64] = {[0 ... 63] = 0x01 };//mask
	const uint32_t U32_FISRT_BYTE_MASK[16] = {[0 ... 15] = 0x000000FF };//mask3
	const uint32_t left1[16] = {0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08};//left
	const uint32_t right1[16] = {0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18};//right
	__m512i mask2 = _mm512_load_epi32 (U8_INV_FISRT_BIT_MASK);
	__m512i mul = _mm512_load_epi32 (COEFF);
	__m512i mask = _mm512_load_epi32 (U8_FISRT_BIT_MASK);
	__m512i mask3 = _mm512_load_epi32 (U32_FISRT_BYTE_MASK);
	__m512i left = _mm512_load_epi32 (left1);
	__m512i right = _mm512_load_epi32 (right1);


  	for (uint8_t t = 0; t < 4; ++t)
  		for (uint8_t i = 0; i < 4; ++i)
   	 		for (uint8_t j = 0; j < 4; ++j)
     				 a[t][i][j] = getSBoxValue(state[t][j][i]); // substitute and swap columbs and rows


	__m512i in1 = _mm512_load_epi32 (a[0][0]);
	__m512i mix_left = _mm512_sllv_epi32(in1,left);
	__m512i mix_right = _mm512_srlv_epi32(in1,right);
	__m512i rotated = _mm512_or_si512(mix_right,mix_left);
	_mm512_store_epi32(a[0][0],rotated);


  	for (uint8_t t = 0; t < 4; ++t)
		for(int x = 0; x < 4;x++)
			for(int y = 0; y < 4;y++)
				state[t][y][x] = a[t][x][y]; // unswap colimbs and rows


	__m512i in = _mm512_load_epi32 (state[0][0]);
	__m512i in_shiftR_8 = _mm512_srli_epi32(in,8);
	__m512i xor2 = _mm512_xor_si512(in_shiftR_8,in);
	__m512i in_shiftR_16 = _mm512_srli_epi32(in,16);
	__m512i xor1 = _mm512_xor_si512(in_shiftR_16,xor2);
	__m512i in_shiftR_24 = _mm512_srli_epi32(in,24);
	__m512i xor = _mm512_xor_si512(xor1,in_shiftR_24);

	__m512i xor_byte = _mm512_and_si512(xor,mask3); // make byte
	__m512i xor_byte_coeff = _mm512_mullo_epi32(xor_byte,mask); 

	__m512i in_shiftL_24 = _mm512_slli_epi32(in,24);
	__m512i in_rotL_8 = _mm512_or_si512(in_shiftR_8,in_shiftL_24);
	
	__m512i Tm  = _mm512_xor_si512(in_rotL_8,in);



	__m512i in_shiftR_7 = _mm512_srli_epi32(Tm,7);
	__m512i in_shiftR_7_BYTE = _mm512_and_si512(in_shiftR_7,mask);
	__m512i mul_1b = _mm512_mullo_epi32(in_shiftR_7_BYTE,mul);

	__m512i in_shiftL_1 = _mm512_slli_epi32(Tm,1);
	__m512i in_shiftL_1_BYTE = _mm512_and_si512(in_shiftL_1,mask2);

	__m512i xtime = _mm512_xor_si512(mul_1b,in_shiftL_1_BYTE);
	__m512i tmp = _mm512_xor_si512(xor_byte_coeff,xtime);
	__m512i matrix_multiply = _mm512_xor_si512(tmp,in);
	return _mm512_xor_si512(key,matrix_multiply);
}



extern void aes(_512_state* state,_512_key *k)
{
	__m512i key = _mm512_load_epi32 (k);
	alignas(64) uint8_t a[4][4][4];

	const uint8_t U8_INV_FISRT_BIT_MASK[64] = {[0 ... 63] = 0xFE };//mask2
	const uint32_t COEFF[16] = {[0 ... 15] = 0x1b };//mul
	const uint8_t U8_FISRT_BIT_MASK[64] = {[0 ... 63] = 0x01 };//mask
	const uint32_t U32_FISRT_BYTE_MASK[16] = {[0 ... 15] = 0x000000FF };//mask3
	const uint32_t left1[16] = {0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08,0x00,0x18,0x10,0x08};//left
	const uint32_t right1[16] = {0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18,0x00,0x08,0x10,0x18};//right
	__m512i mask2 = _mm512_load_epi32 (U8_INV_FISRT_BIT_MASK);
	__m512i mul = _mm512_load_epi32 (COEFF);
	__m512i mask = _mm512_load_epi32 (U8_FISRT_BIT_MASK);
	__m512i mask3 = _mm512_load_epi32 (U32_FISRT_BYTE_MASK);
	__m512i left = _mm512_load_epi32 (left1);
	__m512i right = _mm512_load_epi32 (right1);


  	for (uint8_t t = 0; t < 4; ++t)
  		for (uint8_t i = 0; i < 4; ++i)
   	 		for (uint8_t j = 0; j < 4; ++j)
     				 a[t][i][j] = getSBoxValue((*state)[t][j][i]); // substitute and swap columbs and rows


	__m512i in1 = _mm512_load_epi32 (a[0][0]);
	__m512i mix_left = _mm512_sllv_epi32(in1,left);
	__m512i mix_right = _mm512_srlv_epi32(in1,right);
	__m512i rotated = _mm512_or_si512(mix_right,mix_left);
	_mm512_store_epi32(a[0][0],rotated);


  	for (uint8_t t = 0; t < 4; ++t)
		for(int x = 0; x < 4;x++)
			for(int y = 0; y < 4;y++)
				(*state)[t][y][x] = a[t][x][y]; // unswap colimbs and rows


	__m512i in = _mm512_load_epi32 ((*state)[0][0]);
	__m512i in_shiftR_8 = _mm512_srli_epi32(in,8);
	__m512i xor2 = _mm512_xor_si512(in_shiftR_8,in);
	__m512i in_shiftR_16 = _mm512_srli_epi32(in,16);
	__m512i xor1 = _mm512_xor_si512(in_shiftR_16,xor2);
	__m512i in_shiftR_24 = _mm512_srli_epi32(in,24);
	__m512i xor = _mm512_xor_si512(xor1,in_shiftR_24);

	__m512i xor_byte = _mm512_and_si512(xor,mask3); // make byte
	__m512i xor_byte_coeff = _mm512_mullo_epi32(xor_byte,mask); 

	__m512i in_shiftL_24 = _mm512_slli_epi32(in,24);
	__m512i in_rotL_8 = _mm512_or_si512(in_shiftR_8,in_shiftL_24);
	
	__m512i Tm  = _mm512_xor_si512(in_rotL_8,in);



	__m512i in_shiftR_7 = _mm512_srli_epi32(Tm,7);
	__m512i in_shiftR_7_BYTE = _mm512_and_si512(in_shiftR_7,mask);
	__m512i mul_1b = _mm512_mullo_epi32(in_shiftR_7_BYTE,mul);

	__m512i in_shiftL_1 = _mm512_slli_epi32(Tm,1);
	__m512i in_shiftL_1_BYTE = _mm512_and_si512(in_shiftL_1,mask2);

	__m512i xtime = _mm512_xor_si512(mul_1b,in_shiftL_1_BYTE);
	__m512i tmp = _mm512_xor_si512(xor_byte_coeff,xtime);
	__m512i matrix_multiply = _mm512_xor_si512(tmp,in);
	__m512i ret = _mm512_xor_si512(key,matrix_multiply);
	_mm512_store_epi32((*state)[0][0],ret);
}


