#ifndef LS3W_DECODE_H
#define LS3W_DECODE_H

#include <stdint.h>

int decode_register(uint64_t reg, int quantization, int num_channels,
                    int8_t *iData,
                    int8_t *qData
);

int get_samples_per_register(int quantization, int num_channels);

#endif
