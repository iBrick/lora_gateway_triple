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


HEADER_SIZE = 4
APPKEY_SIZE = 4
PKT_TYPE_DATA = 0x10
PKT_TYPE_ACK = 0x20

PKT_FLAG_ACK_REQ = 0x08
PKT_FLAG_DATA_ENCRYPTED = 0x04
PKT_FLAG_DATA_WAPPKEY = 0x02
PKT_FLAG_DATA_DOWNLINK = 0x01

LORAWAN_HEADER_SIZE = 13


def getSingleChar():
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

pdata = "0,0,0,0,0,0,0,0"
rdata = "0,0,0"
tdata = "N/A"

while True:

    sys.stdout.flush()

    ch = getSingleChar()

    if (ch == '^'):
        now = datetime.datetime.now()
        ch = sys.stdin.read(1)

        if (ch == 'p'):
            pdata = sys.stdin.readline()
            print now.isoformat()
            print "rcv ctrl pkt info (^p): "+pdata,
            arr = map(int, pdata.split(','))
            print "splitted in: ",
            print arr
            dst = arr[0]
            ptype = arr[1]
            ptypestr = "N/A"
            if ((ptype & 0xF0) == PKT_TYPE_DATA):
                ptypestr = "DATA"
                if (ptype & PKT_FLAG_DATA_DOWNLINK) == PKT_FLAG_DATA_DOWNLINK:
                    ptypestr = ptypestr + " DOWNLINK"
                if (ptype & PKT_FLAG_DATA_WAPPKEY) == PKT_FLAG_DATA_WAPPKEY:
                    ptypestr = ptypestr + " WAPPKEY"
                if (ptype & PKT_FLAG_DATA_ENCRYPTED) == PKT_FLAG_DATA_ENCRYPTED:
                    ptypestr = ptypestr + " ENCRYPTED"
                if (ptype & PKT_FLAG_ACK_REQ) == PKT_FLAG_ACK_REQ:
                    ptypestr = ptypestr + " ACK_REQ"
            if ((ptype & 0xF0) == PKT_TYPE_ACK):
                ptypestr = "ACK"
            src = arr[2]
            seq = arr[3]
            datalen = arr[4]
            SNR = arr[5]
            RSSI = arr[6]
            if (_rawFormat == 0):
                info_str = "(dst=%d type=0x%.2X(%s) src=%d seq=%d len=%d SNR=%d RSSI=%d)" % (
                    dst, ptype, ptypestr, src, seq, datalen, SNR, RSSI)
            else:
                info_str = "rawFormat(len=%d SNR=%d RSSI=%d)" % (
                    datalen, SNR, RSSI)
            print info_str
            # TODO: maintain statistics from received messages and periodically add these informations in the gateway.log file

            # here we check for pending downlink message that need to be sent back to the end-device
            #
            for downlink_request in pending_downlink_requests:
                request_json = json.loads(downlink_request)
                if (request_json["dst"] == 0) or (src == request_json["dst"]):
                    print "post downlink: receive from %d with pending request" % src
                    if (request_json["dst"] == 0):
                        print "in broadcast mode"
                    else:
                        print "in unicast mode"
                    print "post downlink: downlink data is \"%s\"" % request_json["data"]
                    print "post downlink: generate "+_gw_downlink_file+" from entry"
                    print downlink_request.replace('\n', '')

                    # generate the MIC corresponding to the clear data and the destination device address
                    # it is possible to have a broadcast address but since the only device that is listening
                    # is the one that has sent a packet, there is little interest in doing so
                    # so currently, we use the sending device's address to compute the MIC
                    # Renha: was MIC=loraWAN_get_MIC(src,request_json["data"])
                    MIC = [0, 0, 0, 0]
                    # add the 4 byte MIC information into the json line
                    request_json['MIC0'] = hex(MIC[0])
                    request_json['MIC1'] = hex(MIC[1])
                    request_json['MIC2'] = hex(MIC[2])
                    request_json['MIC3'] = hex(MIC[3])

                    downlink_json = []
                    downlink_json.append(request_json)

                    f = open(os.path.expanduser(_gw_downlink_file), "a")

                    print "post downlink: write"
                    for downlink_json_line in downlink_json:
                        # print downlink_json_line
                        print json.dumps(downlink_json_line)
                        f.write(json.dumps(downlink_json_line)+'\n')

                    f.close()

                    pending_downlink_requests.remove(downlink_request)

                    # update downlink-post-queued.txt
                    f = open(os.path.expanduser(
                        _post_downlink_queued_file), "w")
                    for downlink_request in pending_downlink_requests:
                        f.write("%s" % downlink_request)
                    # TODO: should we write all pending request for this node
                    # or only the first one that we found?
                    # currently, we do only the first one
                    break

        if (ch == 'r'):
            rdata = sys.stdin.readline()
            print "rcv ctrl radio info (^r): "+rdata,
            arr = map(int, rdata.split(','))
            print "splitted in: ",
            print arr
            bw = arr[0]
            cr = arr[1]
            sf = arr[2]
            info_str = "(BW=%d CR=%d SF=%d)" % (bw, cr, sf)
            print info_str

        if (ch == 't'):
            tdata = sys.stdin.readline()
            print "rcv timestamp (^t): "+tdata

        if (ch == 'l'):
            # TODO: LAS service
            print "not implemented yet"

        if (ch == '$'):

            data = sys.stdin.readline()
            print data,

            # when the low-level gateway program reset the radio module then it is will send "^$Resetting the radio module"
            if 'Resetting' in data:

                if _use_mail_alert:
                    print "post_processing_gw.py sends mail indicating that gateway has reset radio module..."
                    if checkNet():
                        send_alert_mail("Gateway "+_gwid +
                                        " has reset its radio module")
                        print "Sending mail done"

                if _use_sms_alert:
                    print "post_processing_gw.py sends SMS indicating that gateway has reset radio module..."
                    success = libSMS.send_sms(
                        sm, "Gateway "+_gwid+" has reset its radio module", contact_sms)
                    if (success):
                        print "Sending SMS done"

        continue

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
                    cmd_arg = cloud_script+" \""+ldata.replace('\0', '')+"\""+" \""+pdata.replace(
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
