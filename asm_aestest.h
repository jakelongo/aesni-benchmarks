#ifndef __ASM_AESTEST_H__
#define __ASM_AESTEST_H__

// add linkerage
#ifdef __cplusplus
extern "C" {
#endif

void asm_unrolled_4x(uint8_t *keys, uint8_t *data, uint8_t *dataOut);
void asm_unrolled_8x(uint8_t *keys, uint8_t *data, uint8_t *dataOut);

#ifdef __cplusplus
}
#endif

#endif /* __ASM_AESTEST_H__ */