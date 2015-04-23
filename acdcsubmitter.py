#!/usr/bin/python3 
# also works with python 2

"""
    Script for submitting reports to CCH about hosts detected by relbot.
    Reads input from stdin in a format that produced by relbot (see run.sh)

    (c) Copyright 2015 Tigran Avanesov, SnT, University of Luxembourg

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

"""

import json
import sys

import datetime

import logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
log = logging.getLogger(__file__)

import itertools

from PyACDC.pyacdc_apiv2.apiv2 import CCHv2


""" CONFIG """

# The mode of the source IP. This can be plain for unaltered IPs, anon for anonymised IPs, or pseudo for pseudonymised IPs.
IP_MODE="plain"

# filter out all suspecious hosts for which the number of periodic connections are less than MIN_IDX.
# for example, MIN_IDX = 2 will skip reporting this host
# =============================
# 96.249.220.42
# 600000 -> 1
# 900000 -> 1
# 1800900 -> 0
# 2400000 -> 0
#
# But will submit a report about this host:
# =============================
# 96.249.85.26
# 600000 -> 0
# 900000 -> 2
# 1800900 -> 0
# 2400000 -> 0
#
MIN_IDX = 1

# default confidence level (from 0.0 to 1.0) for the reports
CONFIDENCE = 0.5

# CCH key
KEY = "heregoesyourhexadecimalcchkey" 

# CCH host
CCH_HOST =  "webservice.db.acdc-project.eu"
CCH_PORT = 3000

# CCH Port

""" END CONFIG """

def build_report(ip, confidence= CONFIDENCE , timestamp = None, version =1, ipmode = IP_MODE, reporttype="RelBot detected a suspecious host" ):
    """timestamp should be a string in ISO format representing UTC time (Z) without milliseconds;
    if None takes current time"""

    # building a json record to report a bot:
    report = {}
    report['report_category'] = 'eu.acdc.bot'
    report["report_type"] = reporttype
    timestamp = timestamp or datetime.datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")
    report["timestamp"] = timestamp
    report["source_key"] = "ip"
    report["source_value"] = ip
    report["confidence_level"] = confidence 
    report["version"] = version 
    report["report_subcategory"] =  "other"
    if ":" in ip:
        report["ip_version"] = 6
        report["src_ip_v6"] = ip 
    else: 
        report["ip_version"] = 4
        report["src_ip_v4"] = ip 
    report["src_mode"] = ipmode

    return report

def submit_bot():
    pass


def read_input():
    """ reads from std input the output of the relbot tool
    parses it, and returns next IP if it satisfies the MIN_IDX filter
    """
    res = None
    buf = [] #list of lines between two =====
    for line in itertools.chain(sys.stdin, ["============================="]):
        if line.startswith("==="):  
            res = None
            for l in buf[1:]:
                if int(l.split("->")[1]) >= MIN_IDX: # getting the right part of 100000 -> 3   and ocmparing with minimal index
                    res = buf[0]  # since got one period for which teh index is acceptable, save the result 
                    break
            if res is not None: 
                yield (res)
            res = None
            buf=[]
        else: 
            buf.append(line)


if __name__ == '__main__' : 


    cch = CCHv2(apikey = KEY, host = CCH_HOST , port= CCH_PORT)
    cnt = 0
    for ip in read_input(): 
        cnt+=1
        log.info("%d. Reporting ip %s", cnt, ip)
        report = build_report(ip, reporttype="RelBot detected suspicious host activities")
        r =  cch.submit_report(report)
        log.info("Server replied: \n %s", json.dumps(r, indent=2))



    print("Requesting again all reports for last 15 min; should see the submitted one")
    r =  cch.get_reports()
    print(json.dumps(r, indent= 2))
