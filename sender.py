#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import time

index = 0
fname = '/home/pi/lora_gateway/downlink/downlink-post.txt'
fname = '/home/pi/lora_gateway/downlink/downlink.txt'

def post(data):
	with open(fname, 'w') as outfile:
		outfile.write(data)
	print (data)

def on_connect(client, userdata, flags, rc):
	#print('Connected with result code {}'.format(rc))
	client.subscribe("#")

def on_message(client, userdata, msg):
	global index
	dest = ''.join([i for i in msg.topic if i in '0123456789'])
	message = str(msg.payload.decode()) + str(index)
	towrite = '{'+'"status":"send_request","dst":{},"data":"{}"'.format(dest, message)+'}\n'
	post(towrite)
	index += 1

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost")

client.loop_forever()

