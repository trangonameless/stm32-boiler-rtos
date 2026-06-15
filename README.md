<img width="800" height="450" alt="output2" src="https://github.com/user-attachments/assets/ce598522-a322-436a-89e2-a15c51781b7f" />

# stm32-boiler-rtos

## Overview

The STM32-based system controls a heater to maintain a user-defined temperature setpoint. The setpoint is adjusted using an IR remote control, the target temperature are displayed on a 7-segment display. LED indicators provide visual feedback of the heater state (ON/OFF/HEATING/ERROR).

In the updated version of the project, the firmware has been migrated to an RTOS-based architecture, significantly improving system responsiveness, especially for IR remote handling. The system also introduces a finite state machine (FSM) to manage boiler operation in a more structured and deterministic way.

A Raspberry Pi 4 collects runtime data from the STM32 via UART and forwards it using MQTT to a Grafana dashboard for real-time monitoring and visualization.

## Hardware
STM32L476RGTX (main controller)
Raspberry Pi 4 B (data gateway & MQTT publisher)
DS18B20 temperature sensor
Relay module with opto-isolation
7-segment display
IR remote control
Infrared receiver module


STM32 → UART → Raspberry Pi 4 B → MQTT  → Grafana

## Key Features
RTOS-based firmware

The system is built on FreeRTOS, allowing separation of concerns into independent tasks such as:

IR remote processing
temperature acquisition
control logic execution
state publishing

This architecture improves timing predictability and significantly increases responsiveness of IR input handling compared to the previous implementation.

Finite State Machine (FSM)

The boiler control logic is implemented as a finite state machine, providing clear and deterministic transitions between operating modes:

OFF
IDLE
HEATING
ERROR

This improves code maintainability, reduces complexity, and ensures safe and predictable system behavior.

Improved IR responsiveness

Thanks to RTOS task separation and interrupt-driven decoding, IR commands are processed faster and more reliably, eliminating delays caused by blocking loops in the previous version.

## Technologies
STM32 HAL
FreeRTOS 
Raspberry Pi 4 B
MQTT 
Grafana
UART communication

## Improvements over previous version
Migration from loop to RTOS multitasking
Introduction of FSM for boiler control logic
Improved IR remote responsiveness (interrupt + queue-based processing)
<img width="302" height="403" alt="20260610_110549(1)" src="https://github.com/user-attachments/assets/b280b420-0f25-433f-9cb8-f9eba1803db3" />




