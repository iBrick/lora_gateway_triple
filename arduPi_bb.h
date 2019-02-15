/*
*  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
*  http://www.libelium.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Version 2.0 (For Raspberry Pi Rev2)
*  Author: Sergio Martinez
*/

#ifndef arduPi_h
#define arduPi_h

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <algorithm>
#include <limits.h>
#include <pthread.h>
#include <poll.h>
#include "SPI.h"
#include "GPIO.h"
//#include "bcm2835.h"

#include <stdarg.h> //Include forva_start, va_arg and va_end strings functions

#define IOBASE   0x3f000000


#define GPIO_BASE2 (IOBASE + 0x200000)

#define BIT_4 (1 << 4)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_21 (1 << 21)
#define BIT_27 (1 << 27)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)


#define PAGESIZE 4096
#define BLOCK_SIZE 4096


#define CS		10
#define MOSI	11
#define MISO	12
#define SCK		13

#define LSBFIRST  0  ///< LSB First
#define MSBFIRST  1   ///< MSB First

#define SPI_MODE0 0  ///< CPOL = 0, CPHA = 0
#define SPI_MODE1 1  ///< CPOL = 0, CPHA = 1
#define SPI_MODE2 2  ///< CPOL = 1, CPHA = 0
#define SPI_MODE3 3

#define SPI_CLOCK_DIV65536 0       ///< 65536 = 256us = 4kHz
#define SPI_CLOCK_DIV32768 32768   ///< 32768 = 126us = 8kHz
#define SPI_CLOCK_DIV16384 16384   ///< 16384 = 64us = 15.625kHz
#define SPI_CLOCK_DIV8192 8192    ///< 8192 = 32us = 31.25kHz
#define SPI_CLOCK_DIV4096 4096    ///< 4096 = 16us = 62.5kHz
#define SPI_CLOCK_DIV2048 2048    ///< 2048 = 8us = 125kHz
#define SPI_CLOCK_DIV1024 1024    ///< 1024 = 4us = 250kHz
#define SPI_CLOCK_DIV512 512     ///< 512 = 2us = 500kHz
#define SPI_CLOCK_DIV256 256     ///< 256 = 1us = 1MHz
#define SPI_CLOCK_DIV128 128     ///< 128 = 500ns = = 2MHz
#define SPI_CLOCK_DIV64 64      ///< 64 = 250ns = 4MHz
#define SPI_CLOCK_DIV32 32      ///< 32 = 125ns = 8MHz
#define SPI_CLOCK_DIV16 16      ///< 16 = 50ns = 20MHz
#define SPI_CLOCK_DIV8 8       ///< 8 = 25ns = 40MHz
#define SPI_CLOCK_DIV4 4       ///< 4 = 12.5ns 80MHz
#define SPI_CLOCK_DIV2 2       ///< 2 = 6.25ns = 160MHz
#define SPI_CLOCK_DIV1 1       ///< 0 = 256us = 4kHz

namespace unistd {
	//All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};


enum Representation{
	BIN,
	OCT,
	DEC,
	HEX,
	BYTE
};

typedef enum {
	INPUT,
	OUTPUT
}Pinmode;

typedef enum
{
	LOW = 0,
	HIGH = 1,
	RISING = 2,
	FALLING = 3,
	BOTH = 4
} Digivalue;

typedef bool boolean;
typedef unsigned char byte;


class SPIPi{
	public:
		SPIPi();
  		void begin();
    	void end();
    	void setBitOrder(uint8_t order);
 		void setClockDivider(uint16_t divider);
		void setDataMode(uint8_t mode);
 		void chipSelect(uint8_t cs);
		void setChipSelectPolarity(uint8_t cs, uint8_t active);
 		uint8_t transfer(uint8_t value);
 		void transfernb(char* tbuf, char* rbuf, uint32_t len);
};

/* Some useful arduino functions */
void pinMode(int pin, Pinmode mode);
void digitalWrite(int pin, int value);
void delay(long millis);
void delayMicroseconds(long micros);
int digitalRead(int pin);
int analogRead (int pin);


//uint8_t shiftIn  (uint8_t dPin, uint8_t cPin, bcm2835SPIBitOrder order);
//void shiftOut (uint8_t dPin, uint8_t cPin, bcm2835SPIBitOrder order, uint8_t val);
void setup();
void loop();
long millis();

/* Helper functions */
int beaglebonePinNumber(int arduinoPin);
uint32_t ch_peri_read(volatile uint32_t* paddr);
uint32_t ch_peri_read_nb(volatile uint32_t* paddr);
void ch_peri_write(volatile uint32_t* paddr, uint32_t value);
void ch_peri_write_nb(volatile uint32_t* paddr, uint32_t value);
void ch_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask);
void ch_gpio_fsel(uint8_t pin, uint8_t mode);

extern SPIPi SPI;

#endif
