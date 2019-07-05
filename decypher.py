#!/usr/bin/env python3
import sys
import LoRaWAN

appskey = [0]*16
nwskey = appskey

if len( sys.argv ) < 2 or 'h' in sys.argv[ 1 ]:
	print( 'Usage (all data in hex): {} [payload] [appskey] [nwskey]'.format( sys.argv[ 0 ] ) )
else:
	payload = list( bytearray.fromhex( sys.argv[ 1 ] )

	if len(sys.argv) > 2:
		appskey = list( bytearray.fromhex( sys.argv[ 2 ] )

	if len(sys.argv) > 3:
		nwskey = list( bytearray.fromhex( sys.argv[ 3 ] )

	lorawan = LoRaWAN.new(nwskey, appskey)
	lorawan.read(payload)
	print( ''.join('{:02X}'.format(i) for i in lorawan.get_payload()))
