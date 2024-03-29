####################################################
# server: CAUTION must exist
MQTT_server = "localhost"

# project name
project_name = "igla"

# your organization: CHANGE HERE
# choose one of the following: "DEF", "UPPA", "EGM", "IT21", "CREATENET", "CTIC", "UI", "ISPACE", "UGB", "WOELAB", "FARMERLINE", "C4A", "PUBD"
organization_name = "IGLA"
# organization_name="ORG"

# sensor name: CHANGE HERE but maybe better to leave it as Sensor
# the final name will contain the sensor address
sensor_name = "Sensor"

# CloudMQTT will built the sensor name as waziup/UPPA/Sensor2 to serve as an MQTT topic

# Note how we can indicate a device source addr that are allowed to use the script
# Use decimal between 2-255 and use 4-byte hex format for LoRaWAN devAddr
# leave empty to allow all devices
#source_list=["3", "255", "01020304"]
source_list = []

appskey = '10101010101010101010101010101010'
nwkskey = '10101010101010101010101010101010'
#appskey = '00000000000000000000000000000000'
#nwkskey = '00000000000000000000000000000000'

####################################################
