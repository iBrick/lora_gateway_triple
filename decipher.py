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

        if len(sys.argv) > 3:
            nwkskey = list(bytearray.fromhex(sys.argv[3]))
    #print('data, appskey, nwkskey: {} {} {}'.format(payload, appskey, nwkskey))

    lorawan = LoRaWAN.new(nwkskey, appskey)
    lorawan.read(payload)
    print(''.join('{:02X}'.format(i) for i in lorawan.get_payload()))
