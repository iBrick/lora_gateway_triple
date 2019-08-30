include radio.makefile
# 868.3 2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C
debug = $(-DSX1272_debug_mode=2)

lora_gateway_full: uplinker downlinker uplinker_light

downlinker: downlinker.o arduPi_bb.o SX1272_bb.o SPI.o GPIO.o Base64.o
	g++ $(debug) -lrt -lpthread SPI.o GPIO.o downlinker.o Base64.o arduPi_bb.o SX1272_bb.o -o downlinker

downlinker.o: downlinker.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c downlinker.cpp -o downlinker.o

GPIO.o: GPIO.cpp GPIO.h
	g++ $(debug) -lrt -lpthread -c GPIO.cpp -o GPIO.o

SPI.o: SPI.c SPI.h
	g++ $(debug) -lrt -lpthread -c SPI.c -o SPI.o

Base64.o: Base64.h Base64.cpp
	g++ $(debug) -lrt -lpthread -c Base64.cpp -o Base64.o

uplinker: lora_gateway.o Base64.o arduPi_bb.o SX1272_bb.o SPI.o GPIO.o
	g++ $(debug) -lrt -lpthread SPI.o GPIO.o lora_gateway.o Base64.o arduPi_bb.o SX1272_bb.o -o uplinker	

uplinker_light: uplinker.o arduPi_bb.o SX1272_bb.o SPI.o GPIO.o Base64.o
	g++ $(debug) -lrt -lpthread SPI.o GPIO.o uplinker.o Base64.o arduPi_bb.o SX1272_bb.o -o uplinker_light

uplinker.o: uplinker.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c uplinker.cpp -o uplinker.o

lora_gateway.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c lora_gateway.cpp -o lora_gateway.o

arduPi.o: arduPi.cpp arduPi.h
	g++ $(debug) -c arduPi.cpp -o arduPi.o	

arduPi_bb.o: arduPi_bb.cpp arduPi_bb.h SPI.o
	g++ $(debug) -l SPI.o -l GPIO.o -c arduPi_bb.cpp -o arduPi_bb.o

SX1272.o: SX1272.cpp SX1272.h
	g++ $(debug) -c SX1272.cpp -o SX1272.o

SX1272_bb.o: SX1272.cpp SX1272.h
	g++ $(debug) -DBEAGLEBONE -c SX1272.cpp -o SX1272_bb.o

SX1272_wnetkey.o: SX1272.cpp
	g++ $(debug) -DW_NET_KEY -c SX1272.cpp -o SX1272_wnetkey.o

SX1272_pi2_wnetkey.o: SX1272.cpp
	g++ $(debug) -DRASPBERRY2 -DW_NET_KEY -c SX1272.cpp -o SX1272_pi2_wnetkey.o

lora_las_gateway: lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272.o
	g++ $(debug) -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272.o -o lora_las_gateway

lora_las_gateway_pi2: lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2.o
	g++ $(debug) -lrt -lpthread lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2.o -o lora_las_gateway_pi2

lora_las_gateway_wnetkey: lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272_wnetkey.o
	g++ $(debug) -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272_wnetkey.o -o lora_las_gateway_wnetkey

lora_las_gateway_pi2_wnetkey: lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2_wnetkey.o
	g++ $(debug) -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2_wnetkey.o -o lora_las_gateway_pi2_wnetkey
	
lora_las_gateway.o: lora_gateway.cpp
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DIS_RCV_GATEWAY -DLORA_LAS -c lora_gateway.cpp -o lora_las_gateway.o

lora_las_gateway_pi2.o: lora_gateway.cpp
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DRASPBERRY2 -DIS_RCV_GATEWAY -DLORA_LAS -c lora_gateway.cpp -o lora_las_gateway_pi2.o

LoRaActivitySharing.o: LoRaActivitySharing.cpp LoRaActivitySharing.h
	g++ $(debug) -c LoRaActivitySharing.cpp -o LoRaActivitySharing.o
		
lib: arduPi.o arduPi_pi2.o SX1272.o SX1272_pi2.o SX1272_wnetkey.o SX1272_pi2_wnetkey.o lora_gateway.o lora_las_gateway.o lora_gateway_pi2.o lora_las_gateway_pi2.o

clean:
	rm *.o
