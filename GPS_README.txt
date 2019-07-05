Работа с GPS модулем:

1. Пример данных которые выводит cgps:

{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:55:51.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10
.677,"epy":24.216,"epv":15.870,"track":181.3400,"speed":0.000,"climb":0.000,"eps":48.43,"epc":31.74}
{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:55:52.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10
.677,"epy":24.216,"epv":17.710,"track":181.3400,"speed":0.000,"climb":0.000,"eps":48.43,"epc":33.58}
{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:55:53.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10
.677,"epy":24.216,"epv":17.710,"track":181.3400,"speed":0.000,"climb":0.000,"eps":48.43,"epc":35.42}
{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:55:54.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10
.677,"epy":24.216,"epv":17.710,"track":181.3400,"speed":0.000,"climb":0.000,"eps":48.43,"epc":35.42}
{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:55:55.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10
.677,"epy":24.216,"epv":30.590,"track":181.3400,"speed":0.000,"climb":0.000,"eps":48.43,"epc":48.30}
{"class":"SKY","device":"/dev/ttyS4","xdop":0.71,"ydop":1.61,"vdop":0.82,"tdop":1.80,"hdop":0.77,"gdop":3.36,"pdop":1.12,"satellites":[{"PRN":29,"el":83,"az":88,"ss":27,"used":false},{"PRN":25,"el"
:40,"az":166,"ss":13,"used":false},{"PRN":5,"el":39,"az":83,"ss":13,"used":false},{"PRN":26,"el":35,"az":303,"ss":35,"used":false},{"PRN":21,"el":32,"az":223,"ss":20,"used":false},{"PRN":31,"el":26
,"az":253,"ss":24,"used":false},{"PRN":2,"el":22,"az":59,"ss":23,"used":false},{"PRN":131,"el":21,"az":147,"ss":0,"used":false},{"PRN":16,"el":9,"az":314,"ss":30,"used":false},{"PRN":9,"el":7,"az":
7,"ss":34,"used":false},{"PRN":12,"el":7,"az":147,"ss":0,"used":false},{"PRN":84,"el":66,"az":45,"ss":0,"used":false},{"PRN":75,"el":53,"az":282,"ss":33,"used":true},{"PRN":85,"el":48,"az":265,"ss"
:20,"used":true},{"PRN":74,"el":32,"az":202,"ss":21,"used":false},{"PRN":68,"el":30,"az":57,"ss":37,"used":true},{"PRN":76,"el":21,"az":334,"ss":23,"used":true},{"PRN":69,"el":18,"az":109,"ss":0,"u
sed":false},{"PRN":67,"el":12,"az":4,"ss":36,"used":true},{"PRN":83,"el":10,"az":67,"ss":18,"used":true}]}
{"class":"TPV","device":"/dev/ttyS4","mode":3,"time":"2019-07-05T09:56:31.000Z","ept":0.005,"lat":55.771425117,"lon":37.561015233,"alt":189.516,"epx":10.677,"epy":24.216,"epv":18.860,"track":181.34
00,"speed":0.000,"climb":0.000,"eps":48.43,"epc":37.72}

2. Открыть порты (для дебага на локальной машине)
sudo iptables -A INPUT -p tcp --dport 2947 -j ACCEPT
sudo iptables -A OUTPUT -p tcp --dport 2947 -j ACCEPT

3. sudo apt-get install gpsd gpsd-clients
sudo gpsd /dev/ttyS4 -F /var/run/gpsd.sock   (данные появляются не сразу, надо немного подождать)
sudo nano /etc/default/gpsd
	START_DAEMON="true"
	USBAUTO="true"
	DEVICES="/dev/ttyS4"
	GPSD_OPTIONS="-b -n"
sudo systemctl enable gpsd.socket


Хорошая статья: https://medium.com/@DefCon_007/using-a-gps-module-neo-7m-with-raspberry-pi-3-45100bc0bb41
