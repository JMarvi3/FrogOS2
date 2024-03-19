#pragma once

#include <stdint.h>

void enable();

void disable();

void irq_set_mask(uint8_t irq);

void irq_clear_mask(uint8_t irq);

void setup_idt();