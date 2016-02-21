#pragma once

extern unsigned char registers[];

void registers_read_callback(uint16_t address);
void registers_write_callback(uint16_t address);
