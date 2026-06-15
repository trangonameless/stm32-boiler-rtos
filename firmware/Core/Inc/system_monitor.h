/*
 * system_monitor.h
 *
 *  Created on: Jun 11, 2026
 *      Author: patry
 */

#ifndef INC_SYSTEM_MONITOR_H_
#define INC_SYSTEM_MONITOR_H_

#include "stdint.h"
#include "cmsis_os.h"

typedef struct {
    volatile uint32_t ir_tick;
    volatile uint32_t temp_tick;
    volatile uint32_t control_tick;
    volatile uint32_t uart_tick;
} SystemHeartbeat_t;

extern SystemHeartbeat_t hb;

#endif /* INC_SYSTEM_MONITOR_H_ */
