
.intel_syntax noprefix

.macro unwind_blocks_x4 out0, out1, out2, out3, in0, in1, in2, in3, tmp0, tmp1
  vpunpckldq  \tmp0, \in0,  \in1
  vpunpckldq  \tmp1, \in2,  \in3

  vpunpcklqdq \out0, \tmp0, \tmp1
  vpunpckhqdq \out1, \tmp0, \tmp1

  vpunpckhdq  \tmp0, \in0,  \in1
  vpunpckhdq  \tmp1, \in2,  \in3

  vpunpcklqdq \out2, \tmp0, \tmp1
  vpunpckhqdq \out3, \tmp0, \tmp1
.endm

.macro batch_rewind_lo in0, in1, in2, in3, out0, out1
  vpunpckldq  \in1,  \in0,  \out0
  vpunpckldq  \in3,  \in2,  \out1
  vpunpcklqdq \out1, \out0, \out0
  vpunpckhqdq \out1, \out0, \out1
.endm

.macro batch_rewind__hi in0, in1, in2, in3, out0, out1
  vpunpckhdq  \in1,  \in0,  \out0
  vpunpckhdq  \in3,  \in2,  \out1
  vpunpcklqdq \out1, \out0, \out0
  vpunpckhqdq \out1, \out0, \out1
.endm

.macro encblock data0, data1, data2, data3, \
                key0,  key1,  key2,  key3,  \
                rkey0, rkey1, rkey2, rkey3, \
                mask,  rcon,  tmp0,  tmp1

  pxor        \tmp0,   \tmp0
  vaesenclast \tmp0,   \rkey3,  \tmp0
  pshufb      \tmp0,   \mask

  aesenc      \data0,  \key0
  aesenc      \data1,  \key1
  aesenc      \data2,  \key2
  aesenc      \data3,  \key3

  pxor        \tmp0,   \rcon
  pxor        \rkey0,  \tmp0
  pxor        \rkey1,  \rkey0
  pxor        \rkey2,  \rkey1
  pxor        \rkey3,  \rkey2

  unwind_blocks_x4 \key0, \key1, \key2, \key3, \rkey0, \rkey1, \rkey2, \rkey3, \tmp0, \tmp1

.endm

.align 16
rcon:
  .long 141                       ## 0x8d
  .long 1                         ## 0x1
  .long 2                         ## 0x2
  .long 4                         ## 0x4
  .long 8                         ## 0x8
  .long 16                        ## 0x10
  .long 32                        ## 0x20
  .long 64                        ## 0x40
  .long 128                       ## 0x80
  .long 27                        ## 0x1b
  .long 54                        ## 0x36
  .long 108                       ## 0x6c
  .long 216                       ## 0xd8
  .long 171                       ## 0xab
  .long 77                        ## 0x4d
  .long 154                       ## 0x9a

.align 16
maskLiteral:
  .byte   13                      ## 0xd
  .byte   10                      ## 0xa
  .byte   7                       ## 0x7
  .byte   0                       ## 0x0
  .byte   1                       ## 0x1
  .byte   14                      ## 0xe
  .byte   11                      ## 0xb
  .byte   4                       ## 0x4
  .byte   5                       ## 0x5
  .byte   2                       ## 0x2
  .byte   15                      ## 0xf
  .byte   8                       ## 0x8
  .byte   9                       ## 0x9
  .byte   6                       ## 0x6
  .byte   3                       ## 0x3
  .byte   12                      ## 0xc

.align 16
rconOne:
  .quad 0x01

.align 16
rconFinal:
  .quad 0x1b


// This function follows the construction defined
// by Bogdanoc et al.

.globl _asm_unrolled_4x
.globl asm_unrolled_4x

_asm_unrolled_4x:
asm_unrolled_4x:

// keys:    rdi
// data:    rsi
// dataOut: rdx

// xmm0  = mask
// xmm1  = tmp[0]
// xmm2  = tmp[1]
// xmm3  = data / rcon
// xmm4  = key[0]
// xmm5  = key[1]
// xmm6  = key[2]
// xmm7  = key[3]
// xmm8  = data[0]
// xmm9  = data[1]
// xmm10 = data[2]
// xmm11 = data[3]
// xmm12 = rkey[0]
// xmm13 = rkey[1]
// xmm14 = rkey[2]
// xmm15 = rkey[3]

  movdqa xmm0, xmmword ptr [rip + maskLiteral]

  movdqa xmm3, xmmword ptr [rsi]

  movdqa xmm4, [rdi+ 00]
  movdqa xmm5, [rdi+ 16]
  movdqa xmm6, [rdi+ 32]
  movdqa xmm7, [rdi+ 48]

  vpxor  xmm8,  xmm4, xmm3
  vpxor  xmm9,  xmm5, xmm3
  vpxor  xmm10, xmm6, xmm3
  vpxor  xmm11, xmm7, xmm3

  vbroadcastss xmm3, [rip + rconOne]

  unwind_blocks_x4 xmm12, xmm13, xmm14, xmm15, xmm4, xmm5, xmm6, xmm7, xmm1, xmm2

  pxor        xmm2,   xmm2
  vaesenclast xmm1,   xmm15,  xmm2
  pshufb      xmm1,   xmm0

  vpxor       xmm2,   xmm3,   xmm1
  pxor        xmm12,  xmm2
  pxor        xmm13,  xmm12
  pxor        xmm14,  xmm13
  pxor        xmm15,  xmm14

  unwind_blocks_x4 xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm1, xmm2

  vbroadcastss xmm3, [rip + rcon + 0x08]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x0C]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x10]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x14]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x18]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x1C]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x20]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x24]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2
  vbroadcastss xmm3, [rip + rcon + 0x28]
  encblock xmm8, xmm9, xmm10, xmm11, xmm4, xmm5, xmm6, xmm7, xmm12, xmm13, xmm14, xmm15, xmm0, xmm3, xmm1, xmm2

  aesenclast  xmm8,  xmm4
  aesenclast  xmm9,  xmm5
  aesenclast  xmm10, xmm6
  aesenclast  xmm11, xmm7

  vmovdqa   [rdx + 00], xmm8
  vmovdqa   [rdx + 16], xmm9
  vmovdqa   [rdx + 32], xmm10
  vmovdqa   [rdx + 48], xmm11

  ret
