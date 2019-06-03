#!/bin/bash
if  [[ `whoami` == "root" ]]; then
	echo "Error: don't 'sudo' me! I'll ask for privelegies when I'll need those."
else
	(
		echo "-Starting setup"
		cd /home/pi/lora_gateway
		echo "-Creating neccessary folders"
		mkdir /home/pi/lora_gateway/downlink
		echo "-Fetching latest changes from repository"
		git pull
		echo "-Updating cape driver (reboot may be required)"
		sudo bash update_cape.sh
		echo "-Setting up uEnv.txt"
		if cmp -s "/boot/uEnv.txt" "/home/pi/lora_gateway/uEnv.txt"; then
			echo "--No need to update"
		else
			today=`date +%Y-%m-%d.%H:%M:%S`
			echo "--Updating saving backup to /boot/uEnv.txt.bk.$today"
			sudo mv /boot/uEnv.txt "/boot/uEnv.txt.bk.$today"
			sudo cp /home/pi/lora_gateway/uEnv.txt /boot/uEnv.txt
		fi;
		echo "-Building executables"
		make
		echo "-Setting up sustemd services (to be run after mqttclient)"
		sudo ln -s /home/pi/lora_gateway/downlinker.service /etc/systemd/system/
		sudo ln -s /home/pi/lora_gateway/uplinker_plain.service /etc/systemd/system/
		sudo ln -s /home/pi/lora_gateway/uplinker_alarm.service /etc/systemd/system/
		echo "-TODO: time setup, write time to RTC."
		echo "-Setup finished. You may restart now."
	)
fi;
