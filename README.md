# aesni-benchmarks

This project is simply a framework for benchmarking the performance of AES-NI
accelerated brute-force efforts.

### Hardware requirements

The framework requires an Intel platform with a micro architecture
≥ Westmere (although I've only tested it on Sandy Bridge and Haswell).

### Output metrics

The framework reports on three different metrics: execution time (in seconds),
throughput (in cycles per byte) and keys per second. I would recommending
using throughput as the only *meaningful* metric but the others can be useful.

### Parameters

The benchmarks.cpp contains the preprocessor definitions `NUMER_OF_KEYS` and
`NUMBER_OF_REPEATS` that control the conditions for testing. These should
be read in as command line arguments! `¯\_(ツ)_/¯`

### Adding tests

Implementations for testing should have the following prototype definition:

```C
void testname(uint8_t *keys, uint8_t *data, uint8_t *dataOut);
```

and are included in the test suite by adding this call to benchmarks.cpp:

```C
runBenchmarks(&testname, keys, data, cips, comp, STRIDE, "testname_label");
```
where `testname` is your new implementation, `STRIDE` is an integer that sets
the number of keys tested per invocation of `testname` and `testname_label` is
the label that will be printed when producing the benchmark stats.

---

## Implementations included

Source          | Description
--------------- | -------------
intel_impl.c    | Naïve Intel reference implementation <sup>[1]</sup>
bogdanov_impl.c | Implementation as described by Bogdanov et al.<sup>[2]</sup>
luke_impl.c     | Implementation as per an internal reference point
c_aestest.c     | My own implementations written using Intrinsics
asm_aestest.c   | An implementation written directly in assembly

[1] - https://software.intel.com/en-us/articles/intel-advanced-encryption-standard-aes-instructions-set  
[2] - https://eprint.iacr.org/2015/795

---

## Building

The project is written in a mix of C and C++ and includes a Makefile that
*should* compile the code across any platform as long as GCC or clang is
present.

---

## Notes

- The assembly implementation is omitted when compiling on a Windows
platform as I suspect it's not compatible with MASM.
- There are plenty of improvements to be made but this framework has
 served it's and I didn't need it to be perfect.
