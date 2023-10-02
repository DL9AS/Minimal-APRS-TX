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

#ifndef __CONFIG__H_
#define __CONFIG__H_

#include "defines.h"

/*
 * Target config
 */

  // Choose target:
  // TARGET_MIN_APRS_TX (only target for now)
  #define TARGET TARGET_MIN_APRS_TX 

/*
 * Payload config
 */

  #define WATCHDOG_ENABLE

/*
 * Radio config
 */

  #define SX1278_FREQUENCY 144800000 // Frequency in kHz

  // Set SX1278 NF modulation option
  // MOD_DIO2: use SX1278 DIO2 to generate AFSK modulation
  // MOD_F_HOP: use SX1278 fast frequency hopping to generate AFSK modulation (DIO2 not longer needed)
  #define SX1278_MOD_OPTION MOD_DIO2

  #if SX1278_MOD_OPTION == MOD_DIO2
    #define SX1278_FREQUENCY_CORRECTION -60000 // Frequency offset in Hz
    #define SX1278_TX_POWER 17  // Tx power in dbm (2-20 dbm)
    #define SX1278_DEVIATION 2700 // FSK deviation in Hz
  #elif SX1278_MOD_OPTION == MOD_F_HOP
    #define SX1278_FREQUENCY_CORRECTION -55800 // Frequency offset in Hz
    #define SX1278_TX_POWER 17  // Tx power in dbm (2-20 dbm)
    #define SX1278_DEVIATION 3000 // FSK deviation in Hz
  #endif

  #define SX1278_CRYSTAL_FREQ 32000000 // Crystal frequency in Hz

/*
 * APRS config (defaults)
 */
  
  #define APRS_SOURCE_CALLSIGN "N0CALL"
  #define APRS_SOURCE_SSID 1
  
  #define APRS_DESTINATION_CALLSIGN "AMON"
  #define APRS_DESTINATION_SSID 0
  
  #define APRS_DIGIPEATER "WIDE1"
  #define APRS_DIGIPEATER_SSID 1

  #define APRS_SYMBOL_OVERLAY '/'
  #define APRS_SYMBOL 'O'
   
  #define APRS_FLAGS_AT_BEGINNING 100 // Send x times ax25 flag as TX delay

  // AFSK mark/space length can differ due to inaccurate MCU clock 
  // For DIO2 mod: 408us 1200Hz and 204us 2400Hz good starting point
  // For SX1278 Fhop mod: 390us 1200Hz and 195us 2400Hz good stating point
  #if SX1278_MOD_OPTION == MOD_DIO2
    #define APRS_1200_MARK_DELAY 410 // Send 1200Hz mark for ~833us, so theoretical a delay between state changes of ~417us
    #define APRS_2400_SPACE_DELAY 205 // Send twice 2400Hz space for ~417us each, so theoretical a delay between state changes of ~208us
  #elif SX1278_MOD_OPTION == MOD_F_HOP
    #define APRS_1200_MARK_DELAY 390 // Send 1200Hz mark for ~833us, so theoretical a delay between state changes of ~417us
    #define APRS_2400_SPACE_DELAY 195 // Send twice 2400Hz space for ~417us each, so theoretical a delay between state changes of ~208s
  #endif

  #define APRS_COMMENT_BUF_SIZE 150
 
  #define TEMPERATURE_SENSOR SX1278_INTERNAL // SX1278_INTERNAL (only option for now)
  #define SX1278_INTERNAL_TEMP_OFFSET 15 // Offset in K

/*
 * Serial config
 */

  #define DEBUG_SERIAL_ENABLE

  #define DEBUG_SERIAL_BAUD_RATE 9600
  
#endif