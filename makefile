include radio.makefile

debug = $(-DSX1272_debug_mode=2)

lora_gateway_full: uplinker downlinker

downlinker: downlinker.o arduPi_bb.o SX1272_bb.o SPI.o GPIO.o Base64.o
	g++ -lrt -lpthread SPI.o GPIO.o downlinker.o Base64.o arduPi_bb.o SX1272_bb.o -o downlinker

downlinker.o: downlinker.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c downlinker.cpp -o downlinker.o

GPIO.o: GPIO.cpp GPIO.h
	g++ -lrt -lpthread -c GPIO.cpp -o GPIO.o

SPI.o: SPI.c SPI.h
	g++ -lrt -lpthread -c SPI.c -o SPI.o

Base64.o: Base64.h Base64.cpp
	g++ -lrt -lpthread -c Base64.cpp -o Base64.o

uplinker: lora_gateway.o arduPi_bb.o SX1272_bb.o SPI.o GPIO.o
	g++ -lrt -lpthread SPI.o GPIO.o lora_gateway.o Base64.o arduPi_bb.o SX1272_bb.o -o uplinker	

lora_gateway.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) $(debug) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c lora_gateway.cpp -o lora_gateway.o

arduPi.o: arduPi.cpp arduPi.h
	g++ -c arduPi.cpp -o arduPi.o	

arduPi_bb.o: arduPi_bb.cpp arduPi_bb.h SPI.o
	g++ -l SPI.o -l GPIO.o -c arduPi_bb.cpp -o arduPi_bb.o

SX1272.o: SX1272.cpp SX1272.h
	g++ -c SX1272.cpp -o SX1272.o

SX1272_bb.o: SX1272.cpp SX1272.h
	g++ $(debug) -DBEAGLEBONE -c SX1272.cpp -o SX1272_bb.o

SX1272_wnetkey.o: SX1272.cpp
	g++ -DW_NET_KEY -c SX1272.cpp -o SX1272_wnetkey.o

SX1272_pi2_wnetkey.o: SX1272.cpp
	g++ -DRASPBERRY2 -DW_NET_KEY -c SX1272.cpp -o SX1272_pi2_wnetkey.o

lora_las_gateway: lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272.o
	g++ -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272.o -o lora_las_gateway

lora_las_gateway_pi2: lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2.o
	g++ -lrt -lpthread lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2.o -o lora_las_gateway_pi2

lora_las_gateway_wnetkey: lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272_wnetkey.o
	g++ -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi.o SX1272_wnetkey.o -o lora_las_gateway_wnetkey

lora_las_gateway_pi2_wnetkey: lora_las_gateway_pi2.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2_wnetkey.o
	g++ -lrt -lpthread lora_las_gateway.o LoRaActivitySharing.o arduPi_pi2.o SX1272_pi2_wnetkey.o -o lora_las_gateway_pi2_wnetkey
	
lora_las_gateway.o: lora_gateway.cpp
	g++ $(CFLAGS) -DRASPBERRY -DIS_RCV_GATEWAY -DLORA_LAS -c lora_gateway.cpp -o lora_las_gateway.o

lora_las_gateway_pi2.o: lora_gateway.cpp
	g++ $(CFLAGS) -DRASPBERRY -DRASPBERRY2 -DIS_RCV_GATEWAY -DLORA_LAS -c lora_gateway.cpp -o lora_las_gateway_pi2.o

LoRaActivitySharing.o: LoRaActivitySharing.cpp LoRaActivitySharing.h
	g++ -c LoRaActivitySharing.cpp -o LoRaActivitySharing.o
		
lib: arduPi.o arduPi_pi2.o SX1272.o SX1272_pi2.o SX1272_wnetkey.o SX1272_pi2_wnetkey.o lora_gateway.o lora_las_gateway.o lora_gateway_pi2.o lora_las_gateway_pi2.o

clean:
	rm *.o lora_*gateway
