#!/bin/bash
# run with sudo or from root
#sudo ./scripts/bb_triple_restart.sh

killall uplinker 
killall downlinker
(
    cd /home/pi/lora_gateway
    ./uplinker /dev/spidev0.2 4 1 1 14 | python post_processing_gw.py &> log/module0_post_processing &
    ./downlinker > log/downlinker &
    ./uplinker /dev/spidev0.2 4 1 1 14 | python post_processing_gw.py &> log/module0_post_processing &

    echo 'modules started, outputs are in logs folder'
    echo 'killing process would also kill all pythons'
    echo 'process will be killed on Return key press.'
    read
)

#setsid -c sleep infinity

#kill $(jobs -p)
