// Author: Jake Longo
// Description: Code based on the Intel AES-NI reference doc
//
#include <stdint.h>
#include <wmmintrin.h>

#ifndef _WIN32
#include <x86intrin.h>
#endif

#include "intel_impl.h"


inline __m128i AES_128_ASSIST (__m128i temp1, __m128i temp2) {
  __m128i temp3;

  temp2 = _mm_shuffle_epi32 (temp2 ,0xff);
  temp3 = _mm_slli_si128 (temp1, 0x4);
  temp1 = _mm_xor_si128 (temp1, temp3);
  temp3 = _mm_slli_si128 (temp3, 0x4);
  temp1 = _mm_xor_si128 (temp1, temp3);
  temp3 = _mm_slli_si128 (temp3, 0x4);
  temp1 = _mm_xor_si128 (temp1, temp3);
  temp1 = _mm_xor_si128 (temp1, temp2);

return temp1;
}

void AES_128_Key_Expansion (uint8_t *userkey, uint8_t *key) {
  __m128i temp1, temp2;
  __m128i *Key_Schedule = (__m128i*)key;

  temp1 = _mm_loadu_si128((__m128i*)userkey);
  Key_Schedule[0] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1 ,0x1);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[1] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x2);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[2] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x4);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[3] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x8);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[4] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x10);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[5] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x20);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[6] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x40);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[7] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x80);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[8] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x1b);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[9] = temp1;
  temp2 = _mm_aeskeygenassist_si128 (temp1,0x36);
  temp1 = AES_128_ASSIST(temp1, temp2);
  Key_Schedule[10] = temp1;
}

void AES_ECB_encrypt(uint8_t *in, uint8_t *out, uint8_t *key) {

  __m128i tmp;
  int j;

  tmp = _mm_loadu_si128((__m128i*)in);
  tmp = _mm_xor_si128(tmp,((__m128i*)key)[0]);

  for(j=1; j < 10; j++) {
    tmp = _mm_aesenc_si128(tmp,((__m128i*)key)[j]);
  }

  tmp = _mm_aesenclast_si128(tmp,((__m128i*)key)[j]);
  _mm_storeu_si128((__m128i*)out,tmp);

  }

void c_intel_benchmark(uint8_t* keys, uint8_t*data, uint8_t *dataOut) {

  // create a store for the keys
  uint8_t expandedKeys[176];

  // Expand the key and encrypt
  AES_128_Key_Expansion(keys, expandedKeys);
  AES_ECB_encrypt(data, dataOut, expandedKeys);

}

void c_intel_interleaved_4x(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {
  __m128i mask        = _mm_set_epi32(0x0c0f0e0d,0x0c0f0e0d,0x0c0f0e0d,0x0c0f0e0d);

  __m128i dataIn      = _mm_loadu_si128((__m128i*)&data[0]);
  __m128i mmrconInit  = _mm_set_epi32(1,1,1,1);
  __m128i mmrconFinal = _mm_set_epi32(0x1b,0x1b,0x1b,0x1b);
  __m128i mmrcon;

  __m128i mmrk;
  __m128i scratch0;
  __m128i scratch1;

  __m128i state;

  for (uint8_t keyIdx = 0; keyIdx < 64; keyIdx += 16) {

    mmrk    = _mm_loadu_si128((__m128i*)&keys[keyIdx]);
    state   = _mm_xor_si128(dataIn, mmrk);
    mmrcon  = mmrconInit;

    for (uint8_t rouncCounter = 0; rouncCounter <= 7; ++rouncCounter) {
      scratch1   = _mm_shuffle_epi8(mmrk, mask);
      scratch1   = _mm_aesenclast_si128 (scratch1,mmrcon);
      mmrcon     = _mm_slli_epi32(mmrcon,1);
      scratch0   = _mm_slli_si128 (mmrk, 0x4);
      mmrk       = _mm_xor_si128 (mmrk, scratch0);
      scratch0   = _mm_slli_si128 (scratch0, 0x4);
      mmrk       = _mm_xor_si128 (mmrk, scratch0);
      scratch0   = _mm_slli_si128 (scratch0, 0x4);
      mmrk       = _mm_xor_si128 (mmrk, scratch0);
      mmrk       = _mm_xor_si128 (mmrk, scratch1);
      state      = _mm_aesenc_si128 (state, mmrk);
    }

    mmrcon     = mmrconFinal;
    scratch1   = _mm_shuffle_epi8(mmrk, mask);
    scratch1   = _mm_aesenclast_si128(scratch1,mmrcon);
    mmrcon     = _mm_slli_epi32(mmrcon,1);
    scratch0   = _mm_slli_si128(mmrk, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    scratch0   = _mm_slli_si128(scratch0, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    scratch0   = _mm_slli_si128(scratch0, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    mmrk       = _mm_xor_si128(mmrk, scratch1);
    state      = _mm_aesenc_si128(state, mmrk);
    scratch1   = _mm_shuffle_epi8(mmrk, mask);
    scratch1   = _mm_aesenclast_si128(scratch1,mmrcon);
    scratch0   = _mm_slli_si128(mmrk, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    scratch0   = _mm_slli_si128(scratch0, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    scratch0   = _mm_slli_si128(scratch0, 0x4);
    mmrk       = _mm_xor_si128(mmrk, scratch0);
    mmrk       = _mm_xor_si128(mmrk, scratch1);
    state      = _mm_aesenclast_si128(state, mmrk);

    _mm_storeu_si128((__m128i*)&dataOut[keyIdx] ,state);

  }

}
