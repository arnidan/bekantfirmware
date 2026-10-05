
#ifndef BTN_H
#define BTN_H

typedef enum {
    INPUT_IDLE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_MEM_UP,
    INPUT_MEM_DOWN,
    INPUT_SAVE_UP,
    INPUT_SAVE_DOWN,
    INPUT_PRESSED, // a button is down but the gesture is not known yet
} INPUT_t;

void btn_init(void);
void btn_timer(void);
extern void (*btn_report_gesture)(INPUT_t gesture);

#endif /* BTN_H */
