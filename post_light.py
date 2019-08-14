from clouds_parser import retrieve_enabled_clouds
import sys
import subprocess
import select
import threading
from threading import Timer
import time
from collections import deque
import datetime
try:
    import dateutil.tz
except ImportError:
    print "no timezone support, time will be expressed only in local time"
    dateutil_tz = False
import getopt
import os
import os.path
import json
import re
import string
import base64
import requests


_linebuf = "the line buffer"
_linebuf_idx = 0
_has_linebuf = 0


def getSingleChar():
    global _has_linebuf
    # if we have a valid _linebuf then read from _linebuf
    if _has_linebuf == 1:
        global _linebuf_idx
        global _linebuf
        if _linebuf_idx < len(_linebuf):
            _linebuf_idx = _linebuf_idx + 1
            return _linebuf[_linebuf_idx-1]
        else:
            # no more character from _linebuf, so read from stdin
            _has_linebuf = 0
            return sys.stdin.read(1)
    else:
        return sys.stdin.read(1)


def getAllLine():
    toret = ''

    while True:
        c = getSingleChar()
        if c in ('\r', '\n'):
            break
        toret += c
    return toret


f = open(os.path.expanduser("gateway_conf.json"), "r")
lines = f.readlines()
f.close()
json_array = json.loads(''.join(lines))


# set the gateway_address for having different log filenames
_gwid = json_array["gateway_conf"]["gateway_ID"]

# ------------------------------------------------------------
# raw format?
# ------------------------------------------------------------
try:
    _rawFormat = json_array["gateway_conf"]["raw"]
except KeyError:
    _rawFormat = 0

if _rawFormat:
    print "raw output from low-level gateway. post_processing_gw will handle packet format"


# get a copy of the list of enabled clouds
_enabled_clouds = retrieve_enabled_clouds()

print "post_processing_gw.py got cloud list: "
print _enabled_clouds

# ------------------------------------------------------------
# open clouds.json file to get clouds for encrypted data
# ------------------------------------------------------------

_cloud_for_encrypted_data = retrieve_enabled_clouds("encrypted_clouds")
print "post_processing_gw.py got encrypted cloud list: "
print _cloud_for_encrypted_data

_cloud_for_lorawan_encrypted_data = retrieve_enabled_clouds(
    "lorawan_encrypted_clouds")
print "post_processing_gw.py got LoRaWAN encrypted cloud list: "
print _cloud_for_lorawan_encrypted_data


# ------------------------------------------------------------
# main loop
# ------------------------------------------------------------

while True:

    sys.stdout.flush()

    ch = getSingleChar()

    if (ch == '\\'):

        now = datetime.datetime.now()

        ch = getSingleChar()

        if (ch == '!'):
            ldata = getAllLine()

            print "number of enabled clouds is %d" % len(_enabled_clouds)

            for cloud_index in range(0, len(_enabled_clouds)):

                try:
                    print "--> cloud[%d]" % cloud_index
                    cloud_script = _enabled_clouds[cloud_index]
                    print "uploading with "+cloud_script
                    sys.stdout.flush()
                    cmd_arg = cloud_script+" \""+ldata.replace('\n', '').replace('\0', '')+"\""+" \""+pdata.replace(
                        '\n', '')+"\""+" \""+rdata.replace('\n', '')+"\""+" \""+tdata.replace('\n', '')+"\""+" \""+_gwid.replace('\n', '')+"\""
                except UnicodeDecodeError, ude:
                    print ude
                else:
                    print cmd_arg
                    sys.stdout.flush()
                    try:
                        os.system(cmd_arg)
                    except:
                        print "Error when uploading data to the cloud"

            print "--> cloud end"

        else:
            print "unrecognized data logging prefix: discard data"
            getAllLine()
            continue

    sys.stdout.write(ch)
