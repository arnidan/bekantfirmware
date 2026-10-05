/**
 * Button gestures, independent of the hardware so it can be tested on a host.
 *
 * Hold a button to move. Double click a button to move to its memory
 * position. Click, then hold the same button to save its memory position.
 */

#include "gesture.h"

// Called at debounced input frequency: 20 Hz, so one tick is 50 ms.
// A shorter press is a click; a longer first press starts manual movement.
#define CLICK_MAX_TICKS 6 // 300 ms
// Longest pause between the two clicks of a double click.
#define DOUBLE_GAP_TICKS 8 // 400 ms
// How long to hold the second press to save.
#define SAVE_HOLD_TICKS 60 // 3 sec

typedef enum {
    G_IDLE,
    G_PRESS1, // first press, not yet known whether click or hold
    G_MOVE, // first press held: manual movement
    G_GAP, // released after a click, waiting for a second press
    G_PRESS2, // second press: click moves to memory, hold saves
    G_WAIT, // gesture finished or aborted, wait until all released
} Gesture_state_t;

static Gesture_state_t state = G_IDLE;
static bool btn_is_up; // which button the current gesture uses
static uint8_t ticks;

INPUT_t btn_gesture(bool up, bool down) {
    bool same = btn_is_up ? up : down;

    if (up && down) {
        state = G_WAIT; // both buttons pressed: no gesture
    }

    switch (state) {
        case G_IDLE:
            if (up || down) {
                btn_is_up = up;
                ticks = 0;
                state = G_PRESS1;
                return INPUT_PRESSED;
            }
            return INPUT_IDLE;

        case G_PRESS1:
            if (!same) {
                ticks = 0;
                state = G_GAP;
                return INPUT_IDLE;
            }
            if (++ticks >= CLICK_MAX_TICKS) {
                state = G_MOVE;
                return btn_is_up ? INPUT_UP : INPUT_DOWN;
            }
            return INPUT_PRESSED;

        case G_MOVE:
            if (same) {
                return btn_is_up ? INPUT_UP : INPUT_DOWN;
            }
            state = G_IDLE;
            return INPUT_IDLE;

        case G_GAP:
            if (same) {
                ticks = 0;
                state = G_PRESS2;
                return INPUT_PRESSED;
            }
            if (up || down) { // the other button: start over with it
                btn_is_up = up;
                ticks = 0;
                state = G_PRESS1;
                return INPUT_PRESSED;
            }
            if (++ticks >= DOUBLE_GAP_TICKS) {
                state = G_IDLE;
            }
            return INPUT_IDLE;

        case G_PRESS2:
            if (!same) {
                state = G_IDLE;
                if (ticks < CLICK_MAX_TICKS) {
                    return btn_is_up ? INPUT_MEM_UP : INPUT_MEM_DOWN;
                }
                return INPUT_IDLE; // too long for a click, too short to save
            }
            if (++ticks >= SAVE_HOLD_TICKS) {
                state = G_WAIT;
                return btn_is_up ? INPUT_SAVE_UP : INPUT_SAVE_DOWN;
            }
            return INPUT_PRESSED;

        case G_WAIT:
            if (up || down) {
                return INPUT_PRESSED;
            }
            state = G_IDLE;
            return INPUT_IDLE;
    }
    return INPUT_IDLE;
}
