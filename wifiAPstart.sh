sudo rfkill unblock wifi
systemctl stop dnsmasq
sudo killall -9 wpa_supplicant
create_ap --no-virt wlan0 eth0 testAP testtest &