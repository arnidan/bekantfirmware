#ifndef GESTURE_H
#define GESTURE_H

#include <stdbool.h>
#include <stdint.h>
#include "btn.h"

/**
 * @param up whether the up button is pressed (debounced)
 * @param down whether the down button is pressed (debounced)
 * @return current input gesture
 */
INPUT_t btn_gesture(bool up, bool down);

#endif /* GESTURE_H */
