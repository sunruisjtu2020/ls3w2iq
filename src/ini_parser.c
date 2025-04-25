#include "ini_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_ini(const char *filename, IniConfig *config) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    perror("Failed to open ini file");
    return -1;
  }

  char line[256];
  char section[32] = "";

  while (fgets(line, sizeof(line), f)) {

    line[strcspn(line, "\r\n")] = 0;

    if (line[0] == '[') {
      sscanf(line, "[%31[^]]", section);
    } else {
      char key[64], value[64];
      if (sscanf(line, "%[^=]=%s", key, value) == 2) {
        if (strcmp(section, "config") == 0) {
          if (strcmp(key, "SMP") == 0)
            config->sample_rate = atoi(value);
          else if (strcmp(key, "QUA") == 0)
            config->quantization = atoi(value);
          else if (strcmp(key, "CHN") == 0)
            config->num_channels = atoi(value);
        } else if (strcmp(section, "channel A") == 0 &&
                    strcmp(key, "CFA") == 0) {
          config->freqs[0] = atof(value);
        } else if (strcmp(section, "channel B") == 0 &&
                    strcmp(key, "CFB") == 0) {
          config->freqs[1] = atof(value);
        } else if (strcmp(section, "channel C") == 0 &&
                    strcmp(key, "CFC") == 0) {
          config->freqs[2] = atof(value);
        }
      }
    }
  }

  fclose(f);
  return 0;
}
