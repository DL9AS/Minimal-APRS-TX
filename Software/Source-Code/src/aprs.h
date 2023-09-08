/*
 * This file is part of the Minimal APRS TX project.
 * 
 
 * Copyright (C) 2023  Amon Schumann / DL9AS
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */ 

#ifndef __APRS__H__
#define __APRS__H__

#include <Arduino.h>

// Exported functions
void aprs_send_position_packet(uint64_t *freq, uint8_t pwr, uint32_t deviation, char *src_call, uint8_t src_SSID, char *digi_call, uint8_t digi_SSID, char symbol, char symbol_overlay, char *latitude, char *longitude, char *info, char *comment);
void aprs_send_status_packet(uint64_t *freq, uint8_t pwr, uint32_t deviation, char *src_call, uint8_t src_SSID, char *digi_call, uint8_t digi_SSID, char *info, char *comment);

#endif