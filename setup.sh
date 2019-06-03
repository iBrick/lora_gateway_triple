#!/bin/bash
if  [[ `whoami` == "root" ]]; then
	echo "Error: don't `sudo` me! I'll ask for privelegies when I'll need those."
else
	(
		echo "Starting setup"
		echo "Fetching latest changes from repository"
		git pull
		echo "Updating cape driver (reboot may be required)"
		sudo bash update_cape.sh
		echo "Building executables"
		make
		echo "Setting up sustemd services (to be run after mqttclient)"
		sudo ln -s /home/pi/downlinker.service /etc/systemd/system/
		sudo ln -s /home/pi/uplinker-plain.service /etc/systemd/system/
		sudo ln -s /home/pi/uplinker-alarm.service /etc/systemd/system/
		echo "TODO: time setup, write time to RTC."
		echo "Setup finished. You may restart now."
	)
fi;
