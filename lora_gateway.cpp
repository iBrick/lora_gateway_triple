/*
 *  LoRa low-level gateway to receive and send command
 *
 *  Copyright (C) 2015-2017 Congduc Pham
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************
 *  Version:                1.8
 *  Design:                 C. Pham
 *  Implementation:         C. Pham
 */

// Include the SX1272
#include "SX1272.h"

#include "Base64.h"

#define PABOOST

// IMPORTANT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// please uncomment only 1 choice
#define BAND868
//#define BAND900
//#define BAND433
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define PRINTLN printf("\n")
#define PRINT_CSTSTR(fmt, param) printf(fmt, param)
#define PRINT_STR(fmt, param) PRINT_CSTSTR(fmt, param)
#define PRINT_VALUE(fmt, param) PRINT_CSTSTR(fmt, param)
#define PRINT_HEX(fmt, param) PRINT_VALUE(fmt, param)
#define FLUSHOUTPUT fflush(stdout);

#ifdef DEBUG
#  define DEBUGLN PRINTLN
#  define DEBUG_CSTSTR(fmt, param) PRINT_CSTSTR(fmt, param)
#  define DEBUG_STR(fmt, param) PRINT_CSTSTR(fmt, param)
#  define DEBUG_VALUE(fmt, param) PRINT_VALUE(fmt, param)
#else
#  define DEBUGLN
#  define DEBUG_CSTSTR(fmt, param)
#  define DEBUG_STR(fmt, param)
#  define DEBUG_VALUE(fmt, param)
#endif

//#define SHOW_FREEMEMORY
//#define GW_RELAY
//#define RECEIVE_ALL
//#define CAD_TEST
//#define LORA_LAS
//#define WINPUT
//#define ENABLE_REMOTE

#ifdef BAND868
#  define MAX_NB_CHANNEL 15
#  define STARTING_CHANNEL 4
#  define ENDING_CHANNEL 18

uint32_t loraChannelArray[MAX_NB_CHANNEL] = {
    CH_04_868, CH_05_868, CH_06_868, CH_07_868, CH_08_868,
    CH_09_868, CH_10_868, CH_11_868, CH_12_868, CH_13_868,
    CH_14_868, CH_15_868, CH_16_868, CH_17_868, CH_18_868};

#elif defined BAND900
#  define MAX_NB_CHANNEL 13
#  define STARTING_CHANNEL 0
#  define ENDING_CHANNEL 12
uint32_t loraChannelArray[MAX_NB_CHANNEL] = {
    CH_00_900, CH_01_900, CH_02_900, CH_03_900, CH_04_900, CH_05_900, CH_06_900,
    CH_07_900, CH_08_900, CH_09_900, CH_10_900, CH_11_900, CH_12_900};
#elif defined BAND433
#  define MAX_NB_CHANNEL 4
#  define STARTING_CHANNEL 0
#  define ENDING_CHANNEL 3
uint32_t loraChannelArray[MAX_NB_CHANNEL] = {CH_00_433, CH_01_433, CH_02_433,
                                             CH_03_433};
#endif

// use the dynamic ACK feature of our modified SX1272 lib
#define GW_AUTO_ACK

// will use 0xFF0xFE to prefix data received from LoRa, so that post-processing
// stage can differenciate data received from radio
#define WITH_DATA_PREFIX

#ifdef WITH_DATA_PREFIX
#  define DATA_PREFIX_0 0xFF
#  define DATA_PREFIX_1 0xFE
#endif

#ifdef LORA_LAS
#  include "LoRaActivitySharing.h"
// acting as the LR-BS
LASBase loraLAS = LASBase();
#endif

///////////////////////////////////////////////////////////////////
// CONFIGURATION VARIABLES
//
#ifndef ARDUINO
char keyPressBuff[30];
uint8_t keyIndex = 0;
int ch;
#endif

// be careful, max command length is 60 characters
#define MAX_CMD_LENGTH 60

char cmd[MAX_CMD_LENGTH] = "****************";
int msg_sn               = 0;

// number of retries to unlock remote configuration feature
uint8_t unlocked_try       = 3;
boolean unlocked           = false;
boolean receivedFromSerial = false;
boolean receivedFromLoRa   = false;
boolean withAck            = false;

bool radioON    = false;
bool RSSIonSend = true;

#if defined PABOOST || defined RADIO_RFM92_95 || defined RADIO_INAIR9B ||      \
    defined RADIO_20DBM
// HopeRF 92W/95W and inAir9B need the PA_BOOST
// so 'x' set the PA_BOOST but then limit the power to +14dBm
char loraPower = 'x';
#else
// other radio board such as Libelium LoRa or inAir9 do not need the PA_BOOST
// so 'M' set the output power to 15 to get +14dBm
char loraPower = 'M';
#endif

int status_counter = 0;
unsigned long startDoCad, endDoCad;
bool extendedIFS = true;
uint8_t SIFS_cad_number;
// set to 0 to disable carrier sense based on CAD
uint8_t send_cad_number = 3;
uint8_t SIFS_value[11]  = {0, 183, 94, 44, 47, 23, 24, 12, 12, 7, 4};
uint8_t CAD_value[11]   = {0, 62, 31, 16, 16, 8, 9, 5, 3, 1, 1};

bool optAESgw  = false;
uint16_t optBW = 0;
uint8_t optSF  = 0;
uint8_t optCR  = 0;
uint8_t optCH  = 0;
bool optRAW    = false;
double optFQ   = -1.0;
uint8_t optSW  = 0x12;
///////////////////////////////////////////////////////////////////

#if defined ARDUINO && defined SHOW_FREEMEMORY && not defined __MK20DX256__ && \
    not defined __MKL26Z64__ && not defined __SAMD21G18A__ &&                  \
    not defined _VARIANT_ARDUINO_DUE_X_
int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
#endif

long getCmdValue(int &i, char *strBuff = NULL) {

  char seqStr[7] = "******";

  int j = 0;
  // character '#' will indicate end of cmd value
  while ((char)cmd[i] != '#' && (i < strlen(cmd)) && j < strlen(seqStr)) {
    seqStr[j] = (char)cmd[i];
    i++;
    j++;
  }

  // put the null character at the end
  seqStr[j] = '\0';

  if (strBuff) {
    strcpy(strBuff, seqStr);
  } else
    return (atol(seqStr));
}

void startConfig(int channel, int mode, int addr, int dbm) {

  int e;

  // has customized LoRa settings
  if (optBW != 0 || optCR != 0 || optSF != 0) {

    e = sx1272.setCR(optCR - 4);
    PRINT_CSTSTR("%s", "^$LoRa CR ");
    PRINT_VALUE("%d", optCR);
    PRINT_CSTSTR("%s", ": state ");
    PRINT_VALUE("%d", e);
    PRINTLN;

    e = sx1272.setSF(optSF);
    PRINT_CSTSTR("%s", "^$LoRa SF ");
    PRINT_VALUE("%d", optSF);
    PRINT_CSTSTR("%s", ": state ");
    PRINT_VALUE("%d", e);
    PRINTLN;

    e = sx1272.setBW((optBW == 125) ? BW_125
                                    : ((optBW == 250) ? BW_250 : BW_500));
    PRINT_CSTSTR("%s", "^$LoRa BW ");
    PRINT_VALUE("%d", optBW);
    PRINT_CSTSTR("%s", ": state ");
    PRINT_VALUE("%d", e);
    PRINTLN;

    if (optSF < 10)
      SIFS_cad_number = 6;
    else
      SIFS_cad_number = 3;
  } else {

    // Set transmission mode and print the result
    PRINT_CSTSTR("%s", "^$LoRa mode ");
    PRINT_VALUE("%d", mode);
    PRINTLN;

    e = sx1272.setMode(mode);
    PRINT_CSTSTR("%s", "^$Setting mode: state ");
    PRINT_VALUE("%d", e);
    PRINTLN;

#ifdef LORA_LAS
    loraLAS.setSIFS(mode);
#endif

    if (mode > 7)
      SIFS_cad_number = 6;
    else
      SIFS_cad_number = 3;
  }

  // Select frequency channel
  if (mode == 11) {
    // if we start with mode 11, then switch to 868.1MHz for LoRaWAN test
    // Note: if you change to mode 11 later using command /@M11# for instance,
    // you have to use /@C18# to change to the correct channel
    e = sx1272.setChannel(CH_18_868);
    PRINT_CSTSTR("%s", "^$Channel CH_18_868: state ");
  } else {
    e = sx1272.setChannel(loraChannelArray[channel]);

    if (optFQ > 0.0) {
      PRINT_CSTSTR("%s", "^$Frequency ");
      PRINT_VALUE("%f", optFQ);
      PRINT_CSTSTR("%s", ": state ");
    } else {
#ifdef BAND868
      if (channel > 5) {
        PRINT_CSTSTR("%s", "^$Channel CH_1");
        PRINT_VALUE("%d", channel - 6);
      } else {
        PRINT_CSTSTR("%s", "^$Channel CH_0");
        PRINT_VALUE("%d", channel + STARTING_CHANNEL);
      }
      PRINT_CSTSTR("%s", "_868: state ");
#elif defined BAND900
      PRINT_CSTSTR("%s", "^$Channel CH_");
      PRINT_VALUE("%d", channel);
      PRINT_CSTSTR("%s", "_900: state ");
#elif defined BAND433
      // e = sx1272.setChannel(0x6C4000);
      PRINT_CSTSTR("%s", "^$Channel CH_");
      PRINT_VALUE("%d", channel);
      PRINT_CSTSTR("%s", "_433: state ");
#endif
    }
  }
  PRINT_VALUE("%d", e);
  PRINTLN;

  // Select amplifier line; PABOOST or RFO
#ifdef PABOOST
  sx1272._needPABOOST = true;
  // previous way for setting output power
  // loraPower='x';
  PRINT_CSTSTR("%s", "^$Use PA_BOOST amplifier line");
  PRINTLN;
#else
  // previous way for setting output power
  // loraPower='M';
#endif

  // Select output power in dBm
  e = sx1272.setPowerDBM((uint8_t)dbm);

  PRINT_CSTSTR("%s", "^$Set LoRa power dBm to ");
  PRINT_VALUE("%d", (uint8_t)dbm);
  PRINTLN;

  PRINT_CSTSTR("%s", "^$Power: state ");
  PRINT_VALUE("%d", e);
  PRINTLN;

  // get preamble length
  e = sx1272.getPreambleLength();
  PRINT_CSTSTR("%s", "^$Get Preamble Length: state ");
  PRINT_VALUE("%d", e);
  PRINTLN;
  PRINT_CSTSTR("%s", "^$Preamble Length: ");
  PRINT_VALUE("%d", sx1272._preamblelength);
  PRINTLN;

  if (sx1272._preamblelength != 8) {
    PRINT_CSTSTR("%s", "^$Bad Preamble Length: set back to 8");
    sx1272.setPreambleLength(8);
    e = sx1272.getPreambleLength();
    PRINT_CSTSTR("%s", "^$Get Preamble Length: state ");
    PRINT_VALUE("%d", e);
    PRINTLN;
    PRINT_CSTSTR("%s", "^$Preamble Length: ");
    PRINT_VALUE("%d", sx1272._preamblelength);
    PRINTLN;
  }

  // Set the node address and print the result
  // e = sx1272.setNodeAddress(addr);
  sx1272._nodeAddress = addr;
  e                   = 0;
  PRINT_CSTSTR("%s", "^$LoRa addr ");
  PRINT_VALUE("%d", addr);
  PRINT_CSTSTR("%s", ": state ");
  PRINT_VALUE("%d", e);
  PRINTLN;

  if (optAESgw)
    PRINT_CSTSTR("%s", "^$Handle AES encrypted data\n");

  if (optRAW) {
    PRINT_CSTSTR("%s", "^$Raw format, not assuming any header in reception\n");
    // when operating n raw format, the SX1272 library do not decode the packet
    // header but will pass all the payload to stdout note that in this case,
    // the gateway may process packet that are not addressed explicitly to it as
    // the dst field is not checked at all this would be similar to a
    // promiscuous sniffer, but most of real LoRa gateway works this way
    sx1272._rawFormat = true;
  }

  // Print a success message
  PRINT_CSTSTR("%s", "^$SX1272/76 configured ");
  PRINT_CSTSTR("%s",
               "as LR-BS. Waiting RF input for transparent RF-serial bridge\n");
#if defined ARDUINO && defined GW_RELAY
  PRINT_CSTSTR("%s", "^$Act as a simple relay gateway\n");
#endif
}

void setup(char *devpath, int channel, int mode, int addr, int dbm) {
  int e;
  srand(time(NULL));

  // Power ON the module
  e = sx1272.ON(devpath);

  PRINT_CSTSTR("%s", "^$**********Power ON: state ");
  PRINT_VALUE("%d", e);
  PRINTLN;

  e = sx1272.getSyncWord();

  if (!e) {
    PRINT_CSTSTR("%s", "^$Default sync word: 0x");
    PRINT_HEX("%X", sx1272._syncWord);
    PRINTLN;
  }

  if (optSW != 0x12) {
    e = sx1272.setSyncWord(optSW);

    PRINT_CSTSTR("%s", "^$Set sync word to 0x");
    PRINT_HEX("%X", optSW);
    PRINTLN;
    PRINT_CSTSTR("%s", "^$LoRa sync word: state ");
    PRINT_VALUE("%d", e);
    PRINTLN;
  }

  if (!e) {
    radioON = true;
    startConfig(channel, mode, addr, dbm);
  }

  FLUSHOUTPUT;
  delay(1000);

#ifdef LORA_LAS
  loraLAS.ON(LAS_ON_WRESET);
#endif

#ifdef CAD_TEST
  PRINT_CSTSTR("%s", "Do CAD test\n");
#endif
}

void loop(char *devpath, int channel, int mode, int addr, int dbm) {
  int i = 0, e;
  int cmdValue;

///////////////////////////////////////////////////////////////////
// ONLY FOR TESTING CAD
#ifdef CAD_TEST

  startDoCad = millis();
  e          = sx1272.doCAD(SIFS_cad_number);
  endDoCad   = millis();

  PRINT_CSTSTR("%s", "--> SIFS duration ");
  PRINT_VALUE("%ld", endDoCad - startDoCad);
  PRINTLN;

  if (!e)
    PRINT_CSTSTR("%s", "OK");
  else
    PRINT_CSTSTR("%s", "###");

  PRINTLN;

  delay(200);

  startDoCad = millis();
  e          = sx1272.doCAD(SIFS_cad_number * 3);
  endDoCad   = millis();

  PRINT_CSTSTR("%s", "--> DIFS duration ");
  PRINT_VALUE("%ld", endDoCad - startDoCad);
  PRINTLN;

  if (!e)
    PRINT_CSTSTR("%s", "OK");
  else
    PRINT_CSTSTR("%s", "###");

  PRINTLN;

  delay(200);
#endif
  // ONLY FOR TESTING CAD
  /// END/////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // START OF PERIODIC TASKS

  receivedFromSerial = false;
  receivedFromLoRa   = false;

#ifdef LORA_LAS
  // call periodically to be able to detect the start of a new cycle
  loraLAS.checkCycle();
#endif

  ///////////////////////////////////////////////////////////////////
  // THE MAIN PACKET RECEPTION LOOP
  //
  if (radioON && !receivedFromSerial) {

    e = 1;
#ifndef CAD_TEST

    if (status_counter == 60 || status_counter == 0) {
      PRINT_CSTSTR("%s", "^$Low-level gw status ON");
      PRINTLN;
      FLUSHOUTPUT;
      status_counter = 0;
    }

    // check if we received data from the receiving LoRa module
#  ifdef RECEIVE_ALL
    e = sx1272.receiveAll(MAX_TIMEOUT);
#  else
#    ifdef GW_AUTO_ACK

    e = sx1272.receivePacketTimeout(MAX_TIMEOUT);

    status_counter++;
    if (e != 0 && e != 3) {
      PRINT_CSTSTR("%s", "^$Receive error ");
      PRINT_VALUE("%d", e);
      PRINTLN;

      if (e == 2) {
        // Power OFF the module
        sx1272.OFF();
        radioON = false;
        PRINT_CSTSTR("%s", "^$Resetting radio module");
        PRINTLN;
        e = sx1272.ON(devpath);
        PRINT_CSTSTR("%s", "^$Setting power ON: state ");
        PRINT_VALUE("%d", e);
        PRINTLN;

        if (!e) {
          radioON = true;
          startConfig(channel, mode, addr, dbm);
        }
        // to start over
        status_counter = 0;
        e              = 1;
      }
      FLUSHOUTPUT;
    }

    if (!e && sx1272._requestACK_indicator) {
      PRINT_CSTSTR("%s", "^$ACK requested by ");
      PRINT_VALUE("%d", sx1272.packet_received.src);
      PRINTLN;
      FLUSHOUTPUT;
    }
#    else
    // OBSOLETE normally we always use GW_AUTO_ACK
    // Receive message
    if (withAck)
      e = sx1272.receivePacketTimeoutACK(MAX_TIMEOUT);
    else
      e = sx1272.receivePacketTimeout(MAX_TIMEOUT);
#    endif
#  endif
#endif
    ///////////////////////////////////////////////////////////////////

    if (!e) {

      int a = 0, b = 0;
      uint8_t tmp_length;

      receivedFromLoRa = true;

      ///////////////////////////////////////////////////////////////////
      // for Linux-based gateway only
      // provide reception timestamp

#if not defined ARDUINO && not defined GW_RELAY
      char time_buffer[30];
      int millisec;
      struct tm *tm_info;
      struct timeval tv;

      gettimeofday(&tv, NULL);

      millisec = lrint(tv.tv_usec / 1000.0); // Round to nearest millisec

      if (millisec >= 1000) { // Allow for rounding up to nearest second
        millisec -= 1000;
        tv.tv_sec++;
      }

      tm_info = localtime(&tv.tv_sec);
#endif

      tmp_length = sx1272._payloadlength;

#if not defined GW_RELAY

      sx1272.getSNR();
      sx1272.getRSSIpacket();

      // Renha: in base64
      int b64_length = tmp_length;
      if (sx1272.packet_received.data[0] == 0xff)
        b64_length = base64_enc_len(sx1272._payloadlength - 1) + 2;

      // we split in 2 parts to use a smaller buffer size
      sprintf(cmd, "--- rxlora. dst=%d type=0x%.2X src=%d seq=%d",
              sx1272.packet_received.dst, sx1272.packet_received.type,
              sx1272.packet_received.src, sx1272.packet_received.packnum);

      PRINT_STR("%s", cmd);

      sprintf(cmd, " len=%d SNR=%d RSSIpkt=%d BW=%d CR=4/%d SF=%d\n",
              b64_length, // tmp_length,
              sx1272._SNR, sx1272._RSSIpacket,
              (sx1272._bandwidth == BW_125)
                  ? 125
                  : ((sx1272._bandwidth == BW_250) ? 250 : 500),
              sx1272._codingRate + 4, sx1272._spreadingFactor);

      PRINT_STR("%s", cmd);

      // provide a short output for external program to have information about
      // the received packet ^psrc_id,seq,len,SNR,RSSI
      sprintf(cmd, "^p%d,%d,%d,%d,%d,%d,%d\n", sx1272.packet_received.dst,
              sx1272.packet_received.type, sx1272.packet_received.src,
              sx1272.packet_received.packnum,
              b64_length, // tmp_length,
              sx1272._SNR, sx1272._RSSIpacket);

      PRINT_STR("%s", cmd);

      // ^rbw,cr,sf
      sprintf(cmd, "^r%d,%d,%d\n",
              (sx1272._bandwidth == BW_125)
                  ? 125
                  : ((sx1272._bandwidth == BW_250) ? 250 : 500),
              sx1272._codingRate + 4, sx1272._spreadingFactor);

      PRINT_STR("%s", cmd);
#endif
      ///////////////////////////////////////////////////////////////////

#if not defined ARDUINO && not defined GW_RELAY
      strftime(time_buffer, 30, "%Y-%m-%dT%H:%M:%S", tm_info);
      sprintf(cmd, "^t%s.%03d\n", time_buffer, millisec);
      PRINT_STR("%s", cmd);
#endif

#ifdef LORA_LAS
      if (loraLAS.isLASMsg(sx1272.packet_received.data)) {

        // tmp_length=sx1272.packet_received.length-OFFSET_PAYLOADLENGTH;
        tmp_length = sx1272._payloadlength;

        int v = loraLAS.handleLASMsg(sx1272.packet_received.src,
                                     sx1272.packet_received.data, tmp_length);

        if (v == DSP_DATA) {
          a = LAS_DSP + DATA_HEADER_LEN + 1;
#  ifdef WITH_DATA_PREFIX
          PRINT_STR("%c", (char)DATA_PREFIX_0);
          PRINT_STR("%c", (char)DATA_PREFIX_1);
#  endif
        } else
          // don't print anything
          a = tmp_length;
      } else
        PRINT_CSTSTR("%s", "No LAS header. Write raw data\n");
#else
#  if defined WITH_DATA_PREFIX && not defined GW_RELAY
      PRINT_STR("%c", (char)DATA_PREFIX_0);
      PRINT_STR("%c", (char)DATA_PREFIX_1);
#  endif
#endif

      // print to stdout the content of the packet
      // Renha: in base64
      // two first bytes are generated, not from encoding
      if (sx1272.packet_received.data[0] == 0xff) {
        char base64buf[1024];
        cmd[b++] = '\\';
        cmd[b++] = '!';
        PRINT_CSTSTR("%s", "\\!");
        int base64len = base64_enc_len(tmp_length - a - 1);
        base64_encode(base64buf, (char *)(sx1272.packet_received.data + a + 1),
                      tmp_length - a - 1);
        base64buf[base64len] = 0x00;
        PRINT_STR("%s", base64buf);
        for (int i = 0; i < base64len && b < MAX_CMD_LENGTH; i++, b++) {
          cmd[b] = base64buf[i];
        }
      } else {
        for (; a < tmp_length; a++, b++) {
          PRINT_STR("%c", (char)sx1272.packet_received.data[a]);

          if (b < MAX_CMD_LENGTH)
            cmd[b] = (char)sx1272.packet_received.data[a];
        }
      }
      // strlen(cmd) will be correct as only the payload is copied
      cmd[b] = '\0';
      PRINTLN;
      FLUSHOUTPUT;
    }
  }
}

int xtoi(const char *hexstring) {
  int i = 0;

  if ((*hexstring == '0') && (*(hexstring + 1) == 'x'))
    hexstring += 2;
  while (*hexstring) {
    char c = toupper(*hexstring++);
    if ((c < '0') || (c > 'F') || ((c > '9') && (c < 'A')))
      break;
    c -= '0';
    if (c > 9)
      c -= 7;
    i = (i << 4) + c;
  }
  return i;
}

int main(int argc, char *argv[]) {

  if (argc < 6) {
    printf("Usage: sudo %s %s %s %s %s %s\r\n\r\n%s\r\n", argv[0], "spi_device",
           "lora_channel(index in array below, recommended 4,6,8)", "lora_mode",
           "lora_addr", "max_dbm(14 recommended)",
           "CH_04_868, CH_05_868, CH_06_868, CH_07_868, CH_08_868,\
        \r\nCH_09_868, CH_10_868, CH_11_868, CH_12_868, CH_13_868,\
        \r\nCH_14_868, CH_15_868, CH_16_868, CH_17_868, CH_18_868");
    return (-1);
  }

  char *devpath = argv[1];
  int channel   = atoi(argv[2]);
  int mode      = atoi(argv[3]);
  int addr      = atoi(argv[4]);
  int dbm       = atoi(argv[5]);

  // while (setup_radio(devpath, channel, mode, addr, dbm))

  setup(devpath, channel, mode, addr, dbm);

  while (1) {
    loop(devpath, channel, mode, addr, dbm);
  }

  return (0);
}