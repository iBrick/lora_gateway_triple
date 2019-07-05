#!/usr/bin/env python3
import sys
import LoRaWAN

appskey = [0]*16
nwkskey = appskey

if len(sys.argv) < 2 or 'h' in sys.argv[1]:
    print('Usage (all data in hex): {} payload [appskey] [nwskey]'.format(
        sys.argv[0]))
else:
    payload = list(bytearray.fromhex(sys.argv[1]))

    if len(sys.argv) > 2:
        appskey = list(bytearray.fromhex(sys.argv[2]))

<<<<<<< HEAD
    if len(sys.argv) > 3:
        nwskey = list(bytearray.fromhex(sys.argv[3]))

    lorawan = LoRaWAN.new(nwskey, appskey)
    lorawan.read(payload)
    print(''.join('{:02X}'.format(i) for i in lorawan.get_payload()))
=======
	if len(sys.argv) > 3:
		nwkskey = list( bytearray.fromhex( sys.argv[ 3 ] ) )

	lorawan = LoRaWAN.new(nwkskey, appskey)
	lorawan.read(payload)
	print( ''.join('{:02X}'.format(i) for i in lorawan.get_payload()))
>>>>>>> 7cb201b48ce87cd4c01b34044b2741c9a35760ab
