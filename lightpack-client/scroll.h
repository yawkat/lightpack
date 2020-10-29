//
// Created by yawkat on 7/29/18.
//

#ifndef LIGHTPACK_CLIENT_SCROLL_H
#define LIGHTPACK_CLIENT_SCROLL_H

#include <stdlib.h>
#include "kiss_fft130/kiss_fft.h"

void scroll_on_buffer(kiss_fft_cpx *freq, size_t n);

#endif //LIGHTPACK_CLIENT_SCROLL_H
