// misc.hpp
#ifndef misc_hpp
#define misc_hpp
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <cmath>
#include <iostream>
#include <boost/property_tree/ptree.hpp>

using namespace std;

void print_separator();
void print_separator(string title);

template<typename K, typename V>
void print_map(const map<K, V> &m){
	for(auto iter=m.begin(); iter != m.end(); ++iter){
		cout << iter->first << " -> " << iter->second << endl;
	}
};

// mean of vector's values
double mean(const vector<double> & v, bool nonzero = false);
// standard deviation on vector's values
double dispersion(const vector<double>& v, bool nonzero = false);


// from https://stackoverflow.com/questions/23481262
using boost::property_tree::ptree;
template <typename T>
std::vector<T> as_vector(ptree const& pt, ptree::key_type const& key)
{
	std::vector<T> r;
	for (auto& item : pt.get_child(key))
		r.push_back(item.second.get_value<T>());
	return r;
}
#endif

