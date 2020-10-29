//
// Created by yawkat on 7/29/18.
//

#include <stdlib.h>
#include "kiss_fft130/kiss_fft.h"

#ifndef LIGHTPACK_CLIENT_SPECTRUM_H
#define LIGHTPACK_CLIENT_SPECTRUM_H

void spectrum_on_buffer(kiss_fft_cpx *freq, size_t n);

#endif //LIGHTPACK_CLIENT_SPECTRUM_H
