
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

uint32_t loraChannelArray[] = {CH_04_868, CH_05_868, CH_06_868, CH_07_868,
                               CH_08_868, CH_09_868, CH_10_868, CH_11_868,
                               CH_12_868, CH_13_868, CH_14_868, CH_15_868,
                               CH_16_868, CH_17_868, CH_18_868};

int setup_radio(char *devpath, int channel, int mode, int addr, int dbm) {

  printf("%s\r\n", "Started setup process");
  int e;

  e = sx1272.ON(devpath);
  e = sx1272.getSyncWord();

  if (!e) {
    printf("%s\r\n", "Started configuration process");
    int e;

    e = sx1272.setMode(mode);
    e = sx1272.setChannel(loraChannelArray[channel]);
#ifdef PABOOST
    sx1272._needPABOOST = true;
#endif
    e = sx1272.setPowerDBM((uint8_t)dbm);
    e = sx1272.setPreambleLength(8);
    // e = sx1272.setNodeAddress(addr);
    sx1272._nodeAddress = addr;
    // sx1272._rawFormat = true;

    printf("%s\r\n", "SX1272/76 configured");
  }

  delay(1000);
  printf("%s\r\n", "Radio setup finished");
  return e;
}

unsigned char fromfile[2048];
unsigned char decoded[256];
void loop(void) {
  FILE *fp = fopen("/home/pi/lora_gateway/downlink/downlink.txt", "r");
  if (fp) {
    // printf("%s\r\n", "File exists");
    fseek(fp, 0, SEEK_END);
    int size_fromfile = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size_fromfile > 1 && size_fromfile < 2048) {

      // printf("%s\r\n", "File has correct size");
      fread(fromfile, 1, size_fromfile, fp);
      fclose(fp);

      while (size_fromfile && (fromfile[size_fromfile - 1] == '\r' ||
                               fromfile[size_fromfile - 1] == '\n'))
        size_fromfile--;

      if (size_fromfile) {
        int size_decoded =
            base64_decode((char *)decoded, (char *)fromfile, size_fromfile);
        printf("Sending package of %i bytes ... ", size_decoded);
        if (sx1272.sendPacketMAXTimeout(255, decoded, size_decoded)) {
          printf("%s\r\n", "failed, will retry in a second");
          delay(1000);
        } else {
          remove("/home/pi/lora_gateway/downlink/downlink.txt");
          printf("%s\r\n", "done");
        }
      }
    } else {
      fclose(fp);
    }
  }
  delay(10);
  // delay(10000); // debug: send not more often than 10 seconds
}

#include <cstdlib>

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL); // disable buffering // debug

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

  while (setup_radio(devpath, channel, mode, addr, dbm)) {
  }
  while (1) {
    loop();
  }
  return (0);
}
