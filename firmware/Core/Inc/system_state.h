/*
 * system_state.h
 *
 *  Created on: Apr 15, 2026
 *      Author: patry
 */
#include <stdbool.h>

#ifndef INC_SYSTEM_STATE_H_
#define INC_SYSTEM_STATE_H_

typedef struct {
    int set_temp;
    float temp;
    bool relay;
} SystemState_t;

typedef enum {
    IR_EVENT_TEMP_CHANGE,
    IR_EVENT_TOGGLE_POWER
} IrEventType;

typedef struct {
    IrEventType type;
    int delta;
} IrMessage;


#endif /* INC_SYSTEM_STATE_H_ */
