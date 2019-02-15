include radio.makefile

module0 = -DSX1272_RST=9 -DSX1272_SS=7 -DSX1272_NSS1=2 -DSX1272_NSS2=5 -DMODULE=0 -DSX1272_debug_mode=0
module1 = -DSX1272_RST=3 -DSX1272_SS=2 -DSX1272_NSS1=7 -DSX1272_NSS2=5 -DMODULE=1 -DSX1272_debug_mode=0
module2 = -DSX1272_RST=4 -DSX1272_SS=5 -DSX1272_NSS1=7 -DSX1272_NSS2=2 -DMODULE=2 -DSX1272_debug_mode=0

GPIO.o: GPIO.cpp GPIO.h
	g++ -lrt -lpthread -c GPIO.cpp -o GPIO.o

SPI.o: SPI.c SPI.h
	g++ -lrt -lpthread -c SPI.c -o SPI.o

Base64.o: Base64.h Base64.cpp
	g++ -lrt -lpthread -c Base64.cpp -o Base64.o

lora_gateway_pi2_all_modules: lora_gateway_pi2_module0 lora_gateway_pi2_module1 lora_gateway_pi2_module2

lora_gateway_bb_all_modules: lora_gateway_bb_module0 lora_gateway_bb_module1 lora_gateway_bb_module2

lora_gateway: lora_gateway.o arduPi.o SX1272.o
	g++ -lrt -lpthread lora_gateway.o arduPi.o SX1272.o -o lora_gateway	

lora_gateway_pi2: lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2.o
	g++ -lrt -lpthread lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2.o -o lora_gateway_pi2
	rm -f lora_gateway
	ln -s lora_gateway_pi2 ./lora_gateway

lora_gateway_pi2_module0: lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_module0.o
	g++ -lrt -lpthread lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_module0.o -o lora_gateway_pi2_module0

lora_gateway_pi2_module1: lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_module1.o
	g++ -lrt -lpthread lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_module1.o -o lora_gateway_pi2_module1

lora_gateway_pi2_module2: lora_gateway_pi2_downlink.o arduPi_pi2.o SX1272_pi2_module2.o
	g++ -lrt -lpthread lora_gateway_pi2_downlink.o arduPi_pi2.o SX1272_pi2_module2.o -o lora_gateway_pi2_module2

lora_gateway_bb: lora_gateway_bb.o Base64.o arduPi_bb.o SX1272_bb.o
	g++ -lrt -lpthread lora_gateway_bb.o Base64.o arduPi_bb.o SX1272_bb.o -o lora_gateway_bb
	rm -f lora_gateway
	ln -s lora_gateway_bb ./lora_gateway

lora_gateway_bb_module0: lora_gateway_bb.o arduPi_bb.o SX1272_bb_module0.o SPI.o GPIO.o Base64.o
	g++ -lrt -lpthread SPI.o GPIO.o lora_gateway_bb.o Base64.o arduPi_bb.o SX1272_bb_module0.o -o lora_gateway_bb_module0

lora_gateway_bb_module1: lora_gateway_bb.o arduPi_bb.o SX1272_bb_module1.o SPI.o GPIO.o Base64.o
	g++ -lrt -lpthread SPI.o GPIO.o lora_gateway_bb.o Base64.o arduPi_bb.o SX1272_bb_module1.o -o lora_gateway_bb_module1

lora_gateway_bb_module2: lora_gateway_bb_downlink.o arduPi_bb.o SX1272_bb_module2.o SPI.o GPIO.o Base64.o
	g++ -lrt -lpthread SPI.o GPIO.o lora_gateway_bb_downlink.o Base64.o arduPi_bb.o SX1272_bb_module2.o -o lora_gateway_bb_module2

lora_gateway_wnetkey: lora_gateway.o arduPi.o SX1272_wnetkey.o
	g++ -lrt -lpthread lora_gateway.o arduPi.o SX1272_wnetkey.o -o lora_gateway_wnetkey	

lora_gateway_pi2_wnetkey: lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_wnetkey.o
	g++ -lrt -lpthread lora_gateway_pi2.o arduPi_pi2.o SX1272_pi2_wnetkey.o -o lora_gateway_pi2_wnetkey

lora_gateway_winput: lora_gateway_winput.o arduPi.o SX1272.o
	g++ -lrt -lpthread lora_gateway_winput.o arduPi.o SX1272.o -o lora_gateway_winput

lora_gateway_pi2_winput: lora_gateway_pi2_winput.o arduPi_pi2.o SX1272_pi2.o
	g++ -lrt -lpthread lora_gateway_pi2_winput.o arduPi_pi2.o SX1272_pi2.o -o lora_gateway_pi2_winput

lora_gateway_downlink: lora_gateway_downlink.o arduPi.o SX1272.o
	g++ -lrt -lpthread lora_gateway_downlink.o arduPi.o SX1272.o -o lora_gateway_downlink
	rm -f lora_gateway
	ln -s lora_gateway_downlink ./lora_gateway
	
lora_gateway_pi2_downlink: lora_gateway_pi2_downlink.o arduPi_pi2.o SX1272_pi2.o
	g++ -lrt -lpthread lora_gateway_pi2_downlink.o arduPi_pi2.o SX1272_pi2.o -o lora_gateway_pi2_downlink
	rm -f lora_gateway
	ln -s lora_gateway_pi2_downlink ./lora_gateway
	
lora_gateway_bb_downlink: Base64.o lora_gateway_bb_downlink.o arduPi_bb.o SX1272_bb.o
	g++ -lrt -lpthread lora_gateway_bb_downlink.o Base64.o arduPi_bb.o SX1272_bb.o -o lora_gateway_bb_downlink
	rm -f lora_gateway
	ln -s lora_gateway_bb_downlink ./lora_gateway
	
lora_gateway.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DIS_RCV_GATEWAY -c lora_gateway.cpp -o lora_gateway.o

lora_gateway_pi2.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DRASPBERRY2 -DIS_RCV_GATEWAY -c lora_gateway.cpp -o lora_gateway_pi2.o

lora_gateway_bb.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -c lora_gateway.cpp -o lora_gateway_bb.o

lora_gateway_winput.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DIS_RCV_GATEWAY -DWINPUT -c lora_gateway.cpp -o lora_gateway_winput.o

lora_gateway_pi2_winput.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DRASPBERRY2 -DIS_RCV_GATEWAY -DWINPUT -c lora_gateway.cpp -o lora_gateway_pi2_winput.o

lora_gateway_downlink.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DIS_RCV_GATEWAY -DDOWNLINK -c lora_gateway.cpp -o lora_gateway_downlink.o

lora_gateway_pi2_downlink.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DRASPBERRY2 -DIS_RCV_GATEWAY -DDOWNLINK -c lora_gateway.cpp -o lora_gateway_pi2_downlink.o

lora_gateway_bb_downlink.o: lora_gateway.cpp radio.makefile gateway_conf.json
	g++ $(CFLAGS) -DRASPBERRY -DBEAGLEBONE -DIS_RCV_GATEWAY -DDOWNLINK -c lora_gateway.cpp -o lora_gateway_bb_downlink.o

arduPi.o: arduPi.cpp arduPi.h
	g++ -c arduPi.cpp -o arduPi.o	

arduPi_pi2.o: arduPi_pi2.cpp arduPi_pi2.h
	g++ -c arduPi_pi2.cpp -o arduPi_pi2.o	

arduPi_bb.o: arduPi_bb.cpp arduPi_bb.h
	g++ -l SPI.o -l GPIO.o -c arduPi_bb.cpp -o arduPi_bb.o

SX1272.o: SX1272.cpp SX1272.h
	g++ -c SX1272.cpp -o SX1272.o

SX1272_pi2.o: SX1272.cpp SX1272.h
	g++ -DRASPBERRY2 -c SX1272.cpp -o SX1272_pi2.o

SX1272_pi2_module0.o: SX1272.cpp SX1272.h
	g++ -DRASPBERRY2 $(module0) -c SX1272.cpp -o SX1272_pi2_module0.o

SX1272_pi2_module1.o: SX1272.cpp SX1272.h
	g++ -DRASPBERRY2 $(module1) -c SX1272.cpp -o SX1272_pi2_module1.o

SX1272_pi2_module2.o: SX1272.cpp SX1272.h
	g++ -DRASPBERRY2 $(module2) -c SX1272.cpp -o SX1272_pi2_module2.o

SX1272_bb.o: SX1272.cpp SX1272.h
	g++ -DBEAGLEBONE -c SX1272.cpp -o SX1272_pi2.o

SX1272_bb_module0.o: SX1272.cpp SX1272.h
	g++ -DBEAGLEBONE $(module0) -c SX1272.cpp -o SX1272_bb_module0.o

SX1272_bb_module1.o: SX1272.cpp SX1272.h
	g++ -DBEAGLEBONE $(module1) -c SX1272.cpp -o SX1272_bb_module1.o

SX1272_bb_module2.o: SX1272.cpp SX1272.h
	g++ -DBEAGLEBONE $(module2) -c SX1272.cpp -o SX1272_bb_module2.o

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

#for testing as a very simple end-device
lora_device: lora_gateway_dev.o arduPi.o SX1272.o
	g++ -lrt -lpthread lora_gateway_dev.o arduPi.o SX1272.o -o lora_device	

lora_device_pi2: lora_gateway_dev_pi2.o arduPi_pi2.o SX1272_pi2.o
	g++ -lrt -lpthread lora_gateway_dev.o arduPi_pi2.o SX1272_pi2.o -o lora_device_pi2

lora_gateway_dev.o: lora_gateway.cpp
	g++ $(CFLAGS) -DRASPBERRY -DIS_SEND_GATEWAY -DWINPUT -c lora_gateway.cpp -o lora_gateway_dev.o

lora_gateway_dev_pi2.o: lora_gateway.cpp
	g++ $(CFLAGS) -DRASPBERRY -DRASPBERRY2 -DIS_SEND_GATEWAY -DWINPUT -c lora_gateway.cpp -o lora_gateway_dev_pi2.o
#end
		
lib: arduPi.o arduPi_pi2.o SX1272.o SX1272_pi2.o SX1272_wnetkey.o SX1272_pi2_wnetkey.o lora_gateway.o lora_las_gateway.o lora_gateway_pi2.o lora_las_gateway_pi2.o

clean:
	rm *.o lora_*gateway
