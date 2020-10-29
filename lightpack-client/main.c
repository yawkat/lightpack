#include <pulse/simple.h>
#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <stdnoreturn.h>
#include "kiss_fft130/kiss_fftr.h"
#include "send.h"

//#define SCROLL
#define SPECTRUM

#ifdef SCROLL
#include "scroll.h"
#endif
#ifdef SPECTRUM

#include "spectrum.h"

#endif

#define BUFSIZE 2000
#define PACKETS_PER_SECOND 20
#define PACKET_MILLIS (1000/PACKETS_PER_SECOND)
#define SAMPLING_FREQ 44100

#define MIN_FREQ_HZ 100
#define MAX_FREQ_HZ 10000

#define FREQ_START(sample_count) (MIN_FREQ_HZ * (sample_count) / SAMPLING_FREQ)
#define FREQ_END(sample_count) (MAX_FREQ_HZ * (sample_count) / SAMPLING_FREQ)
#define FREQ_COUNT(sample_count) (FREQ_END(sample_count) - FREQ_START(sample_count))

static pa_simple *s;

static int64_t time_now();

static struct buffer {
    int16_t *buffer;
    size_t buffer_size;
    size_t buffer_capacity;
};

static struct buffer buffer_create(void) {
    return (struct buffer) {NULL, 0, 0};
}

static void buffer_push(struct buffer *buf, int16_t *data, size_t n) {
    size_t needed_size = buf->buffer_size + n;
    if (needed_size > buf->buffer_capacity) {
        buf->buffer = reallocarray(buf->buffer, MAX(buf->buffer_capacity * 2, needed_size), sizeof(int16_t));
    }
    memcpy(&buf->buffer[buf->buffer_size], data, n * sizeof(int16_t));
    buf->buffer_size += n;
}

static inline void buffer_clear(struct buffer *buf) {
    buf->buffer_size = 0;
}

static void read_some(struct buffer *buf) {
    int16_t read[BUFSIZE];
    int err;
    if (pa_simple_read(s, read, sizeof(read), &err) < 0) {
        fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(err));
        exit(1);
    }
    buffer_push(buf, read, BUFSIZE);
}

static inline double_t window(int n, int count) {
    double v = sin(M_PI * n / (count - 1));
    return v * v;
}

noreturn int main() {
    int err;
    static const pa_sample_spec ss = {
            .format = PA_SAMPLE_S16LE,
            .rate = SAMPLING_FREQ,
            .channels = 1
    };
    pa_buffer_attr buf_attr;
    buf_attr.maxlength = (uint32_t) -1;
    buf_attr.fragsize = BUFSIZE * sizeof(int16_t);
    buf_attr.tlength = (uint32_t) -1; // not relevant for RECORD
    buf_attr.prebuf = (uint32_t) -1; // not relevant for RECORD
    buf_attr.minreq = (uint32_t) -1; // not relevant for RECORD
    if (!(s = pa_simple_new(NULL, "lightpack-client", PA_STREAM_RECORD,
                            "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor",
            //                "alsa_input.pci-0000_00_1b.0.analog-stereo",
                            "lightpack-client", &ss, NULL, &buf_attr, &err))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(err));
        return 1;
    }

    lightpack_send_start();

    {
        char msg[203 + 4];
        memset(msg, 0, sizeof(msg));
        strcpy(msg, "fhv:");
        SEND(msg);
    }

    struct buffer buffer = buffer_create();

    int64_t deadline = 0;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        read_some(&buffer);

        int64_t now = time_now();
        if (now < deadline) {
            continue;
        }

        size_t sample_count = buffer.buffer_size;
        printf("Tick %li buffer size %zu (off by %li ms for deadline freq of %i ms)\n", now, sample_count,
               now - deadline,
               PACKET_MILLIS);
        deadline = now + PACKET_MILLIS;

        kiss_fft_scalar values[sample_count];
        for (int i = 0; i < sample_count; i++) {
            values[i] = (float) (buffer.buffer[i] / (float) (1 << 16));
        }
        kiss_fftr_cfg cfg = kiss_fftr_alloc((int) sample_count, 0, 0, 0);
        kiss_fft_cpx freq[sample_count];
        kiss_fftr(cfg, values, freq);
        for (int i = 0; i < sample_count; i++) {
            double_t f = (float) i * SAMPLING_FREQ / sample_count;
            if (f > 1000) break;
            //printf("%i: %f Hz: %f\n", i, f, sqrt(freq[i].r * freq[i].r + freq[i].i * freq[i].i));
        }

        printf("Samples: %zu - %zu\n", FREQ_START(sample_count), FREQ_END(sample_count));
#ifdef SCROLL
        scroll_on_buffer(&freq[FREQ_START(buffer.buffer_size)], FREQ_COUNT(buffer.buffer_size));
#endif
#ifdef SPECTRUM
        spectrum_on_buffer(&freq[FREQ_START(sample_count)], FREQ_COUNT(sample_count));
#endif

        buffer_clear(&buffer);
        //printf("Tick took %li ms\n", time_now() - now);
    }
#pragma clang diagnostic pop
}

int64_t time_now() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    int64_t now = spec.tv_sec * 1000 + spec.tv_nsec / 1000000;
    return now;
}
