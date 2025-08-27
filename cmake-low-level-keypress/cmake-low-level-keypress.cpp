#include <stdio.h>
#include <iostream>
#include <chrono>
#include "interception.h"

#define LEFT_ALT 0x38
#define N3       0x04
#define F3       0x3D
#define ESC      0x01
#define INTERCEPTION_KEY_DOWN 0x00
#define INTERCEPTION_KEY_UP   0x01

using namespace std::chrono;

int main() {
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionStroke stroke;
    context = interception_create_context();
    interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    int alt_down = 0;
    int looping = 0;
    steady_clock::time_point last_send;
    InterceptionDevice keyboard_device;

    while (true) {
        if (looping) {
            auto now = steady_clock::now();
            auto elapsed_ms = duration_cast<milliseconds>(now - last_send).count();
            if (elapsed_ms >= 100) {
                InterceptionKeyStroke f3_down = { F3, INTERCEPTION_KEY_DOWN, 0 };
                InterceptionKeyStroke f3_up = { F3, INTERCEPTION_KEY_UP, 0 };
                interception_send(context, keyboard_device, (InterceptionStroke*)&f3_down, 1);
                interception_send(context, keyboard_device, (InterceptionStroke*)&f3_up, 1);
                last_send = now;
            }
        }

        unsigned long timeout_ms = 0xFFFFFFFFUL;
        if (looping) {
            auto now_timeout = steady_clock::now();
            auto elapsed_ms_timeout = duration_cast<milliseconds>(now_timeout - last_send).count();
            timeout_ms = (elapsed_ms_timeout >= 100) ? 0UL : 100UL - elapsed_ms_timeout;
        }

        device = interception_wait_with_timeout(context, timeout_ms);

        if (device > 0) {
            if (interception_receive(context, device, &stroke, 1) > 0) {
                InterceptionKeyStroke* keystroke = (InterceptionKeyStroke*)&stroke;

                if (keystroke->code == LEFT_ALT) {
                    alt_down = !(keystroke->state & INTERCEPTION_KEY_UP);
                }

                if (looping && keystroke->code == ESC && !(keystroke->state & INTERCEPTION_KEY_UP)) {
                    looping = 0;
                }

                if (!looping && alt_down && keystroke->code == N3 && !(keystroke->state & INTERCEPTION_KEY_UP)) {
                    // Solta ALT temporariamente
                    InterceptionKeyStroke alt_up = { LEFT_ALT, INTERCEPTION_KEY_UP, 0 };
                    interception_send(context, device, (InterceptionStroke*)&alt_up, 1);

                    // Envia primeiro F3
                    InterceptionKeyStroke f3_down = { F3, INTERCEPTION_KEY_DOWN, 0 };
                    InterceptionKeyStroke f3_up = { F3, INTERCEPTION_KEY_UP, 0 };
                    interception_send(context, device, (InterceptionStroke*)&f3_down, 1);
                    interception_send(context, device, (InterceptionStroke*)&f3_up, 1);

                    // Restaura ALT se estava pressionado
                    InterceptionKeyStroke alt_down_event = { LEFT_ALT, INTERCEPTION_KEY_DOWN, 0 };
                    interception_send(context, device, (InterceptionStroke*)&alt_down_event, 1);

                    looping = 1;
                    keyboard_device = device;
                    last_send = steady_clock::now();
                    continue; // Ignora o "3" real
                }

                interception_send(context, device, &stroke, 1);
            }
        }
    }

    interception_destroy_context(context);
    return 0;
}
