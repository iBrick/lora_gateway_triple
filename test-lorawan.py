#!/usr/bin/env python3

import LoRaWAN
nwskey = [0xC3, 0x24, 0x64, 0x98, 0xDE, 0x56, 0x5D, 0x8C, 0x55, 0x88, 0x7C, 0x05, 0x86, 0xF9, 0x82, 0x26]
appskey = [0x15, 0xF6, 0xF4, 0xD4, 0x2A, 0x95, 0xB0, 0x97, 0x53, 0x27, 0xB7, 0xC1, 0x45, 0x6E, 0xC5, 0x45]
appskey = list(bytearray.fromhex('2B7E151628AED2A6ABF7158809CF4F3C'))
nwskey=appskey

lorawan = LoRaWAN.new(nwskey, appskey)
PKT = '40 04 05 01 02 00 00 00 01 E4 85 FD 1F 77 91 B9 D9 34 FF 1F F9 A8 BC 3D 7E'
payload = list(bytearray.fromhex(PKT))
lorawan.read(payload)
print(lorawan.get_payload())
