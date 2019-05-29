#!/bin/bash
cd /opt/source/bb.org-overlays/
cp /home/pi/lora_gateway/BB-IGLA-00A0.dts /opt/source/bb.org-overlays/src/arm/
(cd /opt/source/bb.org-overlays/ && make )
cp /opt/source/bb.org-overlays/src/arm/BB-IGLA-00A0.dtbo /lib/firmware

