#!/bin/bash

scripts/config_board.sh

bash scripts/bb_triple_stop.sh &> /dev/null

./lora_gateway_bb_module0 | python post_processing_gw.py &> log/module0_post_processing &
#./lora_gateway_bb_module1 --raw | python post_processing_gw.py &> log/module1_post_processing &
./downlinker &> log/downlinker &
./lora_gateway_bb_module2 | python post_processing_gw.py &> log/module2_post_processing &

echo 'modules started, outputs are in logs folder'
echo 'killing process would also kill all pythons'

echo 'process will be killed on Return key press.'
read

#setsid -c sleep infinity

#kill $(jobs -p)

bash scripts/bb_triple_stop.sh &> /dev/null
