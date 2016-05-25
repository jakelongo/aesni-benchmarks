// Author: Jake Longo
// Description: Intrinsics implementations for key testing
//
#include <wmmintrin.h>
#include <stdint.h>

#ifndef _WIN32
#include <x86intrin.h>
#endif

static const uint32_t rcon [] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
                          0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a };

void c_unrolled_8x(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {

  __m128i mask = _mm_set_epi8(0x0C, 0x03, 0x06, 0x09, 0x08, 0x0F, 0x02, 0x05,
                              0x04, 0x0B, 0x0E, 0x01, 0x00, 0x07, 0x0A, 0x0D);

  __m128i tmp0;
  __m128i zero = _mm_xor_si128(tmp0, tmp0);

  __m128i key0  = _mm_load_si128((__m128i*)&(keys[ 0]));
  __m128i key1  = _mm_load_si128((__m128i*)&(keys[16]));
  __m128i key2  = _mm_load_si128((__m128i*)&(keys[32]));
  __m128i key3  = _mm_load_si128((__m128i*)&(keys[48]));

  __m128i key4  = _mm_load_si128((__m128i*)&(keys[64]));
  __m128i key5  = _mm_load_si128((__m128i*)&(keys[80]));
  __m128i key6  = _mm_load_si128((__m128i*)&(keys[96]));
  __m128i key7  = _mm_load_si128((__m128i*)&(keys[112]));

  __m128i data0  = _mm_load_si128((__m128i*)  data     );
  __m128i data1  = data0;
  __m128i data2  = data0;
  __m128i data3  = data0;

  __m128i data4  = data0;
  __m128i data5  = data0;
  __m128i data6  = data0;
  __m128i data7  = data0;

  __m128i rk0;
  __m128i rk1;
  __m128i rk2;
  __m128i rk3;
  __m128i rk4;
  __m128i rk5;
  __m128i rk6;
  __m128i rk7;

  __m128i rki;
  __m128i rkj;

  __m128i mmrcon = _mm_set1_epi32(rcon[1]);

  data0   = _mm_xor_si128(data0, key0);
  data1   = _mm_xor_si128(data1, key1);
  data2   = _mm_xor_si128(data2, key2);
  data3   = _mm_xor_si128(data3, key3);

  data4   = _mm_xor_si128(data4, key4);
  data5   = _mm_xor_si128(data5, key5);
  data6   = _mm_xor_si128(data6, key6);
  data7   = _mm_xor_si128(data7, key7);

  rki     = _mm_unpacklo_epi32(key0, key1);
  rkj     = _mm_unpacklo_epi32(key2, key3);
  rk0     = _mm_unpacklo_epi64(rki, rkj);
  rk1     = _mm_unpackhi_epi64(rki, rkj);

  rki     = _mm_unpackhi_epi32(key0, key1);
  rkj     = _mm_unpackhi_epi32(key2, key3);
  rk2     = _mm_unpacklo_epi64(rki, rkj);
  rk3     = _mm_unpackhi_epi64(rki, rkj);

  rki     = _mm_unpacklo_epi32(key4, key5);
  rkj     = _mm_unpacklo_epi32(key6, key7);
  rk4     = _mm_unpacklo_epi64(rki, rkj);
  rk5     = _mm_unpackhi_epi64(rki, rkj);

  rki     = _mm_unpackhi_epi32(key4, key5);
  rkj     = _mm_unpackhi_epi32(key6, key7);
  rk6     = _mm_unpacklo_epi64(rki, rkj);
  rk7     = _mm_unpackhi_epi64(rki, rkj);

  tmp0    = _mm_aesenclast_si128(rk3, zero);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);

  tmp0   = _mm_xor_si128(tmp0, mmrcon);
  rk0    = _mm_xor_si128(rk0, tmp0);
  rk1    = _mm_xor_si128(rk1, rk0);
  rk2    = _mm_xor_si128(rk2, rk1);
  rk3    = _mm_xor_si128(rk3, rk2);

  tmp0    = _mm_aesenclast_si128(rk7, zero);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);

  tmp0   = _mm_xor_si128(tmp0, mmrcon);
  rk4    = _mm_xor_si128(rk4, tmp0);
  rk5    = _mm_xor_si128(rk5, rk4);
  rk6    = _mm_xor_si128(rk6, rk5);
  rk7    = _mm_xor_si128(rk7, rk6);

  rki    = _mm_unpacklo_epi32(rk0, rk1);
  rkj    = _mm_unpacklo_epi32(rk2, rk3);
  key0   = _mm_unpacklo_epi64(rki, rkj);
  key1   = _mm_unpackhi_epi64(rki, rkj);

  rki    = _mm_unpackhi_epi32(rk0, rk1);
  rkj    = _mm_unpackhi_epi32(rk2, rk3);
  key2   = _mm_unpacklo_epi64(rki, rkj);
  key3   = _mm_unpackhi_epi64(rki, rkj);

  rki    = _mm_unpacklo_epi32(rk4, rk5);
  rkj    = _mm_unpacklo_epi32(rk6, rk7);
  key4   = _mm_unpacklo_epi64(rki, rkj);
  key5   = _mm_unpackhi_epi64(rki, rkj);

  rki    = _mm_unpackhi_epi32(rk4, rk5);
  rkj    = _mm_unpackhi_epi32(rk6, rk7);
  key6   = _mm_unpacklo_epi64(rki, rkj);
  key7   = _mm_unpackhi_epi64(rki, rkj);

  for (uint8_t roundCounter = 1; roundCounter < 10; roundCounter++) {

    tmp0    = _mm_aesenclast_si128(rk3, zero);
    tmp0    = _mm_shuffle_epi8(tmp0, mask);
    mmrcon  = _mm_set1_epi32(rcon[roundCounter+1]);

    data0   = _mm_aesenc_si128(data0, key0);
    data1   = _mm_aesenc_si128(data1, key1);
    data2   = _mm_aesenc_si128(data2, key2);
    data3   = _mm_aesenc_si128(data3, key3);

    data4   = _mm_aesenc_si128(data4, key4);
    data5   = _mm_aesenc_si128(data5, key5);
    data6   = _mm_aesenc_si128(data6, key6);
    data7   = _mm_aesenc_si128(data7, key7);

    tmp0   = _mm_xor_si128(tmp0, mmrcon);
    rk0    = _mm_xor_si128(rk0, tmp0);
    rk1    = _mm_xor_si128(rk1, rk0);
    rk2    = _mm_xor_si128(rk2, rk1);
    rk3    = _mm_xor_si128(rk3, rk2);

    tmp0    = _mm_aesenclast_si128(rk7, zero);
    tmp0    = _mm_shuffle_epi8(tmp0, mask);

    tmp0   = _mm_xor_si128(tmp0, mmrcon);
    rk4    = _mm_xor_si128(rk4, tmp0);
    rk5    = _mm_xor_si128(rk5, rk4);
    rk6    = _mm_xor_si128(rk6, rk5);
    rk7    = _mm_xor_si128(rk7, rk6);

    rki    = _mm_unpacklo_epi32(rk0, rk1);
    rkj    = _mm_unpacklo_epi32(rk2, rk3);
    key0   = _mm_unpacklo_epi64(rki, rkj);
    key1   = _mm_unpackhi_epi64(rki, rkj);

    rki    = _mm_unpackhi_epi32(rk0, rk1);
    rkj    = _mm_unpackhi_epi32(rk2, rk3);
    key2   = _mm_unpacklo_epi64(rki, rkj);
    key3   = _mm_unpackhi_epi64(rki, rkj);

    rki    = _mm_unpacklo_epi32(rk4, rk5);
    rkj    = _mm_unpacklo_epi32(rk6, rk7);
    key4   = _mm_unpacklo_epi64(rki, rkj);
    key5   = _mm_unpackhi_epi64(rki, rkj);

    rki    = _mm_unpackhi_epi32(rk4, rk5);
    rkj    = _mm_unpackhi_epi32(rk6, rk7);
    key6   = _mm_unpacklo_epi64(rki, rkj);
    key7   = _mm_unpackhi_epi64(rki, rkj);

  }

  data0   = _mm_aesenclast_si128(data0, key0);
  data1   = _mm_aesenclast_si128(data1, key1);
  data2   = _mm_aesenclast_si128(data2, key2);
  data3   = _mm_aesenclast_si128(data3, key3);

  data4   = _mm_aesenclast_si128(data4, key4);
  data5   = _mm_aesenclast_si128(data5, key5);
  data6   = _mm_aesenclast_si128(data6, key6);
  data7   = _mm_aesenclast_si128(data7, key7);

  _mm_store_si128((__m128i*)&(dataOut[00]), data0);
  _mm_store_si128((__m128i*)&(dataOut[16]), data1);
  _mm_store_si128((__m128i*)&(dataOut[32]), data2);
  _mm_store_si128((__m128i*)&(dataOut[48]), data3);

  _mm_store_si128((__m128i*)&(dataOut[64]),  data4);
  _mm_store_si128((__m128i*)&(dataOut[80]),  data5);
  _mm_store_si128((__m128i*)&(dataOut[96]),  data6);
  _mm_store_si128((__m128i*)&(dataOut[112]), data7);

}

void c_unrolled_4x(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {

  __m128i mask = _mm_set_epi8(0x0C, 0x03, 0x06, 0x09, 0x08, 0x0F, 0x02, 0x05,
                              0x04, 0x0B, 0x0E, 0x01, 0x00, 0x07, 0x0A, 0x0D);

  __m128i tmp0;
  __m128i zero = _mm_xor_si128(tmp0, tmp0);

  __m128i key0  = _mm_load_si128((__m128i*)&(keys[ 0]));
  __m128i key1  = _mm_load_si128((__m128i*)&(keys[16]));
  __m128i key2  = _mm_load_si128((__m128i*)&(keys[32]));
  __m128i key3  = _mm_load_si128((__m128i*)&(keys[48]));

  __m128i data0  = _mm_load_si128((__m128i*)  data     );
  __m128i data1  = data0;
  __m128i data2  = data0;
  __m128i data3  = data0;

  __m128i rk0;
  __m128i rk1;
  __m128i rk2;
  __m128i rk3;
  __m128i rki;
  __m128i rkj;

  __m128i mmrcon;

  data0   = _mm_xor_si128(data0, key0);
  data1   = _mm_xor_si128(data1, key1);
  data2   = _mm_xor_si128(data2, key2);
  data3   = _mm_xor_si128(data3, key3);

  rki     = _mm_unpacklo_epi32(key0, key1);
  rkj     = _mm_unpacklo_epi32(key2, key3);
  rk0     = _mm_unpacklo_epi64(rki, rkj);
  rk1     = _mm_unpackhi_epi64(rki, rkj);

  rki     = _mm_unpackhi_epi32(key0, key1);
  rkj     = _mm_unpackhi_epi32(key2, key3);
  rk2     = _mm_unpacklo_epi64(rki, rkj);
  rk3     = _mm_unpackhi_epi64(rki, rkj);

  tmp0    = _mm_aesenclast_si128(rk3, zero);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);
  mmrcon  = _mm_set1_epi32(rcon[1]);

  tmp0   = _mm_xor_si128(tmp0, mmrcon);
  rk0    = _mm_xor_si128(rk0, tmp0);
  rk1    = _mm_xor_si128(rk1, rk0);
  rk2    = _mm_xor_si128(rk2, rk1);
  rk3    = _mm_xor_si128(rk3, rk2);

  rki    = _mm_unpacklo_epi32(rk0, rk1);
  rkj    = _mm_unpacklo_epi32(rk2, rk3);
  key0   = _mm_unpacklo_epi64(rki, rkj);
  key1   = _mm_unpackhi_epi64(rki, rkj);

  rki    = _mm_unpackhi_epi32(rk0, rk1);
  rkj    = _mm_unpackhi_epi32(rk2, rk3);
  key2   = _mm_unpacklo_epi64(rki, rkj);
  key3   = _mm_unpackhi_epi64(rki, rkj);

  for (uint8_t roundCounter = 1; roundCounter < 10; roundCounter++) {

    tmp0    = _mm_aesenclast_si128(rk3, zero);
    tmp0    = _mm_shuffle_epi8(tmp0, mask);
    mmrcon  = _mm_set1_epi32(rcon[roundCounter+1]);

    data0   = _mm_aesenc_si128(data0, key0);
    data1   = _mm_aesenc_si128(data1, key1);
    data2   = _mm_aesenc_si128(data2, key2);
    data3   = _mm_aesenc_si128(data3, key3);

    tmp0   = _mm_xor_si128(tmp0, mmrcon);
    rk0    = _mm_xor_si128(rk0, tmp0);
    rk1    = _mm_xor_si128(rk1, rk0);
    rk2    = _mm_xor_si128(rk2, rk1);
    rk3    = _mm_xor_si128(rk3, rk2);

    rki    = _mm_unpacklo_epi32(rk0, rk1);
    rkj    = _mm_unpacklo_epi32(rk2, rk3);
    key0   = _mm_unpacklo_epi64(rki, rkj);
    key1   = _mm_unpackhi_epi64(rki, rkj);

    rki    = _mm_unpackhi_epi32(rk0, rk1);
    rkj    = _mm_unpackhi_epi32(rk2, rk3);
    key2   = _mm_unpacklo_epi64(rki, rkj);
    key3   = _mm_unpackhi_epi64(rki, rkj);

  }

  data0   = _mm_aesenclast_si128(data0, key0);
  data1   = _mm_aesenclast_si128(data1, key1);
  data2   = _mm_aesenclast_si128(data2, key2);
  data3   = _mm_aesenclast_si128(data3, key3);

  _mm_store_si128((__m128i*)&(dataOut[00]), data0);
  _mm_store_si128((__m128i*)&(dataOut[16]), data1);
  _mm_store_si128((__m128i*)&(dataOut[32]), data2);
  _mm_store_si128((__m128i*)&(dataOut[48]), data3);
}

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

void c_opt_unrolled_8x(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {

  __m128i mask        = _mm_set_epi8(0x0C, 0x03, 0x06, 0x09, 0x08, 0x0F, 0x02, 0x05,
                                     0x04, 0x0B, 0x0E, 0x01, 0x00, 0x07, 0x0A, 0x0D);

  __m128i mmrcon      = _mm_set_epi8(0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
                                     0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00);

  __m128i mmrconFinal = _mm_set_epi8(0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1b, 0x00,
                                     0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x1b, 0x00);

  __m128i key  [8];
  __m128i rkey [8];
  __m128i state[8];
  __m128i tmp0;

  key[0]  = _mm_load_si128((__m128i*)&(keys[  0]));
  key[1]  = _mm_load_si128((__m128i*)&(keys[ 16]));
  key[2]  = _mm_load_si128((__m128i*)&(keys[ 32]));
  key[3]  = _mm_load_si128((__m128i*)&(keys[ 48]));
  key[4]  = _mm_load_si128((__m128i*)&(keys[ 64]));
  key[5]  = _mm_load_si128((__m128i*)&(keys[ 80]));
  key[6]  = _mm_load_si128((__m128i*)&(keys[ 96]));
  key[7]  = _mm_load_si128((__m128i*)&(keys[112]));

  transpose_state(rkey, key);
  transpose_state(&(rkey[4]), &(key[4]));

  state[0]  = _mm_load_si128((__m128i*)  data     );
  state[1]  = state[0];
  state[2]  = state[0];
  state[3]  = state[0];
  state[4]  = state[0];
  state[5]  = state[0];
  state[6]  = state[0];
  state[7]  = state[0];

  tmp0    = _mm_aesenclast_si128(rkey[3], mmrcon);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);

  state[0]   = _mm_xor_si128(state[0], key[0]);
  state[1]   = _mm_xor_si128(state[1], key[1]);
  state[2]   = _mm_xor_si128(state[2], key[2]);
  state[3]   = _mm_xor_si128(state[3], key[3]);
  state[4]   = _mm_xor_si128(state[4], key[4]);
  state[5]   = _mm_xor_si128(state[5], key[5]);
  state[6]   = _mm_xor_si128(state[6], key[6]);
  state[7]   = _mm_xor_si128(state[7], key[7]);

  rkey[0]    = _mm_xor_si128(rkey[0], tmp0);

  tmp0    = _mm_aesenclast_si128(rkey[7], mmrcon);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);
  mmrcon  = _mm_slli_epi16(mmrcon, 0x01);

  rkey[1]    = _mm_xor_si128(rkey[1], rkey[0]);
  rkey[2]    = _mm_xor_si128(rkey[2], rkey[1]);
  rkey[3]    = _mm_xor_si128(rkey[3], rkey[2]);

  rkey[4]    = _mm_xor_si128(rkey[4], tmp0);
  rkey[5]    = _mm_xor_si128(rkey[5], rkey[4]);
  rkey[6]    = _mm_xor_si128(rkey[6], rkey[5]);
  rkey[7]    = _mm_xor_si128(rkey[7], rkey[6]);

  _mm_prefetch((char const *)state, 0);

  transpose_state(key, rkey);
  transpose_state(&(key[4]), &(rkey[4]));

  for (uint8_t roundCounter = 1; roundCounter < 8; roundCounter++) {

    tmp0    = _mm_aesenclast_si128(rkey[3], mmrcon);
    tmp0    = _mm_shuffle_epi8(tmp0, mask);

    state[0]   = _mm_aesenc_si128(state[0], key[0]);
    state[1]   = _mm_aesenc_si128(state[1], key[1]);
    state[2]   = _mm_aesenc_si128(state[2], key[2]);
    state[3]   = _mm_aesenc_si128(state[3], key[3]);
    state[4]   = _mm_aesenc_si128(state[4], key[4]);
    state[5]   = _mm_aesenc_si128(state[5], key[5]);

    rkey[0]    = _mm_xor_si128(rkey[0], tmp0);
    tmp0    = _mm_aesenclast_si128(rkey[7], mmrcon);

    rkey[1]    = _mm_xor_si128(rkey[1], rkey[0]);
    rkey[2]    = _mm_xor_si128(rkey[2], rkey[1]);
    rkey[3]    = _mm_xor_si128(rkey[3], rkey[2]);

    state[6]   = _mm_aesenc_si128(state[6], key[6]);
    state[7]   = _mm_aesenc_si128(state[7], key[7]);

    transpose_state(key, rkey);

    tmp0    = _mm_shuffle_epi8(tmp0, mask);
    mmrcon  = _mm_slli_epi16(mmrcon, 0x01);

    rkey[4]    = _mm_xor_si128(rkey[4], tmp0);
    rkey[5]    = _mm_xor_si128(rkey[5], rkey[4]);
    rkey[6]    = _mm_xor_si128(rkey[6], rkey[5]);
    rkey[7]    = _mm_xor_si128(rkey[7], rkey[6]);

    _mm_prefetch((char const *)state, 0);
    transpose_state(&(key[4]), &(rkey[4]));

  }

  tmp0    = _mm_aesenclast_si128(rkey[3], mmrconFinal);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);

  state[0]   = _mm_aesenc_si128(state[0], key[0]);
  state[1]   = _mm_aesenc_si128(state[1], key[1]);
  state[2]   = _mm_aesenc_si128(state[2], key[2]);
  state[3]   = _mm_aesenc_si128(state[3], key[3]);
  state[4]   = _mm_aesenc_si128(state[4], key[4]);
  state[5]   = _mm_aesenc_si128(state[5], key[5]);

  rkey[0]    = _mm_xor_si128(rkey[0], tmp0);
  tmp0       = _mm_aesenclast_si128(rkey[7], mmrconFinal);

  rkey[1]    = _mm_xor_si128(rkey[1], rkey[0]);
  rkey[2]    = _mm_xor_si128(rkey[2], rkey[1]);
  rkey[3]    = _mm_xor_si128(rkey[3], rkey[2]);

  state[6]   = _mm_aesenc_si128(state[6], key[6]);
  state[7]   = _mm_aesenc_si128(state[7], key[7]);

  transpose_state(key, rkey);

  tmp0          = _mm_shuffle_epi8(tmp0, mask);
  mmrconFinal   = _mm_slli_epi16(mmrconFinal, 0x01);

  rkey[4]    = _mm_xor_si128(rkey[4], tmp0);
  rkey[5]    = _mm_xor_si128(rkey[5], rkey[4]);
  rkey[6]    = _mm_xor_si128(rkey[6], rkey[5]);
  rkey[7]    = _mm_xor_si128(rkey[7], rkey[6]);

  _mm_prefetch((char const *)state, 0);
  transpose_state(&(key[4]), &(rkey[4]));

  tmp0    = _mm_aesenclast_si128(rkey[3], mmrconFinal);
  tmp0    = _mm_shuffle_epi8(tmp0, mask);

  state[0]   = _mm_aesenc_si128(state[0], key[0]);
  state[1]   = _mm_aesenc_si128(state[1], key[1]);
  state[2]   = _mm_aesenc_si128(state[2], key[2]);
  state[3]   = _mm_aesenc_si128(state[3], key[3]);
  state[4]   = _mm_aesenc_si128(state[4], key[4]);
  state[5]   = _mm_aesenc_si128(state[5], key[5]);

  rkey[0]    = _mm_xor_si128(rkey[0], tmp0);
  tmp0       = _mm_aesenclast_si128(rkey[7], mmrconFinal);

  rkey[1]    = _mm_xor_si128(rkey[1], rkey[0]);
  rkey[2]    = _mm_xor_si128(rkey[2], rkey[1]);
  rkey[3]    = _mm_xor_si128(rkey[3], rkey[2]);

  state[6]   = _mm_aesenc_si128(state[6], key[6]);
  state[7]   = _mm_aesenc_si128(state[7], key[7]);

  transpose_state(key, rkey);
  tmp0          = _mm_shuffle_epi8(tmp0, mask);

  state[0]   = _mm_aesenclast_si128(state[0], key[0]);
  state[1]   = _mm_aesenclast_si128(state[1], key[1]);
  state[2]   = _mm_aesenclast_si128(state[2], key[2]);
  state[3]   = _mm_aesenclast_si128(state[3], key[3]);

  rkey[4]    = _mm_xor_si128(rkey[4], tmp0);
  rkey[5]    = _mm_xor_si128(rkey[5], rkey[4]);
  rkey[6]    = _mm_xor_si128(rkey[6], rkey[5]);
  rkey[7]    = _mm_xor_si128(rkey[7], rkey[6]);

  transpose_state(&(key[4]), &(rkey[4]));

  state[4]   = _mm_aesenclast_si128(state[4], key[4]);
  state[5]   = _mm_aesenclast_si128(state[5], key[5]);
  state[6]   = _mm_aesenclast_si128(state[6], key[6]);
  state[7]   = _mm_aesenclast_si128(state[7], key[7]);

  _mm_store_si128((__m128i*)&(dataOut[  0]),  state[0]);
  _mm_store_si128((__m128i*)&(dataOut[ 16]),  state[1]);
  _mm_store_si128((__m128i*)&(dataOut[ 32]),  state[2]);
  _mm_store_si128((__m128i*)&(dataOut[ 48]),  state[3]);
  _mm_store_si128((__m128i*)&(dataOut[ 64]),  state[4]);
  _mm_store_si128((__m128i*)&(dataOut[ 80]),  state[5]);
  _mm_store_si128((__m128i*)&(dataOut[ 96]),  state[6]);
  _mm_store_si128((__m128i*)&(dataOut[112]),  state[7]);

}