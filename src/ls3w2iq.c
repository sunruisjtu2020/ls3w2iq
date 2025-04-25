#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <math.h>

#include "ini_parser.h"
#include "ls3w_decode.h"

#define MAX_BLOCK 100000
const char* channel_suffix[3] = {"_A.iq", "_B.iq", "_C.iq"};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s -i File_001.LS3W -o File_001\n", argv[0]);
        return 1;
    }

    const char *in_path = NULL, *base_name = NULL;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) in_path = argv[++i];
        else if (!strcmp(argv[i], "-o")) base_name = argv[++i];
    }

    if (!in_path || !base_name) {
        fprintf(stderr, "Missing input or output base.\n");
        return 1;
    }

    char ini_path[512];
    strncpy(ini_path, in_path, sizeof(ini_path));
    char* ext = strrchr(ini_path, '.');
    if (ext) strcpy(ext, ".ini");
    else strcat(ini_path, ".ini");

    IniConfig config;
    if (parse_ini(ini_path, &config) != 0) {
        fprintf(stderr, "Failed to parse INI file.\n");
        return 1;
    }

    printf("Sample Rate: %d Hz\n", config.sample_rate);
    printf("Quantization: %d-bit\n", config.quantization);
    printf("Channels: %d\n", config.num_channels);

    FILE* fin = fopen(in_path, "rb");
    if (!fin) {
        perror("Failed to open LS3W file");
        return 1;
    }

    struct stat st;
    if (stat(in_path, &st) != 0) {
        perror("stat failed");
        fclose(fin);
        return 1;
    }
    size_t total_regs = st.st_size / sizeof(uint64_t);
    size_t processed_regs = 0;

    FILE* fout[3] = {NULL};
    for (int ch = 0; ch < config.num_channels && ch < 3; ch++) {
        char out_path[512];
        snprintf(out_path, sizeof(out_path), "%s%s", base_name, channel_suffix[ch]);
        fout[ch] = fopen(out_path, "wb");
        if (!fout[ch]) {
            perror("Failed to open output file");
            fclose(fin);
            return 1;
        }
        printf("Writing: %s (%.2f MHz)\n", out_path, config.freqs[ch]);
    }

    int samples_per_reg = get_samples_per_register(config.quantization, config.num_channels);
    uint64_t* regs = malloc(sizeof(uint64_t) * MAX_BLOCK);
    uint8_t* i_buf = malloc(sizeof(uint8_t) * samples_per_reg * config.num_channels);
    uint8_t* q_buf = malloc(sizeof(uint8_t) * samples_per_reg * config.num_channels);
    uint8_t* out_buf = malloc(2 * samples_per_reg);

    while (!feof(fin)) {
        size_t read_regs = fread(regs, sizeof(uint64_t), MAX_BLOCK, fin);
        if (read_regs == 0) break;
        processed_regs += read_regs;

        for (size_t i = 0; i < read_regs; i++) {
            int count = decode_register(regs[i], config.quantization, config.num_channels,
                                        (int8_t*)i_buf, (int8_t*)q_buf);

            int ch_samples = count / config.num_channels;

            for (int ch = 0; ch < config.num_channels && ch < 3; ch++) {
                if (!fout[ch]) continue;
                for (int s = 0; s < ch_samples; s++) {
                    int idx = s * config.num_channels + ch;
                    out_buf[2 * s]     = i_buf[idx];
                    out_buf[2 * s + 1] = q_buf[idx];
                }
                fwrite(out_buf, sizeof(uint8_t), 2 * ch_samples, fout[ch]);
            }
        }

        float progress = 100.0f * processed_regs / total_regs;
        printf("\rProcessing: %.1f%% (%zu / %zu regs)", progress, processed_regs, total_regs);
        fflush(stdout);
    }

    printf("\nDone.\n");

    fclose(fin);
    for (int i = 0; i < 3; i++)
        if (fout[i]) fclose(fout[i]);
    free(regs); free(i_buf); free(q_buf); free(out_buf);

    return 0;
}
