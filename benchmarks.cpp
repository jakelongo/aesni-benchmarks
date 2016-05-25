// Author       : Jake Longo
// Name         : benchmarks.cpp
// Description  : Benchmarking brute-force code for AES using AES-NI

#ifdef _WIN32
#include <Windows.h>
#include <intrin.h>
#define _ALIGN_ALLOC_(_ptr, _bnd, _size) _ptr = (uint8_t*) _aligned_malloc(_size, _bnd)
#define _ALIGN_FREE_(_ptr) _aligned_free(_ptr)
#define _GETHOSTNAME(_buff, _buffLen) GetComputerName((LPWSTR) _buff, (LPDWORD) &(_buffLen))
#else /* (__linux) || defined (__APPLE__) */
#include <unistd.h>
#define _ALIGN_ALLOC_(_ptr, _bnd, _size) posix_memalign((void**)&_ptr, _bnd, _size)
#define _ALIGN_FREE_(_ptr) free(_ptr)
#define _GETHOSTNAME(_buff, _buffLen) gethostname(_buff, _buffLen)
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>

#include <vector>
#include <random>

#ifndef _WIN32
#include "asm_aestest.h"
#endif


#include "bogdanov_impl.h"
#include "intel_impl.h"
#include "luke_impl.h"
#include "c_aestest.h"

// Defines for 'number of keys' in terms of bytes
#define KILOBYTES           64
#define MEGABYTES           KILOBYTES*1024
#define GIGABYTES           MEGABYTES*1024

#define NUMBER_OF_KEYS      32*KILOBYTES
#define NUMBER_OF_REPEATS   10000


// Get cycle counter from rdtsc register
#ifdef _WIN32
uint64_t getclks(void) {
	return __rdtsc();
}
#else
__inline uint64_t getclks(void) {
	uint32_t lo, hi;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}
#endif

// Auxiliary function to print out groups of 16 bytes in hex
void prettyprint(uint8_t *data, uint32_t bytes) {

  for (uint32_t cntr = 0; cntr < bytes; ++cntr) {

    if (0 == (cntr % 16))
      std::cout << std::endl;

    std::cout << "0x" << std::setfill('0') << std::uppercase
              << std::setw(2) << std::hex << (int)data[cntr] << " ";

  }

  std::cout << std::endl;

}

// Print the benchmark tool info
void printInfo(void){

  // get the hostname of the machine
  char     hostname[512];
  uint32_t hostnameLen = 512;

  _GETHOSTNAME(hostname, hostnameLen);

  // Print the test information
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "Running on hostname: " << hostname             << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "Benchmark information:"                        << std::endl;
  std::cout << "Number of repeats = " << NUMBER_OF_REPEATS     << std::endl;
  std::cout << "Number of keys    = " << NUMBER_OF_KEYS        << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

// used for checking expected output/correctness
void simpleEncrypt(void(*encFunction)(uint8_t*, uint8_t*, uint8_t*),
                   uint8_t *keys, uint8_t *data, uint8_t *cips, uint32_t packSize) {
  for (uint32_t cntr = 0; cntr < NUMBER_OF_KEYS; cntr += packSize) {
    (*encFunction)(&(keys[cntr*16]), data, &(cips[cntr*16]));
  }
}

void runBenchmarks(void(*encFunction)(uint8_t*, uint8_t*, uint8_t*),
                   uint8_t *keys, uint8_t *data, uint8_t *cips,
                   uint8_t *comp, uint32_t packSize, char const *descr) {

  uint64_t tck_start, tck_stop;
  clock_t  clk_start, clk_stop;

  double  elapsed    = 0.0f;
  double  throughput = 0.0f;
  double  kps        = 0.0f;

  std::cout << "============================================="  << std::endl;
  std::cout << "Test: "   << descr                              << std::endl;

  simpleEncrypt(encFunction, keys, data, cips, packSize);

  if (0 != std::memcmp(cips, comp, NUMBER_OF_KEYS*16)) {
    std::cout << "Equality test: FAIL -- not doing what it     "  << std::endl;
    std::cout << "                       says on the tin!      "  << std::endl;
    std::cout << "============================================="  << std::endl;
    return;
  }
  std::cout << "Equality test: PASS!" << std::endl;
  std::memset(cips, 0x00, NUMBER_OF_KEYS*16);

  // Perform encryptions for some number of repetitions
  for (uint32_t rptCntr = 0; rptCntr < NUMBER_OF_REPEATS; rptCntr++) {
    // start recording
    clk_start = clock();
    tck_start = getclks();

    // You could call this 'the business end' of things
    for (uint32_t cntr = 0; cntr < NUMBER_OF_KEYS; cntr += packSize) {
      (*encFunction)(&(keys[cntr*16]), data, &(cips[cntr*16]));
    }

    // stop recording
    tck_stop   = getclks();
    clk_stop   = clock();

    // Tally up the stats
    elapsed    += (double)(clk_stop - clk_start) / CLOCKS_PER_SEC;
    throughput += (double)(tck_stop - tck_start) / (NUMBER_OF_KEYS*16);

    // Clear the memory space to avoid overwrite caching
    memset(cips, 0x00, NUMBER_OF_KEYS * 16);
  }

  // compute on findings
  elapsed    = elapsed    / NUMBER_OF_REPEATS;
  throughput = throughput / NUMBER_OF_REPEATS;
  kps        = log( NUMBER_OF_KEYS / elapsed ) / log(2);

  // Spill the results to the console
  std::cout << "Time: "   << std::setprecision(4) << elapsed    << " seconds"     << std::endl;
  std::cout << "Thpt: "   << std::setprecision(4) << throughput << " cycles/byte" << std::endl;
  std::cout << "Keys: 2^" << std::setprecision(4) << kps        << " keys/sec"    << std::endl;
  std::cout << "============================================="                    << std::endl;
}

int main(int argc, char* argv[]) {

  alignas(16) uint8_t data[32]; // plaintext data buffer

  uint8_t *keys; // pointer to key        buffer
  uint8_t *cips; // pointer to ciphertext buffer
  uint8_t *comp; // pointer to comparison buffer

  // Set an RNG up for generating our data
  std::seed_seq seed{0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xC0, 0xDE}; // <-- Change if you want real random stuff
  std::mt19937 mt_rand(seed);

  // Allocate the memory pools
  _ALIGN_ALLOC_(keys, 32, NUMBER_OF_KEYS * 16);
  _ALIGN_ALLOC_(cips, 32, NUMBER_OF_KEYS * 16);
  _ALIGN_ALLOC_(comp, 32, NUMBER_OF_KEYS * 16);

  // Check we didn't have a problem allocating the memory
  if ( (NULL == keys) || (NULL == cips) || (NULL == comp) ) {
    std::cout << "Unable to allocate memory buffers! Bailing out" << std::endl;
    return -1;
  }

  // print the test preamble
  printInfo();

  // init a seed
  std::memset(cips, 0x00, NUMBER_OF_KEYS*16);

  // set the keys
  for (uint32_t cntr = 0; cntr < (NUMBER_OF_KEYS*16)/4; ++cntr) {
    ((uint32_t*)keys)[cntr] = mt_rand();
  }

  // set data
  for (uint32_t cntr = 0; cntr < 16/4; ++cntr) {
    ((uint32_t*)data)[cntr] = mt_rand();
  }

  // Fill comparison buffer with a known working implementation
  simpleEncrypt(c_intel_benchmark, keys, data, comp, 1);

  // Benchmark codes!
  runBenchmarks(&c_intel_benchmark,      keys, data, cips, comp, 1, "c_intel");
  runBenchmarks(&c_intel_interleaved_4x, keys, data, cips, comp, 4, "c_intel_interleaved_4x");

  runBenchmarks(&c_luke_impl,            keys, data, cips, comp, 4, "c_luke_4x");

#ifndef _WIN32
  runBenchmarks(&asm_unrolled_4x,        keys, data, cips, comp, 4, "asm_unrolled_4x");
#endif

 runBenchmarks(&c_unrolled_4x,          keys, data, cips, comp, 4, "c_unrolled_4x");
 runBenchmarks(&c_unrolled_8x,          keys, data, cips, comp, 8, "c_unrolled_8x");

 runBenchmarks(&c_bogdanov_4x,          keys, data, cips, comp, 4, "c_bogdanov_4x");
 runBenchmarks(&c_opt_unrolled_8x,      keys, data, cips, comp, 8, "c_opt_unrolled_8x");

  // House keeping
  if (NULL != keys)
	_ALIGN_FREE_(keys);

  if (NULL != cips)
	_ALIGN_FREE_(cips);

  if (NULL != comp)
	_ALIGN_FREE_(comp);

  return 0;

}