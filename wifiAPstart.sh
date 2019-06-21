#!/bin/bash

/usr/sbin/rfkill unblock wifi
systemctl stop dnsmasq
/bin/ip link set wlan0 up
/usr/bin/killall -9 wpa_supplicant
/usr/bin/create_ap --no-virt wlan0 eth0 iglaWifi iglaigla