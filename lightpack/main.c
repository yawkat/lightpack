#include <avr/io.h>
#include <string.h>
#include "light_ws2812.h"
#include "uart.h"
#include "wifi_config.h"
#include "fast_hsv2rgb.h"

#if F_CPU == 8000000
#define PIXELS 97
#elif F_CPU == 16000000
#define PIXELS 203
#endif

struct cRGB led[PIXELS];
struct cRGB next = (struct cRGB) {0, 0, 0};

void init_state();

static void write_uart(const char const *msg) {
    uint8_t msg_len = strlen(msg);
    for (uint8_t i = 0; i < msg_len; i++) {
        USART_SendByte(msg[i]);
    }
}

static void read_uart_until(char *msg) {
    uint8_t msg_len = strlen(msg);
    uint8_t ix = 0;
    while (ix < msg_len) {
        if (USART_ReceiveByte() == msg[ix]) {
            ix++;
        } else {
            ix = 0;
        }
    }
}

static inline void tick(void) {
    memmove(&led[1], &led[0], (PIXELS - 1) * sizeof(struct cRGB));
    led[0] = next;
    next = (struct cRGB) {0, 0, 0};
}

static void status(uint8_t s) {
    for (int i = 0; i < PIXELS; i++) {
        led[i].r = 0;
        led[i].g = 0;
        led[i].b = i < s ? 10 : 0;
    }
    ws2812_setleds(led, PIXELS);
}

int main() {
    for (int i = 0; i < PIXELS; i++) {
        led[i].r = 255;
        led[i].g = 255;
        led[i].b = 255;
    }
    ws2812_setleds(led, PIXELS);
    return 0;

    status(0);
    USART_Init();
    status(1);

    write_uart("AT+RST\r\n");
    read_uart_until("ready\r\n");

    status(2);
    write_uart("AT+CWMODE=3\r\n");
    read_uart_until("OK\r\n");
    status(3);
    write_uart("AT+CWJAP=");
    write_uart(CON_STR);
    write_uart("\r\n");
    read_uart_until("OK\r\n");
    status(4);
    write_uart("AT+CIPMUX=1\r\n");
    read_uart_until("OK\r\n");
    status(5);
    write_uart("AT+CIPSTART=0,\"UDP\",\"0\",5000,5000\r\n");
    read_uart_until("OK\r\n");
    status(6);

    DDRD |= _BV(ws2812_pin);

    init_state();

    while (1) {
        read_uart_until("+IPD,0,");
        size_t n = 0;
        while (1) {
            uint8_t c = USART_ReceiveByte();
            if (c == ':') break;
            n = 10 * n + (c - '0');
        }
        uint8_t buf[n];
        for (size_t i = 0; i < n; i++) {
            buf[i] = USART_ReceiveByte();
        }
        if (n >= 5 && memcmp(buf, "c:", 2) == 0) {
            next.r = buf[2];
            next.g = buf[3];
            next.b = buf[4];
            tick();
        } else if (n >= 5 && memcmp(buf, "full:", 5) == 0) {
            for (int i = 0; i < (n - 5) / 3 && i < PIXELS; i++) {
                led[i].r = buf[i * 3 + 5];
                led[i].g = buf[i * 3 + 6];
                led[i].b = buf[i * 3 + 7];
            }
        } else if (n >= 4 && memcmp(buf, "fhv:", 4) == 0) {
            for (int i = 0; i < n - 4 && i < PIXELS; i++) {
                uint8_t enc = buf[i + 4];
                uint8_t h = enc & (uint8_t) 0xf0;
                uint8_t v = enc << 4;
                fast_hsv2rgb_8bit(h, 255, v, &led[i].r, &led[i].g, &led[i].b);
            }
        } else {
            init_state();
        }
        ws2812_setleds(led, PIXELS);
    }
}

void init_state() {
    for (int i = 0; i < PIXELS; i++) {
        led[i].r = 0;
        led[i].g = 10;
        led[i].b = 0;
    }
    ws2812_setleds(led, PIXELS);
}