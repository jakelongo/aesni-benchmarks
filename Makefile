OBJS   := $(patsubst %.c,%.o,$(wildcard *.c))
OBJS   += $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS   += $(patsubst %.S,%.o,$(wildcard *.S))

.PHONY: all clean

CFLAGS   := -Ofast -maes -msse4.2 -mavx -std=c11
CPPFLAGS := -Ofast -maes -msse4.2 -mavx -std=c++11
ASFLAGS  := -masm=intel
DBGFLAGS := -g

CC  := gcc
CXX := g++

all: aesni-benchmark

aesni-benchmark: $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $^

%.o: %.S
	$(CC)  -o $@ $(CFLAGS) $(ASFLAGS) -c $^

%.o: %.c
	$(CC)  -o $@ $(CFLAGS) 	 -c $^

%.o: %.cpp
	$(CXX) -o $@ $(CPPFLAGS) -c $^

clean:
	rm -f $(OBJS)