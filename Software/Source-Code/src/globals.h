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

#ifndef __GLOBALS__H__
#define __GLOBALS__H__

#include <Arduino.h>

#include "config.h"

#ifdef DEBUG_SERIAL_ENABLE
  #define DEBUG_PRINT(x)  Serial.print (x)
  #define DEBUG_PRINTLN(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

#ifdef WATCHDOG_ENABLE
  #define WDT_INIT _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8KCLK_gc) // 8s WDT timeout - longest available
  #define WDT_RESET __asm__ __volatile__ ("wdr"::)
#else
  #define WDT_INIT
  #define WDT_RESET
#endif

#endif