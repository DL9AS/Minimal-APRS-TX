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

#include "config.h"
#include "globals.h"
#include "aprs.h"
#include "ax25.h"
#include "SX1278.h"
#include "voltage.h"

// Module globals
uint16_t aprs_packet_counter = 0;

// APRS configuration
uint64_t APRS_freq = SX1278_FREQUENCY;
int32_t APRS_freq_err = SX1278_FREQUENCY_CORRECTION;
uint8_t TX_pwr = SX1278_TX_POWER;
uint32_t FSK_deviation = SX1278_DEVIATION;
char Src_call[8] = APRS_SOURCE_CALLSIGN;
uint8_t Src_ssid = APRS_SOURCE_SSID;
char Digi_call[8] = APRS_DIGIPEATER;
uint8_t Digi_ssid = APRS_DIGIPEATER_SSID;

// APRS Position packets tmp
int32_t latitude = 0;
int32_t longitude = 0;
int32_t altitude = 0;
char latitude_DMH[9];
char longitude_DMH[10];
char comment[81] = "";
char symbol = APRS_SYMBOL;
char symbol_overlay = APRS_SYMBOL_OVERLAY;

// Module functions
void main_generate_aprs_position_packet();
void main_generate_aprs_status_packet();
void convert_gps_coordinates_to_DMH(int32_t gps_latitude, int32_t gps_longitude, char* gps_latitude_DMH, char* gps_longitude_DMH);

void setup()
{
  WDT_INIT;

  // Enable or disable debug serial
  #ifdef DEBUG_SERIAL_ENABLE
    Serial.begin(DEBUG_SERIAL_BAUD_RATE);
  #endif

  // Begin SX1278
  SX1278_begin();

  // Set unused Pins to INPUT_PULLUP for power savings
  pinMode(1, INPUT_PULLUP );
  pinMode(3, INPUT_PULLUP );
  pinMode(6, INPUT_PULLUP );
  pinMode(7, INPUT_PULLUP );
  pinMode(11, INPUT_PULLUP );

  DEBUG_PRINTLN("[STAT]: booted");
}

void loop()
{

  char serial_command_buffer[100]; // TODO: Check size requirements
  memset(serial_command_buffer, '\0', 100);

  while (Serial.available() == false) {WDT_RESET;} // Wait for serial data

  Serial.readBytesUntil(';', serial_command_buffer, 150);

  DEBUG_PRINT("[RCV]: ");
  DEBUG_PRINTLN(serial_command_buffer);

  uint8_t status;

  // Parse $STAT command
  if (!strcmp(serial_command_buffer, "$STAT"))
  {
    DEBUG_PRINTLN("[ACK]: Minimal APRS TX - V1.0");
  }

  // Parse $CfgRadio command
  else if ((status = sscanf(serial_command_buffer, "$CfgRadio %lu,%ld,%hhu,%hhu,%lu,%[^,],%hhu,%[^,],%hhu", &APRS_freq, &APRS_freq_err, &TX_pwr, &AFSK_tone_adj_space, &FSK_deviation, Src_call, &Src_ssid, Digi_call, &Digi_ssid)))
  {
    if(status == 9)
    {
      DEBUG_PRINTLN("[ACK]: valid $CfgRadio");
      AFSK_tone_adj_mark = AFSK_tone_adj_space * 2;
    }
    else
    {
      DEBUG_PRINTLN("[ERR]: invalid $CfgRadio");
    }
  }

  // Parse $TxPosPkt command
  else if ((status = sscanf(serial_command_buffer, "$TxPosPkt %ld,%ld,%ld,%c,%c,\"%[^\"]", &latitude, &longitude, &altitude, &symbol, &symbol_overlay, comment)))
  {
    if(status == 6)
    {
      DEBUG_PRINTLN("[ACK]: valid $TxPosPkt");
      main_generate_aprs_position_packet();
    }
    else
    {
      DEBUG_PRINTLN("[ERR]: invalid $TxPosPkt");
    }
  }

  // Parse $TXStatPkt command
  else if ((status = sscanf(serial_command_buffer, "$TxStatPkt \"%[^\"]", comment)))
  {
    if(status == 1)
    {
      DEBUG_PRINTLN("[ACK]: valid $TXStatPkt");
      main_generate_aprs_status_packet();
    }
    else
    {
      DEBUG_PRINTLN("[ERR]: invalid $TXStatPkt");
    }
  }

  // No valid command found
  else
  {
    DEBUG_PRINTLN("[ERR]: invalid input");
  }
}

/*
* GNSS coordinates format conversion in degrees, minutes and hundredths of a minute (needed for aprs)
* N = north; S = south; E = east; W = west
* more information at: http://www.aprs.org/doc/APRS101.PDF
*/
void convert_gps_coordinates_to_DMH(int32_t gps_latitude, int32_t gps_longitude, char* gps_latitude_DMH, char* gps_longitude_DMH)
{
  int32_t degrees_buffer;
  int32_t minutes_buffer;
  int32_t h_minutes_buffer;
  
  // Calculate latitude

  // Determine whether to append "N" or "S"
  char compass_direction = 'N';
  
  if(gps_latitude < 0)
  {
    gps_latitude = -(gps_latitude);
    compass_direction = 'S';
  }
    
  // Full degrees
  degrees_buffer = gps_latitude/10000;

  // Degree minutes from the remainder
  gps_latitude = gps_latitude - (degrees_buffer*10000);
  gps_latitude = (gps_latitude)*60;
  minutes_buffer = gps_latitude/10000;

  // Hundredths of degree minutes from the remainder
  h_minutes_buffer = (gps_latitude - minutes_buffer*10000)/100;

  // Generate postion data string
  sprintf(gps_latitude_DMH, "%02ld%02ld.%02ld%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, compass_direction);

  // Calculate longitude

  // Determine whether to append "E" or "W"
  compass_direction = 'E';
  
  if(gps_longitude < 0)
  {
    gps_longitude = -(gps_longitude);
    compass_direction = 'W';
  }
  
  // Full degrees
  degrees_buffer = gps_longitude/10000;

  // Degree minutes from the remainder
  gps_longitude = gps_longitude - (degrees_buffer*10000);
  gps_longitude = (gps_longitude)*60;
  minutes_buffer = gps_longitude/10000;

  // Hundredths of degree minutes from the remainder
  h_minutes_buffer = (gps_longitude - minutes_buffer*10000)/100;
  
  // Generate postion data string
  sprintf(gps_longitude_DMH, "%03ld%02ld.%02ld%c" , degrees_buffer, minutes_buffer, h_minutes_buffer, compass_direction);  
}

void main_generate_aprs_position_packet()
{
  // Convert GPS coordinates
  convert_gps_coordinates_to_DMH(latitude, longitude, latitude_DMH, longitude_DMH);

  // Get temperature measurement from SX1278
  SX1278_measure_internal_temperature(&sx1278_internal_last_temp);

  voltage_get_measurements();
  
  char aprs_packet_comment_buf[41];
  sprintf(aprs_packet_comment_buf, "/A=%06ld/N%dT%dV%d|DL9AS MinAPRSTX| ",
  int32_t (altitude * 3.28),
  aprs_packet_counter,
  sx1278_internal_last_temp,
  mcu_voltage);

  // Send APRS packet
  aprs_send_position_packet(&APRS_freq, TX_pwr, FSK_deviation, Src_call, Src_ssid, Digi_call, Digi_ssid, symbol, symbol_overlay, latitude_DMH, longitude_DMH, aprs_packet_comment_buf, comment);

  DEBUG_PRINTLN("[APRS]: PosPkt send");

  // Increment APRS packet counter
  aprs_packet_counter++;
}

void main_generate_aprs_status_packet()
{
  // Convert GPS coordinates
  convert_gps_coordinates_to_DMH(latitude, longitude, latitude_DMH, longitude_DMH);

  // Get temperature measurement from SX1278
  SX1278_measure_internal_temperature(&sx1278_internal_last_temp);
  
  voltage_get_measurements();

  char aprs_packet_info_buf[41];
  sprintf(aprs_packet_info_buf, "N%dT%dV%d|DL9AS MinAPRSTX| ",
  aprs_packet_counter,
  sx1278_internal_last_temp,
  mcu_voltage);

  // Send APRS packet
  aprs_send_status_packet(&APRS_freq, TX_pwr, FSK_deviation, Src_call, Src_ssid, Digi_call, Digi_ssid, aprs_packet_info_buf, comment);

  DEBUG_PRINTLN("[APRS]: StatPkt send");

  // Increment APRS packet counter
  aprs_packet_counter++;
}