//
// Created by yawkat on 7/29/18.
//

#include <math.h>
#include <stdio.h>
#include "scroll.h"
#include "hsv.h"
#include "send.h"

//static int angle = 0;

void scroll_on_buffer(kiss_fft_cpx *freq, size_t n) {
    double_t vol = 0;
    double_t angle = 0;
    size_t bins = n / 8;
    for (int i = 0; i < bins; i++) {
        double_t abs = (double_t) freq[i].r * freq[i].r + (double_t) freq[i].i * freq[i].i;
        abs = abs * abs;
        vol += abs;
        angle += abs * i;
    }
    vol /= bins;

    char msg[5];
    msg[0] = 'c';
    msg[1] = ':';
    printf("s %lf\n", vol);
    if (vol > 1) vol = 1;
    rgb col = hsv2rgb((hsv) {(int) (angle * 360 / (bins * bins)) % 360, 1, vol / 2});
    msg[2] = (char) (col.r * 255);
    msg[3] = (char) (col.g * 255);
    msg[4] = (char) (col.b * 255);
    SEND(msg);
    //angle = (angle + 1) % 360;
}