// Author: Jake Longo
// Description: Code Luke was using in his verfication
//              setup.
//
#include <stdint.h>
#include <wmmintrin.h>

#include "luke_impl.h"

// Luke Mather

#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, _mm_aeskeygenassist_si128(k, rcon))

static __m128i aes_128_key_expansion(__m128i key, __m128i keygened){
    keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3,3,3,3));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    return _mm_xor_si128(key, keygened);
}

void c_luke_impl(uint8_t *keys, uint8_t *data, uint8_t *dataOut) {

  __m128i data0  = _mm_load_si128((__m128i*)data);
  __m128i state;
  __m128i roundKeys[11];


  for (uint8_t keyIdx = 0; keyIdx < 64; keyIdx += 16) {

    roundKeys[0]  = _mm_load_si128((__m128i*)&(keys[keyIdx]));
    roundKeys[1]  = AES_128_key_exp(roundKeys[0], 0x01);
    roundKeys[2]  = AES_128_key_exp(roundKeys[1], 0x02);
    roundKeys[3]  = AES_128_key_exp(roundKeys[2], 0x04);
    roundKeys[4]  = AES_128_key_exp(roundKeys[3], 0x08);
    roundKeys[5]  = AES_128_key_exp(roundKeys[4], 0x10);
    roundKeys[6]  = AES_128_key_exp(roundKeys[5], 0x20);
    roundKeys[7]  = AES_128_key_exp(roundKeys[6], 0x40);
    roundKeys[8]  = AES_128_key_exp(roundKeys[7], 0x80);
    roundKeys[9]  = AES_128_key_exp(roundKeys[8], 0x1B);
    roundKeys[10] = AES_128_key_exp(roundKeys[9], 0x36);

    state = _mm_xor_si128(data0,    roundKeys[0]);

    state = _mm_aesenc_si128(state, roundKeys[1]);
    state = _mm_aesenc_si128(state, roundKeys[2]);
    state = _mm_aesenc_si128(state, roundKeys[3]);
    state = _mm_aesenc_si128(state, roundKeys[4]);
    state = _mm_aesenc_si128(state, roundKeys[5]);
    state = _mm_aesenc_si128(state, roundKeys[6]);
    state = _mm_aesenc_si128(state, roundKeys[7]);
    state = _mm_aesenc_si128(state, roundKeys[8]);
    state = _mm_aesenc_si128(state, roundKeys[9]);

    state = _mm_aesenclast_si128(state, roundKeys[10]);

    _mm_store_si128((__m128i*)&(dataOut[keyIdx]), state);

  }
}