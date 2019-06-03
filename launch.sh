#!/bin/bash
# run with sudo or from root
#sudo ./scripts/bb_triple_restart.sh

killall uplinker ; killall downlinker
(
    cd /home/pi/lora_gateway
    ./uplinker /dev/spidev0.0 4 1 1 14 | python post_processing_gw.py &> log/uplinker_plain &
    ./downlinker /dev/spidev0.1 6 1 1 14 > log/downlinker &
    ./uplinker /dev/spidev0.2 8 1 1 14 | python post_processing_gw.py &> log/uplinker_alarm &

    echo 'modules started, outputs are in logs folder'
    echo 'killing process would also kill all pythons'
    echo 'process will be killed on Return key press.'
    read
)
killall uplinker ; killall downlinker

#setsid -c sleep infinity

#kill $(jobs -p)
