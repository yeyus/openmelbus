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

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart/uart.h"

// MCU Settings
#define F_CPU 16000000UL  // 16 MHz

#define HIGH        1
#define LOW         0

// Data lines definition
#define MCLK         2
#define MBUSY        3
#define MDATA        4
#define MRUN         5
#define LOG_ISR      6


#define CMD_UNKNOWN      0
#define CMD_FF           1
#define CMD_FR           2
#define CMD_RND          3
#define CMD_TRACK_UP     4
#define CMD_TRACK_DOWN   5
#define CMD_DISC_UP      6
#define CMD_DISC_DOWN    7
#define CMD_POWER_DOWN   8
#define CMD_PLAY_INFO_1B    9
#define CMD_PLAY_INFO_1A    12
#define CMD_CHG_INFO_REQ     10
#define CMD_DEVICE_ID_REQ    11
#define CMD_DEVICE_CD_INIT   13

#define DEVICE_CD            0x80
#define DEVICE_TV            0xA9
#define DEVICE_SAT           0xC0
#define DEVICE_MDC           0xD8
#define DEVICE_CDC           0xE8

#define BUFSIZE 80

void main(void) __attribute__((noreturn));
void signal_hu_presence();
uint8_t parse_melbus_command();
void debug_melbus_command(int cmd);
void device_recognition();
