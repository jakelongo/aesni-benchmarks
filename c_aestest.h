#ifndef __C_AESTEST_H__
#define __C_AESTEST_H__

// add linkerage
#ifdef __cplusplus
extern "C" {
#endif

void c_unrolled_8x(uint8_t *keys, uint8_t *data, uint8_t *dataOut);
void c_unrolled_4x(uint8_t *keys, uint8_t *data, uint8_t *dataOut);

void c_opt_unrolled_8x(uint8_t *keys, uint8_t *data, uint8_t *dataOut);

#ifdef __cplusplus
}
#endif

#endif /* __C_AESTEST_H__ */