#include <avr/interrupt.h>
#include <avr/io.h>

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

#define LOGGING

volatile byte dataIn[BUFSIZE];
volatile byte dataOut;
volatile int byteMarker = 0;
volatile int bitMarker = 7;
volatile int busy = HIGH;
volatile int delivered = true;

const int knownDevices[] = {DEVICE_CD, DEVICE_TV, DEVICE_SAT, DEVICE_MDC, DEVICE_CDC};
const int enabledDevices[] = {DEVICE_TV, DEVICE_MDC, DEVICE_CDC};
const int enabledDevicesCount = 3;
volatile int devicePointer;

const int deviceId[] = {0xA9, 0xE8};
const int deviceNum = 2;

void setup() 
{
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
  
  sei();
 
  // Initialize serial port
  Serial.begin(115200,SERIAL_8N1);  
  Serial.println("RUN state active");
  Serial.println("Melbus Analyzer -- Jesus Trujillo 2014(C)");
}

void loop() 
{
  int cmd = CMD_UNKNOWN;
  if(busy == HIGH && delivered == false) 
  {
    cmd = parse_melbus_command();
    if(cmd == CMD_UNKNOWN) {
      for(int i = 0;i<byteMarker;i++) {
        Serial.print(dataIn[i], HEX);
        Serial.print(".");
      }
      Serial.print("\r\n");
    }
    debug_melbus_command(cmd);
    delivered = true;
  } 
}

void signal_hu_presence()
{
  DDRD |= (1<<PIND3); // Set MBUSY output
  PORTD &= ~(1 << PD3); // Write 0
  delay(1500);
  DDRD &= ~(1<<PIND3); // input
  PORTD |= (1<<PD3); // enable pull-up
}

int parse_melbus_command()
{
  int cmd = CMD_UNKNOWN;
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

void debug_melbus_command(int cmd) {
  switch(cmd) {
    case CMD_UNKNOWN:
      Serial.println("UNKNOWN");
      break;
    case CMD_FF:
      Serial.println("FF");    
      break;
    case CMD_FR:
      Serial.println("FR");    
      break;
    case CMD_RND:
      Serial.println("RND");
      break;
    case CMD_DISC_UP:
      Serial.println("DISC_UP");
      break;
    case CMD_DISC_DOWN:
      Serial.println("DISC_DOWN");
      break;
    case CMD_TRACK_UP:
      Serial.println("TRACK_UP");
      break;
    case CMD_TRACK_DOWN:
      Serial.println("TRACK_DOWN");
      break;
    case CMD_POWER_DOWN:
      Serial.println("POWER_DOWN");
      break;
    case CMD_PLAY_INFO_1A:
      Serial.println("PLAY_INFO_1A");
    case CMD_PLAY_INFO_1B:
      Serial.println("PLAY_INFO");
      Serial.print("DISC:");
      Serial.print(dataIn[8],HEX);
      Serial.print("TRACK:");
      Serial.print(dataIn[10],HEX);
      Serial.print(" - ");
      Serial.print(dataIn[12],HEX);
      Serial.print("m");
      Serial.print(dataIn[13],HEX);
      Serial.println("s");
      break;
    case CMD_CHG_INFO_REQ:
      Serial.println("CHG_INFO_REQUEST");
      break;
    case CMD_DEVICE_ID_REQ:
      Serial.println("DEVICE_ID_REQUEST");
      device_recognition();
      break;
    case CMD_DEVICE_CD_INIT:
      Serial.println("DEVICE_CD_INIT");
      break;
  }
}

void device_recognition() 
{
  for(int i=0;i<byteMarker;i++) 
  {
    if(dataIn[i] == DEVICE_CD && dataIn[i+1]!=0xFF) {
      Serial.println("Found DEVICE_CD");
    } else if (dataIn[i] == DEVICE_TV && dataIn[i+1] != 0xFF) {
      Serial.println("Found DEVICE_TV");
    } else if (dataIn[i] == DEVICE_SAT && dataIn[i+1] != 0xFF) {
      Serial.println("Found DEVICE_SAT");
    } else if (dataIn[i] == DEVICE_MDC && dataIn[i+1] != 0xFF) {
      Serial.println("Found DEVICE_MDC");
    } else if (dataIn[i] == DEVICE_CDC && dataIn[i+1] != 0xFF) {
      Serial.println("Found DEVICE_CDC");
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
      int pinv = bitRead(PORTD, 4); // MDATA   
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
