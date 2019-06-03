#!/bin/bash
if  [[ `whoami` == "root" ]]; then
	echo "Error: don't 'sudo' me! I'll ask for privelegies when I'll need those."
else
	(
		echo "-Starting setup"
		echo "-Fetching latest changes from repository"
		git pull
		echo "-Updating cape driver (reboot may be required)"
		sudo bash update_cape.sh
		echo "-Setting up uEnv.txt (backing up previous to uEnv.txt.bk)"
		if [[ cmp --silent /boot/uEnv.txt /home/pi/lora_gateway/uEnv.txt ]]; then
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
		sudo ln -s /home/pi/lora_gateway/uplinker-plain.service /etc/systemd/system/
		sudo ln -s /home/pi/lora_gateway/uplinker-alarm.service /etc/systemd/system/
		echo "-TODO: time setup, write time to RTC."
		echo "-Setup finished. You may restart now."
	)
fi;
