#include "Base64.h"
#include "SX1272.h"

#define PABOOST

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define STARTING_CHANNEL 4

uint32_t loraChannelArray[] = {CH_04_868, CH_05_868, CH_06_868, CH_07_868,
                               CH_08_868, CH_09_868, CH_10_868, CH_11_868,
                               CH_12_868, CH_13_868, CH_14_868, CH_15_868,
                               CH_16_868, CH_17_868, CH_18_868};

#define GW_AUTO_ACK
#define WITH_DATA_PREFIX
#ifdef WITH_DATA_PREFIX
#  define DATA_PREFIX_0 0xFF
#  define DATA_PREFIX_1 0xFE
#endif
#define MAX_CMD_LENGTH 255

boolean receivedFromLoRa = false;
boolean withAck          = false;

bool radioON = false;

int status_counter = 0;
uint8_t SIFS_cad_number;

uint16_t optBW = 0;
uint8_t optSF  = 0;
uint8_t optCR  = 0;
double optFQ   = -1.0;
uint8_t optSW  = 0x12;

void startConfig(int channel, int mode, int addr, int dbm) {

  int e;

  if (optBW != 0 || optCR != 0 || optSF != 0) {
    e = sx1272.setCR(optCR - 4);
    e = sx1272.setSF(optSF);
    e = sx1272.setBW((optBW == 125) ? BW_125
                                    : ((optBW == 250) ? BW_250 : BW_500));
    if (optSF < 10)
      SIFS_cad_number = 6;
    else
      SIFS_cad_number = 3;
  } else {
    e = sx1272.setMode(mode);

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
    printf("%s", "^$Channel CH_18_868: state ");
  } else {
    e = sx1272.setChannel(loraChannelArray[channel]);

    if (optFQ > 0.0) {
      printf("^$Frequency %f: state ", optFQ);
    } else {
      if (channel > 5)
        printf("^$Channel CH_1%d", channel - 6);
      else
        printf("^$Channel CH_0%d", channel + STARTING_CHANNEL);
      printf("_868: state ");
    }
  }
  printf("%d\r\n", e);
  PRINTLN;

#ifdef PABOOST
  sx1272._needPABOOST = true;
#endif

  e = sx1272.setPowerDBM((uint8_t)dbm);
  e = sx1272.getPreambleLength();
  if (sx1272._preamblelength != 8) {
    sx1272.setPreambleLength(8);
    e = sx1272.getPreambleLength();
  }

  sx1272._nodeAddress = addr;
  sx1272._rawFormat   = true;
}

void setup(char *devpath, int channel, int mode, int addr, int dbm) {
  int e;
  srand(time(NULL));

  e = sx1272.ON(devpath);
  e = sx1272.getSyncWord();
  if (optSW != 0x12) {
    e = sx1272.setSyncWord(optSW);
  }

  if (!e) {
    radioON = true;
    startConfig(channel, mode, addr, dbm);
  }
  delay(1000);
}

void loop(char *devpath, int channel, int mode, int addr, int dbm) {
  int i = 0, e;
  int cmdValue;

  receivedFromLoRa = false;

  if (radioON) {

    e = 1;

    if (status_counter == 60 || status_counter == 0) {
      printf("%s", "^$Low-level gw status ON");
      status_counter = 0;
    }

#ifdef GW_AUTO_ACK

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
    }

    if (!e && sx1272._requestACK_indicator) {
      printf("%s %d", "^$ACK requested by", sx1272.packet_received.src);
    }
#else
    // OBSOLETE normally we always use GW_AUTO_ACK
    // Receive message
    if (withAck)
      e = sx1272.receivePacketTimeoutACK(MAX_TIMEOUT);
    else
      e = sx1272.receivePacketTimeout(MAX_TIMEOUT);
#endif

    if (!e) {

      int a = 0, b = 0;
      uint8_t tmp_length;

      receivedFromLoRa = true;

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

      tmp_length = sx1272._payloadlength;

      sx1272.getSNR();
      sx1272.getRSSIpacket();

      // Renha: in base64
      int b64_length = tmp_length;
      if (sx1272.packet_received.data[0] == 0xff)
        b64_length = base64_enc_len(sx1272._payloadlength - 1) + 2;

      printf("--- rxlora. dst=%d type=0x%.2X src=%d seq=%d",
             sx1272.packet_received.dst, sx1272.packet_received.type,
             sx1272.packet_received.src, sx1272.packet_received.packnum);

      printf(" len=%d SNR=%d RSSIpkt=%d BW=%d CR=4/%d SF=%d\n",
             b64_length, // tmp_length,
             sx1272._SNR, sx1272._RSSIpacket,
             (sx1272._bandwidth == BW_125)
                 ? 125
                 : ((sx1272._bandwidth == BW_250) ? 250 : 500),
             sx1272._codingRate + 4, sx1272._spreadingFactor);

      printf("^p%d,%d,%d,%d,%d,%d,%d\n", sx1272.packet_received.dst,
             sx1272.packet_received.type, sx1272.packet_received.src,
             sx1272.packet_received.packnum,
             b64_length, // tmp_length,
             sx1272._SNR, sx1272._RSSIpacket);

      sprint("^r%d,%d,%d\n",
             (sx1272._bandwidth == BW_125)
                 ? 125
                 : ((sx1272._bandwidth == BW_250) ? 250 : 500),
             sx1272._codingRate + 4, sx1272._spreadingFactor);

#if defined WITH_DATA_PREFIX
      printf("%c%c", (char)DATA_PREFIX_0, (char)DATA_PREFIX_1);
#endif

      // print to stdout the content of the packet
      // Renha: in base64
      // two first bytes are generated, not from encoding
      if (sx1272.packet_received.data[0] == 0xff) {
        char base64buf[1024];
        cmd[b++] = '\\';
        cmd[b++] = '!';
        printf("%s", "\\!");
        int base64len = base64_enc_len(tmp_length - a - 1);
        base64_encode(base64buf, (char *)(sx1272.packet_received.data + a + 1),
                      tmp_length - a - 1);
        base64buf[base64len] = 0x00;
        printf("%s", base64buf);
        for (int i = 0; i < base64len && b < MAX_CMD_LENGTH; i++, b++) {
          cmd[b] = base64buf[i];
        }
      } else {
        for (; a < tmp_length; a++, b++) {
          printf("%c", (char)sx1272.packet_received.data[a]);

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

  // while (setup(devpath, channel, mode, addr, dbm))

  setup(devpath, channel, mode, addr, dbm);

  while (1) {
    loop(devpath, channel, mode, addr, dbm);
  }

  return (0);
}