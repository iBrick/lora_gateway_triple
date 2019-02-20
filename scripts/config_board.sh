#!/bin/bash

IP=()
OP0=()
OP1=()

config-pin 911 uart
config-pin 912 gpio ; OP1+=(60)
config-pin 913 uart
config-pin 914 gpio ; IP+=(40)
config-pin 918 spi
config-pin 919 i2c
config-pin 920 i2c
config-pin 921 spi
config-pin 922 spi_sclk
# module 0: NSS, RST, DIO0, DIO1
config-pin 926 gpio ; OP1+=(14)
config-pin 820 gpio ; IP+=(63)
config-pin 815 gpio ; IP+=(47)
config-pin 816 gpio ; IP+=(46)
# LEDs: LED1, LED2, LED3, LED4
config-pin 807 gpio ; OP0+=(66)
config-pin 808 gpio ; OP0+=(67)
config-pin 809 gpio ; OP0+=(69)
config-pin 810 gpio ; OP0+=(68)
# module 1: NSS, RST, DIO0, DIO1
config-pin 823 gpio ; OP1+=(49)
config-pin 822 gpio ; IP+=(37)
config-pin 813 gpio ; IP+=(23)
config-pin 814 gpio ; IP+=(26)
# BTP, BUZZER, iBUTTON
config-pin 817 gpio ; IP+=(27)
config-pin 818 gpio ; OP0+=(65)
config-pin 819 gpio ; IP+=(22) # ?
# module 2: NSS, RST, DIO0, DIO1
config-pin 916 gpio ; OP1+=(51)
config-pin 821 gpio ; IP+=(52)
config-pin 811 gpio ; IP+=(45)
config-pin 812 gpio ; IP+=(44)

# exporting unexported GPIO pins
for OPN in ${OP1[@]} ${OP0[@]} ${IP[@]}
do
	if [ ! -d "/sys/class/gpio/gpio${OPN}" ]; then
		echo "GPIO${OPN} was not exported, exporting..."
			echo ${OPN} > /sys/class/gpio/export
		echo "done"
	fi
done

# setting in pins direction to input
for OPN in ${OP1[@]} ${OP0[@]}
do
        echo in > /sys/class/gpio/gpio${OPN}/direction
done

# setting out pins direction to output
for OPN in ${OP1[@]} ${OP0[@]}
do
        echo out > /sys/class/gpio/gpio${OPN}/direction
done

# setting 1 pins value to 1
for OPN in ${OP1[@]}
do
        echo 1 > /sys/class/gpio/gpio${OPN}/value
done

if [ ! -e "/dev/rtc1" ]; then
	echo "no external RTC module registered, registering..."
	echo ds1307 0x68 >/sys/bus/i2c/devices/i2c-2/new_device
	sleep 1
	echo "done"
fi

#timedatectl

if [[ `timedatectl status | grep sync | grep yes | wc -l` == 0 ]] ; then
	echo "time not in sync, reading from RTC..."
	hwclock --set --date="`hwclock -f /dev/rtc1`" -f /dev/rtc0
	#timedatectl set-ntp false
	hwclock -s
	#timedatectl set-ntp true
	echo "done"
fi
if [[ `timedatectl status | grep sync | grep yes | wc -l` == 1 ]] ; then
	echo "time is in sync, writing to RTC..."
	hwclock -w -f /dev/rtc0
	hwclock -w -f /dev/rtc1
	echo "done"
fi

#timedatectl
#sudo hwclock -r -f /dev/rtc0 ; sudo hwclock -r -f /dev/rtc1
