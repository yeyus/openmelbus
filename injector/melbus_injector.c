/**
 * OpenMelbus - Melbus Injector
 *
 *  Copyright 2014 by Jesus F. Trujillo <elyeyus@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */
 
#include "melbus_injector.h"

#define LOGGING

volatile uint8_t dataIn[BUFSIZE];
volatile uint8_t dataOut;
volatile uint8_t byteMarker = 0;
volatile uint8_t bitMarker = 7;
volatile uint8_t busy = HIGH;
volatile uint8_t delivered = HIGH;

const uint8_t knownDevices[] = {DEVICE_CD, DEVICE_TV, DEVICE_SAT, DEVICE_MDC, DEVICE_CDC};
const uint8_t enabledDevices[] = {DEVICE_TV, DEVICE_MDC, DEVICE_CDC};
const uint8_t enabledDevicesCount = 3;
volatile uint8_t devicePointer;

const uint8_t deviceId[] = {0xA9, 0xE8};
const uint8_t deviceNum = 2;

void main(void)
{
  // Setup phase
  cli();
  
  pinMode(MBUSY,INPUT_PULLUP);
  pinMode(MCLK, INPUT_PULLUP);
  pinMode(MDATA, INPUT_PULLUP);
  pinMode(MRUN, INPUT);
  
  #ifdef LOGGING
  DDRD |= (1<<PIND6); // Set LOG_ISR output
  #endif
  
  EIMSK = (1 << INT0 | 1 << INT1);
  EICRA |= (1 << ISC01); // INT0 Falling edge
  EICRA |= (0 << ISC00);
  EICRA |= (1 << ISC10); // INT1 on CHANGE
  EICRA |= (0 << ISC11);

  //while(!bitRead(PORTD, 5)) {}; // MRUN
  signal_hu_presence();

  // Initialize serial port
  uart0_init(UART_BAUD_SELECT(115200, 16000000L)+1);
  
  sei();
 
  uart0_puts("RUN state active");
  uart0_puts("Melbus Injector -- Jesus Trujillo 2014(C)");
  
  // Infinte loop
  while(1) {
    uint8_t cmd = CMD_UNKNOWN;
    if(busy == HIGH && delivered == false)
    {
      cmd = parse_melbus_command();
      if(cmd == CMD_UNKNOWN) {
        for(uint8_t i = 0;i<byteMarker;i++) {
          Serial.print(dataIn[i], HEX);
          uart0_puts(".");
        }
        uart0_puts("\r\n");
      }
      debug_melbus_command(cmd);
      delivered = true;
    }
  }
}

void signal_hu_presence()
{
  DDRD |= (1<<PIND3); // Set MBUSY output
  PORTD &= ~(1 << PD3); // Write 0
  _delay_ms(1500);
  DDRD &= ~(1<<PIND3); // input
  PORTD |= (1<<PD3); // enable pull-up
}

uint8_t parse_melbus_command()
{
  uint8_t cmd = CMD_UNKNOWN;
  if (dataIn[1]==0x19 && dataIn[2]==0x2F) {
    cmd = CMD_FR;
  } else if (dataIn[1]==0x19 && dataIn[2]==0x29) {
    cmd = CMD_FF;
  } else if (dataIn[1]==0x19 && dataIn[2]==0x52) {
    cmd = CMD_RND;
  } else if (dataIn[1]==0x19 && dataIn[2]==0x22) {
    cmd = CMD_POWER_DOWN;
  } else if (dataIn[1]==0x1B && dataIn[2]==0x2D &&
      dataIn[3]==0x40 && dataIn[4]==0x01) {
    cmd = CMD_TRACK_UP;
  } else if (dataIn[1]==0x1B && dataIn[2]==0x2D &&
      dataIn[3]==0x00 && dataIn[4]==0x01) {
    cmd = CMD_TRACK_DOWN;
  } else if (dataIn[1]==0x1A && dataIn[2]==0x50 &&
      dataIn[3]==0x41) {
    cmd = CMD_DISC_UP;
  } else if (dataIn[1]==0x1A && dataIn[2]==0x50 &&
      dataIn[3]==0x01) {
    cmd = CMD_DISC_DOWN;
  } else if (dataIn[1]==0x1B && dataIn[2]==0xE0) {
    cmd = CMD_PLAY_INFO_1B;
  } else if (dataIn[1]==0x1A && dataIn[2]==0xE0) {
    cmd = CMD_PLAY_INFO_1A;
  } else if (dataIn[1]==0x1E && dataIn[2]==0xEF) {
    cmd = CMD_CHG_INFO_REQ;
  } else if (dataIn[2]==0xED && dataIn[3]==0x80 && dataIn[4]==0x86) {
    cmd = CMD_DEVICE_CD_INIT;
  } else if (dataIn[0]==0x07 && dataIn[1]==0x1A && dataIn[2]==0xEE) {
    cmd = CMD_DEVICE_ID_REQ;
  } else {
    cmd = CMD_UNKNOWN;
  }
  return cmd;
}

void debug_melbus_command(int cmd)
{
  switch(cmd) {
    case CMD_UNKNOWN:
      uart0_puts("UNKNOWN");
      break;
    case CMD_FF:
      uart0_puts("FF");
      break;
    case CMD_FR:
      uart0_puts("FR");
      break;
    case CMD_RND:
      uart0_puts("RND");
      break;
    case CMD_DISC_UP:
      uart0_puts("DISC_UP");
      break;
    case CMD_DISC_DOWN:
      uart0_puts("DISC_DOWN");
      break;
    case CMD_TRACK_UP:
      uart0_puts("TRACK_UP");
      break;
    case CMD_TRACK_DOWN:
      uart0_puts("TRACK_DOWN");
      break;
    case CMD_POWER_DOWN:
      uart0_puts("POWER_DOWN");
      break;
    case CMD_PLAY_INFO_1A:
      uart0_puts("PLAY_INFO_1A");
    case CMD_PLAY_INFO_1B:
      uart0_puts("PLAY_INFO");
      Serial.print("DISC:");
      Serial.print(dataIn[8],HEX);
      Serial.print("TRACK:");
      Serial.print(dataIn[10],HEX);
      Serial.print(" - ");
      Serial.print(dataIn[12],HEX);
      Serial.print("m");
      Serial.print(dataIn[13],HEX);
      uart0_puts("s");
      break;
    case CMD_CHG_INFO_REQ:
      uart0_puts("CHG_INFO_REQUEST");
      break;
    case CMD_DEVICE_ID_REQ:
      uart0_puts("DEVICE_ID_REQUEST");
      device_recognition();
      break;
    case CMD_DEVICE_CD_INIT:
      uart0_puts("DEVICE_CD_INIT");
      break;
  }
}

void device_recognition()
{
  for(uint8_t i=0;i<byteMarker;i++)
  {
    if(dataIn[i] == DEVICE_CD && dataIn[i+1]!=0xFF) {
      uart0_puts("Found DEVICE_CD");
    } else if (dataIn[i] == DEVICE_TV && dataIn[i+1] != 0xFF) {
      uart0_puts("Found DEVICE_TV");
    } else if (dataIn[i] == DEVICE_SAT && dataIn[i+1] != 0xFF) {
      uart0_puts("Found DEVICE_SAT");
    } else if (dataIn[i] == DEVICE_MDC && dataIn[i+1] != 0xFF) {
      uart0_puts("Found DEVICE_MDC");
    } else if (dataIn[i] == DEVICE_CDC && dataIn[i+1] != 0xFF) {
      uart0_puts("Found DEVICE_CDC");
     }
  }
}

// CLOCK ISR
ISR(INT0_vect)
{
  #ifdef LOGGING
    PORTD |= (1 << PD6); // Write 1 to LOG_ISR
  #endif
  
  if(busy == LOW) {

    if(byteMarker > 3 &&
        dataIn[0] == 0x07 &&
        dataIn[byteMarker-1] == DEVICE_CDC &&
        dataIn[byteMarker-2] != DEVICE_CDC)
    {
      dataOut = DEVICE_CDC;
      DDRD |= (1<<PIND4); // Set PD4 output
      if (dataOut & (1<<bitMarker)) {
        PORTD |= (1 << PD4); // Write 1
      } else {
        PORTD &= ~(1 << PD4); // Write 0
      }
    } else {
      // We are reading from the BUS
      // We set the proper bit in the array to 1 or 0
      uint8_t pinv = bitRead(PORTD, 4); // MDATA
      if(pinv) {
        dataIn[byteMarker] |= (1 << bitMarker);
      } else {
        dataIn[byteMarker] &= ~(1 << bitMarker);
      }
    }
    
    
    bitMarker--;
    if(bitMarker < 0) {
      DDRD &= ~(1<<PIND4); // input
      PORTD |= (1<<PD4); // enable pull-up
      bitMarker = 7;
      byteMarker++;
    }
  }
  
  #ifdef LOGGING
    PORTD &= ~(1 << PD6); // Write 0 to LOG_ISR
  #endif
}

// BUSY LINE ISR
ISR(INT1_vect)
{
  #ifdef LOGGING
    PORTD |= (1 << PD6); // Write 1 to LOG_ISR
  #endif
  
  busy = bitRead(PORTD, 3); // MBUSY
  if(busy == LOW) {
    // When we detect busy pulling up we reset the byteMarker
    byteMarker = 0;
  } else {
    delivered = (1&(byteMarker == 0));
  }
  
  #ifdef LOGGING
    PORTD &= ~(1 << PD6); // Write 0 to LOG_ISR
  #endif
}
