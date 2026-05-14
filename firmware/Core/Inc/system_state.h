/*
 * system_state.h
 *
 *  Created on: Apr 15, 2026
 *      Author: patry
 */


#ifndef INC_SYSTEM_STATE_H_
#define INC_SYSTEM_STATE_H_
#include <stdbool.h>



typedef enum {
    BOILER_OFF,
    BOILER_IDLE,
    BOILER_HEATING,
    BOILER_ERROR
} BoilerState_t;

typedef struct {
    int set_temp;
    float temp;

    BoilerState_t mode;

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
