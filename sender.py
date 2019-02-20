#!/usr/bin/env python3
import paho.mqtt.client as mqtt

def post(data):
	with open('/home/pi/lora_gateway/downlink/downlink-post.txt', 'w') as outfile:
		outfile.write(data)

def on_connect(client, userdata, flags, rc):
	#print('Connected with result code {}'.format(rc))
	client.subscribe("#")

def on_message(client, userdata, msg):
	dest = ''.join([i for i in msg.topic if i in '0123456789'])
	message = str(msg.payload.decode())
	towrite = '{'+'"status":"send_request","dst":{},"data":"{}"'.format(dest, message)+'}\n'
	post(towrite)
	#print("{} {}".format(msg.topic, msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost")

client.loop_forever()

