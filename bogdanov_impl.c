// Author: Jake Longo
// Description: Implementation based on the description of Bogdanov et al.[1]
//
// [1]: https://eprint.iacr.org/2015/795.pdf
//
#include <stdint.h>
#include <wmmintrin.h>

#ifndef _WIN32
#include <x86intrin.h>
#endif

#include "bogdanov_impl.h"

static const uint32_t rcon [] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
                                 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a };

static inline void transpose_state(__m128i *stateOut, __m128i *stateIn) {

  __m128i tmpi;
  __m128i tmpj;

  tmpi    = _mm_unpacklo_epi32(stateIn[0], stateIn[1]);
  tmpj    = _mm_unpacklo_epi32(stateIn[2], stateIn[3]);

  stateOut[0] = _mm_unpacklo_epi64(tmpi, tmpj);
  stateOut[1] = _mm_unpackhi_epi64(tmpi, tmpj);

  tmpi   = _mm_unpackhi_epi32(stateIn[0], stateIn[1]);
  tmpj   = _mm_unpackhi_epi32(stateIn[2], stateIn[3]);

  stateOut[2] = _mm_unpacklo_epi64(tmpi, tmpj);
  stateOut[3] = _mm_unpackhi_epi64(tmpi, tmpj);

}

void c_bogdanov_4x(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {

  __m128i ROL8_MASK     = _mm_set_epi8(0x0C, 0x0F, 0x0E, 0x0D, 0x08, 0x0B, 0x0A, 0x09,
                                       0x04, 0x07, 0x06, 0x05, 0x00, 0x03, 0x02, 0x01);

  __m128i SHIFTROWS_INV = _mm_set_epi8(0x03, 0x06, 0x09, 0x0C, 0x0F, 0x02, 0x05, 0x08,
                                       0x0B, 0x0E, 0x01, 0x04, 0x07, 0x0A, 0x0D, 0x00);

  __m128i tmp;

  __m128i block[4];
  __m128i rk[4];
  __m128i w[4];

  __m128i ZERO = _mm_xor_si128(tmp, tmp);

  rk[0] = _mm_load_si128((__m128i*)&(keys[ 0]));
  rk[1] = _mm_load_si128((__m128i*)&(keys[16]));
  rk[2] = _mm_load_si128((__m128i*)&(keys[32]));
  rk[3] = _mm_load_si128((__m128i*)&(keys[48]));

  block[0]  = _mm_load_si128((__m128i*)  data );
  block[1]  = block[0];
  block[2]  = block[0];
  block[3]  = block[0];

  __m128i RC;

  block[0]   = _mm_xor_si128(block[0], rk[0]);
  block[1]   = _mm_xor_si128(block[1], rk[1]);
  block[2]   = _mm_xor_si128(block[2], rk[2]);
  block[3]   = _mm_xor_si128(block[3], rk[3]);

  transpose_state(w, rk);

  tmp   = _mm_shuffle_epi8(w[3], ROL8_MASK);
  tmp   = _mm_aesenclast_si128(tmp, ZERO);
  tmp   = _mm_shuffle_epi8(tmp, SHIFTROWS_INV);
  RC    = _mm_set1_epi32(rcon[1]);

  tmp   = _mm_xor_si128(tmp, RC);
  w[0]  = _mm_xor_si128(w[0], tmp );
  w[1]  = _mm_xor_si128(w[1], w[0]);
  w[2]  = _mm_xor_si128(w[2], w[1]);
  w[3]  = _mm_xor_si128(w[3], w[2]);

  transpose_state(rk, w);

  for (uint32_t r = 2; r < 11; ++r) {

    tmp   = _mm_shuffle_epi8(w[3], ROL8_MASK);
    tmp   = _mm_aesenclast_si128(tmp, ZERO);

    block[0]   = _mm_aesenc_si128(block[0], rk[0]);
    block[1]   = _mm_aesenc_si128(block[1], rk[1]);
    block[2]   = _mm_aesenc_si128(block[2], rk[2]);
    block[3]   = _mm_aesenc_si128(block[3], rk[3]);

    tmp      = _mm_shuffle_epi8(tmp, SHIFTROWS_INV);
    RC       = _mm_set1_epi32(rcon[r]);

    tmp   = _mm_xor_si128(tmp, RC);
    w[0]  = _mm_xor_si128(w[0], tmp );
    w[1]  = _mm_xor_si128(w[1], w[0]);
    w[2]  = _mm_xor_si128(w[2], w[1]);
    w[3]  = _mm_xor_si128(w[3], w[2]);

    transpose_state(rk, w);

  }

  block[0]   = _mm_aesenclast_si128(block[0], rk[0]);
  block[1]   = _mm_aesenclast_si128(block[1], rk[1]);
  block[2]   = _mm_aesenclast_si128(block[2], rk[2]);
  block[3]   = _mm_aesenclast_si128(block[3], rk[3]);

  _mm_store_si128((__m128i*)&(dataOut[00]), block[0]);
  _mm_store_si128((__m128i*)&(dataOut[16]), block[1]);
  _mm_store_si128((__m128i*)&(dataOut[32]), block[2]);
  _mm_store_si128((__m128i*)&(dataOut[48]), block[3]);

}