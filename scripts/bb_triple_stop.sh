#!/bin/bash

sudo killall python

sudo killall lora_gateway_bb_module0
sudo killall lora_gateway_bb_module1
sudo killall lora_gateway_bb_module2

sudo killall python

rm /dev/shm/sem.spi_shared_access

