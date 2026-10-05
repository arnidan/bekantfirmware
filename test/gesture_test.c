/*
 * Host test for button gestures:
 *   clang -Wall -o /tmp/gesture_test test/gesture_test.c && /tmp/gesture_test
 */
#include <assert.h>
#include <stdio.h>
#include "../bekantfirmware.X/btn/gesture.c"

// Feed the same button state for n ticks (50 ms each),
// return the last reported gesture.
static INPUT_t feed(bool up, bool down, int n) {
    INPUT_t in = INPUT_IDLE;
    for (int i = 0; i < n; i++) {
        in = btn_gesture(up, down);
    }
    return in;
}

// Feed ticks and assert that the gesture is reported at some tick.
static bool saw(bool up, bool down, int n, INPUT_t want) {
    bool seen = false;
    for (int i = 0; i < n; i++) {
        seen |= btn_gesture(up, down) == want;
    }
    return seen;
}

static void reset(void) {
    feed(false, false, 20);
}

int main(void) {
    // Hold moves after the click limit and stops on release
    reset();
    assert(feed(true, false, 2) == INPUT_PRESSED);
    assert(feed(true, false, 10) == INPUT_UP);
    assert(feed(false, false, 1) == INPUT_IDLE);

    // Single click does nothing
    reset();
    assert(!saw(true, false, 2, INPUT_UP));
    assert(!saw(false, false, 20, INPUT_MEM_UP));

    // Double click up / down goes to memory
    reset();
    feed(true, false, 2);
    feed(false, false, 3);
    feed(true, false, 2);
    assert(feed(false, false, 1) == INPUT_MEM_UP);
    reset();
    feed(false, true, 2);
    feed(false, false, 3);
    feed(false, true, 2);
    assert(feed(false, false, 1) == INPUT_MEM_DOWN);

    // Pause too long between clicks: no memory move
    reset();
    feed(true, false, 2);
    feed(false, false, 10);
    feed(true, false, 2);
    assert(feed(false, false, 1) == INPUT_IDLE);

    // Click + hold 3 s saves, only once, and never moves
    reset();
    feed(false, true, 2);
    feed(false, false, 3);
    assert(!saw(false, true, 60, INPUT_SAVE_DOWN));
    assert(feed(false, true, 1) == INPUT_SAVE_DOWN);
    assert(!saw(false, true, 100, INPUT_DOWN));
    assert(feed(false, false, 1) == INPUT_IDLE);
    reset();
    feed(true, false, 2);
    feed(false, false, 3);
    assert(saw(true, false, 61, INPUT_SAVE_UP));

    // Click + hold released before 3 s: nothing
    reset();
    feed(true, false, 2);
    feed(false, false, 3);
    assert(!saw(true, false, 30, INPUT_UP));
    assert(feed(false, false, 1) == INPUT_IDLE);

    // Click up, then click down: not a double click
    reset();
    feed(true, false, 2);
    feed(false, false, 3);
    feed(false, true, 2);
    assert(feed(false, false, 1) == INPUT_IDLE);

    // Both buttons: no movement until released
    reset();
    feed(true, false, 1);
    assert(!saw(true, true, 20, INPUT_UP));
    assert(feed(true, false, 20) == INPUT_PRESSED);
    assert(feed(false, false, 1) == INPUT_IDLE);

    // Any press reports PRESSED right away (cancels a memory move)
    reset();
    assert(feed(false, true, 1) == INPUT_PRESSED);

    puts("gesture tests passed");
    return 0;
}
