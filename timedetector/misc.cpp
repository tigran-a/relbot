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
#include "misc.hpp"
#include <iostream>
#include <string>

using namespace std;

void print_separator(){
	cout << "=============================" << endl;
}
void print_separator(string title){
	print_separator();
	cout << title << endl;
}

double count_nonzero_elems(const vector<double> &v){
	int res = 0;
	for (auto x:v) {
		if (x!=0) res ++;
	}
	return res;
}


// two functions from https://stackoverflow.com/questions/7616511/

// mean of vector's values
double mean(const vector<double> & v, bool nonzero){
	double sum = std::accumulate(v.begin(), v.end(), 0.0);
	double nitems = 0;
	if (nonzero) {
		nitems= count_nonzero_elems(v);
	} else {
		nitems = v.size();
	}
	double mean = double(sum) / nitems;
	return mean;
}

// standard deviation on vector's values
double dispersion(const vector<double>& v, bool nonzero){
	double sq_sum = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
	double meanv = mean(v, nonzero);
	double nitems = 0;
	if (nonzero) {
		nitems= count_nonzero_elems(v);
	} else {
		nitems = v.size();
	}
	double stdev = sqrt(sq_sum / nitems - meanv * meanv);
	return stdev;
}
