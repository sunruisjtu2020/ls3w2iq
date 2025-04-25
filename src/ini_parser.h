#ifndef INI_PARSER_H
#define INI_PARSER_H

typedef struct {
  int sample_rate;
  int quantization;
  int num_channels;
  float freqs[3];
} IniConfig;

int parse_ini(const char *filename, IniConfig *config);

#endif
