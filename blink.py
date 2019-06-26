#!/usr/bin/env python3

import time
import sys
from ind import indicators
import os

os.chdir('/home/pi/igla-gui/')


if len(sys.argv) < 2:
    print('Usage: {} {} {}'.format(sys.argv[0], 'led_name', '[blink_time]'))

else:
    if len(sys.argv) < 3:
        sys.argv.append('2')
    indicators[sys.argv[1]].state = True

    time.sleep(int(sys.argv[2]) if len(sys.argv) > 2 else 2)

    indicators[sys.argv[1]].state = False
