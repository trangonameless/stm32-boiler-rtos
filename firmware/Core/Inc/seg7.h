/*
 * seg7.h
 *
 *  Created on: Sep 23, 2025
 *      Author: patry
 */

#pragma once

#include <stdint.h>

// Pokaż cyfrę na wyświetlaczu

// value - cyfra do wyświetlenia

void seg7_show_digit(uint32_t value);

void seg7_show(uint32_t value);

// Funkcja pomocnicza

// Zmiana aktywnego wyświetlacza

void seg7_update(void);

