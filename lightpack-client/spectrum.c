//
// Created by yawkat on 7/29/18.
//

#include "spectrum.h"
#include "send.h"
#include <sys/param.h>

#define out_count 203
//#define out_count 97

#define DYNAMIC_FACTOR_MUL .001
#define LOG
#define LOG_FACTOR 10
#define FADE

static double averaged[out_count];

static double dynamic_factor = 1;

static inline double_t step(double_t x) {
    return x * x * x;
}

static inline double_t step_inv(double_t y) {
    return cbrt(y);
}

void spectrum_on_buffer(kiss_fft_cpx *freq, size_t n) {
#ifdef LOG
    double end = step_inv(n);
#else
    double end = n;
#endif

    double max = 0;

    double last = 0;
    for (int i = 0; i < out_count; i++) {
        double x = i * end / out_count;
#ifdef LOG
        double y = step(x);
#else
        double y = x;
#endif
        double sum = 0;
        int summed = 0;
        for (int j = (int) last; j <= y; j++) {
            sum += sqrt(((double_t) freq[j].r * freq[j].r + (double_t) freq[j].i * freq[j].i) / n / n * 100) * dynamic_factor;
            summed++;
        }
        //printf("%lf-%lf %lf / %i\n", last, y, sum, summed);
        double av = sum;
        max = MAX(av, max);
#ifdef FADE
        averaged[i] = MAX(av, averaged[i] / 3);
#else
        averaged[i] = av;
#endif
        last = y;
    }

    if (max > 1) {
        dynamic_factor = MAX(0.5, dynamic_factor - dynamic_factor * (DYNAMIC_FACTOR_MUL * max));
    } else if (max != 0) {
        dynamic_factor = dynamic_factor + dynamic_factor * (DYNAMIC_FACTOR_MUL / max);
    }
    printf("dynamic_factor: %f max: %f\n", dynamic_factor, max);

    char msg[out_count + 4];
    strcpy(msg, "fhv:");
    for (int i = 0; i < out_count; i++) {
        double_t sum = MIN(1, averaged[i]);
        //rgb col = hsv2rgb((hsv) {i % 360, 1, abs / 2});
        msg[i + 4] = ((char) (16.0 * i / out_count) << 4) | (char) (sum * 16);
    }
    SEND(msg);
}