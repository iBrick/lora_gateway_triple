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
*  Version 2.0
*  Author: Sergio Martinez
*/
#include "arduPi_bb.h"

timeval start_program, end_point;

/*******************************
 *                             *
 * SPIPi Class implementation *
 * --------------------------- *
 *******************************/

/******************
 * Public methods *
 ******************/

 SPIPi::SPIPi(){
//	memset(Tx_spi, 0, sizeof(Tx_spi));
//	memset(RX_spi, 0, sizeof(RX_spi));
 }

#define SPI_PORT (BBBIO_McSPI_SPI1)

void SPIPi::begin(){
	if (SPI_DEV1_init(SPIDEV_BYTES_NUM, SPIDEV1_BUS_SPEED_HZ, SPI_SS_LOW,
                      SPIDEV_DELAY_US, SPIDEV_DATA_BITS_NUM,
                      SPI_MODE0) == -1)
		printf("(Main)spidev1.0 initialization failed\r\n");
	else 	printf("(Main)spidev1.0 initialized - READY\r\n");
}

void SPIPi::end(){  
}

void SPIPi::setBitOrder(uint8_t order){
    // BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one suported by SPI0
}

// defaults to 0, which means a divider of 65536.
// The divisor must be a power of 2. Odd numbers
// rounded down. The maximum SPI clock rate is
// of the APB clock
void SPIPi::setClockDivider(uint16_t divider){
//    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CLK/4;
//    ch_peri_write(paddr, divider);
}

void SPIPi::setDataMode(uint8_t mode){
//    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    // Mask in the CPO and CPHA bits of CS
//    ch_peri_set_bits(paddr, mode << 2, BCM2835_SPI0_CS_CPOL | BCM2835_SPI0_CS_CPHA);
}

// Writes (and reads) a single byte to SPI
//uint8_t SPIPi::transfer(uint8_t value){
//    return 0;
//}

// Writes (and reads) a number of bytes to SPI
void SPIPi::transfernb(char *tbuf, char* rbuf, uint32_t len){
	SPIDEV1_transfer(reinterpret_cast<unsigned char*>(tbuf),reinterpret_cast<unsigned char*>(rbuf), len);
}

void SPIPi::chipSelect(uint8_t cs){
}

void SPIPi::setChipSelectPolarity(uint8_t cs, uint8_t active){
}


// safe read from peripheral
uint32_t ch_peri_read(volatile uint32_t* paddr){
    uint32_t ret = *paddr;
    ret = *paddr;
    return ret;
}

// read from peripheral without the read barrier
uint32_t ch_peri_read_nb(volatile uint32_t* paddr){
    return *paddr;
}

// safe write to peripheral
void ch_peri_write(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
    *paddr = value;
}

// write to peripheral without the write barrier
void ch_peri_write_nb(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
}

// Set/clear only the bits in value covered by the mask
void ch_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask){
    uint32_t v = ch_peri_read(paddr);
    v = (v & ~mask) | (value & mask);
    ch_peri_write(paddr, v);
}


/********** FUNCTIONS OUTSIDE CLASSES **********/

// Sleep the specified milliseconds
void delay(long millis){
	unistd::usleep(millis*1000);
}

void delayMicroseconds(long micros)
{
    if (micros > 100){
        struct timespec tim, tim2;
        tim.tv_sec = 0;
        tim.tv_nsec = micros * 1000;
        
        if(nanosleep(&tim , &tim2) < 0 )   {
            fprintf(stderr,"Nano sleep system call failed \n");
            exit(1);
        }
    }else{
        struct timeval tNow, tLong, tEnd ;
        
        gettimeofday (&tNow, NULL) ;
        tLong.tv_sec  = micros / 1000000 ;
        tLong.tv_usec = micros % 1000000 ;
        timeradd (&tNow, &tLong, &tEnd) ;
        
        while (timercmp (&tNow, &tEnd, <))
            gettimeofday (&tNow, NULL) ;
    }
}

void pinMode(int pin, Pinmode mode){
	gpio_set_dir(beaglebonePinNumber(pin), (mode == OUTPUT) ? OUTPUT_PIN : INPUT_PIN);
}

void digitalWrite(int pin, int value){
	gpio_set_value(beaglebonePinNumber(pin), value ? P_HIGH : P_LOW);
	delayMicroseconds(1);
	//delay(1);
}


int digitalRead(int pin){
	unsigned int value;
	gpio_get_value(beaglebonePinNumber(pin), &value);
	return value;
}
/*
int analogRead (int pin){

	int value;
	char selected_channel[1];
	char read_values[2];

	if (pin == 0) {
		selected_channel[0] = 0xDC;
	} else if (pin == 1){
		selected_channel[0] = 0x9C;
	} else if (pin == 2){ 
		selected_channel[0] = 0xCC ;
	} else if (pin == 3){ 
		selected_channel[0] = 0x8C;
	} else if (pin == 4){ 
		selected_channel[0] = 0xAC;
	} else if (pin == 5){ 
		selected_channel[0] = 0xEC;
	} else if (pin == 6){ 
		selected_channel[0] = 0xBC;
	} else if (pin == 7){ 
		selected_channel[0] = 0xFC;
	}
	
	Wire.begin();
	Wire.beginTransmission(8); 
	Wire.read_rs(selected_channel, read_values, 2);
	Wire.read_rs(selected_channel, read_values, 2);

	value = int(read_values[0])*16 + int(read_values[1]>>4);
	value = value * 1023 / 4095;  //mapping the value between 0 and 1023
	return value;
}
*/
long millis(){
	long elapsedTime;
	// stop timer
    gettimeofday(&end_point, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (end_point.tv_sec - start_program.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (end_point.tv_usec - start_program.tv_usec) / 1000.0;   // us to ms
    return elapsedTime;
}

/* Some helper functions */

int beaglebonePinNumber(int arduinoPin){
	switch(arduinoPin){
		case 2: return  49; break;	// CS1
                //case 3: return  37; break;	// RS1
                //case 4: return  62; break;	// RS2
                case 5: return  51; break;	// CS2
                case 6: return  47; break;	// DIO0_0
                case 7: return  14; break;	// CS0
                case 8: return  46; break;	// DIO1_0
                //case 9: return  63; break;	// RS0
                case 10: return 23; break;	// DIO0_1
                case 11: return 26; break;	// DIO1_1
                case 12: return 45; break;	// DIO0_2
                case 13: return 44; break;	// DIO1_2
		default: return 48; break; // NC
	}
}

void ch_gpio_fsel(uint8_t pin, uint8_t mode){
    // Function selects are 10 pins per 32 bit word, 3 bits per pin
}

SPIPi SPI = SPIPi();
