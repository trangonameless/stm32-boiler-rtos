/*
 * ir_messages.h
 *
 *  Created on: Jun 11, 2026
 *      Author: patry
 */

#ifndef INC_IR_MESSAGES_H_
#define INC_IR_MESSAGES_H_

typedef enum {
    IR_EVENT_TEMP_CHANGE,
    IR_EVENT_TOGGLE_POWER
} IrEventType;

typedef struct {
    IrEventType type;
    int delta;
} IrMessage;



#endif /* INC_IR_MESSAGES_H_ */
