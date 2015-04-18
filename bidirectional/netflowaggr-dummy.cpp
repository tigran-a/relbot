/*
 * (c) Copyright 2015 Tigran Avanesov, SnT, University of Luxembourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/* 
 * basically does almost nothing: 
 * convert timestamp from the format output by the nfdump into epoch time
 * Does not perform the aggregation of bidirectional flows
 *
 * It seems that the aggregation is not needed for the periodic time detection
 *
 */
#include <algorithm>
#include <iostream>
#include <string>

#include <time.h>

using namespace std;




size_t datetime_string_to_milliseconds(const string &sdate, const string &stime){
	//string sdate="2011-12-05";
        //string stime= "00:04:16.223";

	//dummy way, non-locale-proof!  but did not find library (TODO)
	size_t pos = sdate.find("-");
	int year = stoi(sdate.substr(0,pos++));
	size_t pos1 = sdate.find("-", pos);
	int month = stoi(sdate.substr(pos,pos1++));
	int day = stoi(sdate.substr(pos1));

	pos = stime.find(":");
	int hour = stoi(stime.substr(0,pos++));
	pos1 = stime.find(":", pos);
	int minutes =  stoi(stime.substr(pos,pos1++));
	int milliseconds = int(1000*stof(stime.substr(pos1++)));
	

	/*
	cout << year << endl; 
	cout << month << endl; 
	cout << day << endl; 
	cout << hour << endl; 
	cout << minutes << endl; 
	cout << milliseconds << endl; 
	*/
	
	struct tm t;
	time_t res;
	t.tm_year = year-1900;
	t.tm_mon = month-1;           // Month, 0 - jan
	t.tm_mday = day;          // Day of the month
	t.tm_hour = hour;     
	t.tm_min = minutes;
	t.tm_sec = milliseconds / 1000;
	t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
	res = timegm(&t);
	// http://pubs.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap04.html#tag_04_14
	return res*1000+ milliseconds % 1000;
}


int main(){

	string sdate, stime, srcip, destip;
	int srcport;
	long  bytez;
	float destport; //   "fmt:%ts %sa %sp %da %dp %byt"  2011-12-05 00:04:16.226    107.242.91.50      0   177.228.219.35   1.11    12672

	// will read output from 
	// nfdump -q  -N  -R nfcapd.201112050002:nfcapd.201112052357  -o "fmt:%ts %sa %sp %da %dp %byt"
	// e.g.
	// 2011-12-05 00:04:16.223   177.233.227.36  63057   107.242.93.192   6969      100
	// 2011-12-05 00:04:47.839   171.55.226.123     80  128.238.221.202  53653       46
	// 2011-12-05 00:04:30.495    107.242.86.82  30284  173.239.176.201  16064     1944
	// start date-time            src ip         srcport	dst ip       dstport  bytes
	while (cin >> sdate >> stime >> srcip >>srcport >> destip >> destport >> bytez){
		cout << datetime_string_to_milliseconds(sdate, stime) << " " << srcip << " " <<  destip << endl;
	}


	return 0;
}

