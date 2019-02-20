#!/bin/bash

bash scripts/config_pins.sh

bash scripts/triple_stop.sh &> /dev/null

./lora_gateway_bb_module0 --raw | python post_processing_gw.py &> log/module0_post_processing &
./lora_gateway_bb_module1 --raw | python post_processing_gw.py &> log/module1_post_processing &
./lora_gateway_bb_module2 --raw | python post_processing_gw.py &> log/module2_post_processing &

echo 'modules started, outputs are in logs folder'
echo 'killing process would also kill all pythons'

setsid -c sleep infinity

kill $(jobs -p)
killall python
