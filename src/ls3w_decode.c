#include "ls3w_decode.h"

int get_samples_per_register(int quantization, int num_channels) {
  return 64 / (2 * quantization * num_channels);
}

int decode_register(uint64_t reg, int quantization, int num_channels,
                    int8_t *iData, int8_t *qData) {
  int bits_per_sample = 2 * quantization * num_channels;
  int samples_per_reg = 64 / bits_per_sample;
  int spare_bits = 64 - samples_per_reg * bits_per_sample;

  int sample_counter = 0;

  for (int sample_idx = 0; sample_idx < samples_per_reg; sample_idx++) {
    for (int ch = 0; ch < num_channels; ch++) {
      int bit_offset =
          spare_bits + sample_idx * bits_per_sample + ch * 2 * quantization;

      uint8_t ibits =
          (reg >> (64 - bit_offset - quantization)) & ((1 << quantization) - 1);
      uint8_t qbits = (reg >> (64 - bit_offset - 2 * quantization)) &
                      ((1 << quantization) - 1);

      int8_t i = 0, q = 0;

      switch (quantization) {
      case 1: // 1-bit
        i = (ibits == 0) ? 1 : -1;
        q = (qbits == 0) ? 1 : -1;
        break;
      case 2: // 2-bit → [00,01,10,11] → [1,2,-2,-1]
      {
        int val_i = (ibits & 0x2) >> 1;
        int val_q = (qbits & 0x2) >> 1;
        i = 1 - 3 * val_i + (ibits & 0x1);
        q = 1 - 3 * val_q + (qbits & 0x1);
      } break;
      case 3: // 3-bit → [000...111] → [1..4,-4..-1]
      {
        int vi = (ibits & 0x4) >> 2;
        int i_map = 1 - 5 * vi + 2 * ((ibits & 0x2) >> 1) + (ibits & 0x1);
        int vq = (qbits & 0x4) >> 2;
        int q_map = 1 - 5 * vq + 2 * ((qbits & 0x2) >> 1) + (qbits & 0x1);
        i = (int8_t)i_map;
        q = (int8_t)q_map;
      } break;
      default:
        return -1; // unsupported quantization
      }

      iData[sample_counter] = i;
      qData[sample_counter] = q;
      sample_counter++;
    }
  }

  return sample_counter;
}
