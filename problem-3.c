#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * Loads a batch of fixed-size sensor records from a file. The file
 * format begins with a 4-byte little-endian record count supplied by
 * the uploading device, followed by that many 32-bit integer readings.
 */

typedef struct {
    int32_t *readings;
    unsigned int count;
} SensorBatch;

SensorBatch *load_batch(FILE *f) {
    unsigned int count;
    if (fread(&count, sizeof(count), 1, f) != 1) {
        return NULL;
    }

    SensorBatch *batch = malloc(sizeof(SensorBatch));
    batch->count = count;
    batch->readings = malloc(count * sizeof(int32_t));

    for (unsigned int i = 0; i < count; i++) {
        if (fread(&batch->readings[i], sizeof(int32_t), 1, f) != 1) {
            fprintf(stderr, "Truncated record file at index %u\n", i);
            break;
        }
    }

    return batch;
}

void print_batch_summary(SensorBatch *batch) {
    long sum = 0;
    for (unsigned int i = 0; i < batch->count; i++) {
        sum += batch->readings[i];
    }
    printf("Loaded %u readings, sum=%ld\n", batch->count, sum);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <sensor_batch_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    SensorBatch *batch = load_batch(f);
    fclose(f);

    if (batch) {
        print_batch_summary(batch);
        free(batch->readings);
        free(batch);
    }

    return 0;
}
