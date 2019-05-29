
#include "SX1272.h"

#include "Base64.h"

#define BAND868

#include <getopt.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

using namespace std;

#define LORAMODE 1
#define LORA_ADDR 1
#define MAX_DBM 14

#define STARTING_CHANNEL 4
#define ENDING_CHANNEL 18

#ifndef LCI
#  define LCI (6)
#endif
uint8_t loraChannelIndex    = LCI;
uint32_t loraChannelArray[] = {CH_04_868, CH_05_868, CH_06_868, CH_07_868,
                               CH_08_868, CH_09_868, CH_10_868, CH_11_868,
                               CH_12_868, CH_13_868, CH_14_868, CH_15_868,
                               CH_16_868, CH_17_868, CH_18_868};

uint8_t loraMode     = LORAMODE;
uint8_t loraAddr     = LORA_ADDR;
uint32_t loraChannel = loraChannelArray[loraChannelIndex];

void startConfig() {

  int e;

  e = sx1272.setMode(loraMode);

  e = sx1272.setChannel(loraChannel);

#ifdef PABOOST
  sx1272._needPABOOST = true;
#endif

  e = sx1272.setPowerDBM((uint8_t)MAX_DBM);

  e = sx1272.setPreambleLength(8);

  // e = sx1272.setNodeAddress(loraAddr);
  sx1272._nodeAddress = loraAddr;

  // sx1272._rawFormat = true;

  printf("%s", "^$SX1272/76 configured ");
}

int setup_radio() {
  int e;

  e = sx1272.ON();
  e = sx1272.getSyncWord();

  if (!e) {
    startConfig();
  }

  delay(1000);
  return e;
}

unsigned char fromfile[2048];
unsigned char decoded[256];
void loop(void) {
  FILE *fp = fopen("/home/pi/lora_gateway/downlink/downlink.txt", "r");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    int size_fromfile = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size_fromfile > 1 && size_fromfile < 2048) {
      fread(fromfile, 1, size_fromfile, fp);
      fclose(fp);

      while (size_fromfile && (fromfile[size_fromfile - 1] == '\r' ||
                               fromfile[size_fromfile - 1] == '\n'))
        size_fromfile--;

      if (size_fromfile) {
        int size_decoded =
            base64_decode((char *)decoded, (char *)fromfile, size_fromfile);
        sx1272.sendPacketMAXTimeout(255, decoded, size_decoded);

        remove("/home/pi/lora_gateway/downlink/downlink.txt");
      }
    } else
      fclose(fp);
  }
  delay(10);
}

int main(int argc, char *argv[]) {
  while (setup_radio()) {
  }
  while (1) {
    loop();
  }
  return (0);
}
