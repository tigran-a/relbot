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

//
// Read bidirectional flows from stdin and analyze flow start times per each source node.
// prototype
//
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "misc.hpp"
#include "nodegraph.hpp"
#include <thread>

namespace po = boost::program_options;

using namespace boost;
using namespace std;

string DEFAULT_CONF_FILE="./timedetector.conf";

// These constants are overwritten by config file
double DISP_COEF = -3.0;
double FREE_COEF = 0;
double MAX_COEF = 1.0;
double MEAN_COEF = 0;

double MIN_VAL = 4; // minimal value of a peak to start to dig deeper

int UNIT_MS = 1000; // will calculate for every second the "spector"
int RADIUS  = 3;  // num of elems (discret buckets) affected around


int NTHREADS  = 1;  // num of threads for parallel the caluclation of suspiciousness

double E = 2.7182818284590452353602874713526; // e
//vector<ts_t> PERIODS  = {10*60*1000, 15*60*1000, 30*60*1000, 40*60*1000}; 

void read_bidir_flows(NodeGraph& ng){


	unsigned long int stime;
	string srcip, destip;
	while (cin >> stime >> srcip >> destip ) {
		NodeGraph::Vertex v0 = ng.add_node(srcip);
		NodeGraph::Vertex v1 = ng.add_node(destip);
		ng.add_link(v0,v1,stime);

	}

	return;
}

// for a given list of start-flow stimestamps will calculate 
// the weight (for now 0 or 1) w.r.t. repetitions w.r.t. each period
// WARNING: times will be changed (sorted)
map<ts_t, float> find_submarine(vector<ts_t>& times, const vector<ts_t>& periods){
	map<ts_t, float> weights; 
	for (auto const &ts : periods){
		weights[ts] = 0;
	};
	if (times.size() < MIN_VAL){ // dataset is too small...
		return weights;
	}

	sort(times.begin(), times.end()); // needed: get min element  and  avoiding close timestamps

	ts_t startts = times[0];

	for (auto const & period: periods){
		int max_idx = period / UNIT_MS;
		// array of discretized time line (max = period); store n. of points in different levels
		// init vals are 0
		vector<double> submarine (max_idx + RADIUS + 1, 0);

		// detecting submarines...  recipe URSS

		int prev_idx = -1;
		int prev_level = -1;
		// build a spector (submarine array)
		for (auto const & _ts: times){
			ts_t ts = _ts - startts;

			int level = ts / period;
			int wrapped_ms = ts - level*period;
			int idx = wrapped_ms/UNIT_MS;

			if ( (prev_level!=level) || (prev_idx!=idx )) { //avoiding duplicates on one level: 10 close points on one level are quiv. to one
				prev_idx = idx;
				prev_level = level;

				for (int x = -RADIUS; x<= RADIUS; ++x){
					int v = wrapped_ms/UNIT_MS -x;
					if (v<0) continue;
					if (RADIUS == 0){
						submarine[v]+=1;
					} else {
					    submarine[v]+= (1-pow(float(abs(x))/RADIUS, 1./E));    //  1-x**1/2 for x in -1, 1   
					}
				}
			}
		}; // now submarine is built

		
		// these 3 can be optimized
		double meanv =  mean(submarine, true);
		double dispersionv  =  dispersion(submarine, true);
		double maxv = * max_element(submarine.begin(), submarine.end());

		// look if the submarine is there (one spike/peak)
		bool seen = false;
		bool takeit = false;
		// cerr << "Current max el: " << maxv <<endl;
		if (maxv >= MIN_VAL) { // at least MIN_VAL (e.g. 4) flows are detected ( multiple of period), in one discrete bucket
			/* cerr << " start digging" << endl;
			for (auto y: submarine){ // look for spikes  
				cout << y << " ";
			};
			cout << endl;
			cout << "Mean, disp, max: " << meanv << " " << dispersionv << " " << maxv << endl;
			*/
			for (auto y: submarine){ // look for spikes  
				if (y >= FREE_COEF + MAX_COEF*maxv + DISP_COEF*dispersionv + MEAN_COEF*meanv){ // which are not below some linear combination fo max, mean and dispersion
					if ((seen) || (y != maxv)) { // found 2nd spike => bad, don't want this set
						takeit = false;
						break; 
					} else {
						seen = true; // found 1st spike; 
						takeit = true; // if no other spike found => the set is suspicious
					}

				}
			}
			if (takeit){
				weights[period]+=1; // add one if suspecious
				/* TODO:
				 * We aslo might need to note the other end of the communication
				 * as if we detect a _link_ between two infected nodes, 
				 * both ends should be infected; currently we are taking note only of one infected node,
				 * the one who initiated the communication
				 */
			}
		}
	}


	return weights;

}
void out_time_deltas(NodeGraph& ng){
  // cout << "Num of vertices: " << ng.get_num_vertices() << endl;


  auto for_edge = [&](NodeGraph::Edge e){
      auto tlist = ng.get_edge_times(e);

	  if (tlist.size()<2){
	  } else {
		  cout << "\t" << ng.get_node_name(ng.getTarget(e))<< ")" << endl;
		  cout << "\t\t";
                  unsigned long int prev = tlist[0];
                  for (auto ts : tlist){
                          //cout << (signed long int)(ts - prev) << " ";
                          cout << ts << " ";
                          prev = ts;
                  }
		  cout << endl;
	  }

  };
  auto for_vert = [&](NodeGraph::Vertex v){ 
	  cout << ng.get_node_name(v) << ")" <<endl;;
	  ng.process_out_edges(v, for_edge);
          cout << endl;
  };
  ng.process_nodes(for_vert);

}

/*
 * Go through the whole graph and collect "suspections" nodes
 *
 */
map<string, map<ts_t, float>>  look_for_suspicious(NodeGraph& ng, const vector<ts_t>& periods){

  
  // map ip addr (str) to the level of its suspiciousness for each period; 
  // not added if 0 of suspeciousness for all periods
  map<string, map<ts_t, float>> resume; 


  auto for_vert = [&](NodeGraph::Vertex v){ 
	  map<ts_t, float> marks;
	  for (auto const &ts : periods){
	     marks[ts] = 0;
	  };
	  
	  // TODO:
	  auto for_edge = [&](NodeGraph::Edge e){
	      auto tlist = ng.get_edge_times(e); // list of timestamps for given dest. node e
	      // cout << "\t" << ng.get_node_name(ng.getTarget(e))<< ")" << endl;

	      // calulate marks for this pair
	      auto marks_e = find_submarine(tlist, periods);
	      // adding marks to the global counter for the given source v
              for (auto const &ts : periods){
	         marks[ts] += marks_e[ts];
	      };
	      return;
	  };

	  ng.process_out_edges(v, for_edge);

	  for (auto const &ts : periods){
	     if (marks[ts] != 0) { // if non-zero mark
		     resume[ng.get_node_name(v)] = marks; // add to the final list
		     break;
	     }
	  };
	  // DEBUG:
	  // print_separator(ng.get_node_name(v));
          // print_map(resume[ng.get_node_name(v)]); 
  };

  // call for_vert for every node in the graph
  ng.process_nodes(for_vert);

  return resume;
}

int main(int argc, char* argv[])
{
	/*
	 * reading command line params
	 */
	string config_file = DEFAULT_CONF_FILE;
	try {

		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "short help message")
			("config", po::value<string>(&config_file)->default_value(DEFAULT_CONF_FILE), "configuration file")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 1;
		}

		/*
		if (vm.count("config")) {
			//config_file = vm["config"].as<string>();
			cout << "Config file was set to "
				<< config_file  << ".\n";
		} else {
		}
		*/
		cout << "Config file was set to " << config_file << "\n";
	}
	catch(std::exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch(...) {
		cerr << "Exception of unknown type!\n";
	}
	
	/*
	 * open and read config file
	 */

	boost::property_tree::ptree config;
	try {
		read_json(config_file, config);
	}
	catch (boost::property_tree::json_parser_error e) {
		cerr << "Error reading config file: " << e.what() << std::endl;
		exit(1);
	}
	vector<ts_t> PERIODS = as_vector<ts_t>(config, "times");
	DISP_COEF = config.get<double>("DISP_COEF", DISP_COEF);
	MAX_COEF = config.get<double>("MAX_COEF", MAX_COEF);
	FREE_COEF = config.get<double>("FREE_COEF", FREE_COEF);
	MEAN_COEF = config.get<double>("MEAN_COEF", MEAN_COEF);

	MIN_VAL = config.get<double>("MIN_VAL", MIN_VAL);

	//	cerr << DISP_COEF << " " << MAX_COEF << " " << FREE_COEF << " " << MEAN_COEF << endl;

	UNIT_MS = config.get<int>("UNIT_MS", UNIT_MS); // will calculate for every second the "spector"
	RADIUS  = config.get<int>("RADIUS", RADIUS);;  // num of elems (discret buckets) affected around


	NTHREADS = config.get<int>("threads", NTHREADS);;  // num of elems (discret buckets) affected around



	NodeGraph ng;
	//reading flows in form start-time  src-ip dest-ip
	read_bidir_flows(ng);

	vector<std::thread> thread_list;
	/*
		NodeGraph::Vertex start_node = 0;
		int piece = nvert/nthreads;
		int rest = nvert % nthreads;
		for (int i = 0; i < nthreads; ++i){
			int end_node = start_node + piece;
			if ((rest--)>0) end_node++;
			//cout << endl << "---- " << start_node << " : " << end_node-1 << "--- " << endl;
			thread_list.push_back(std::thread(&PageRank::do_iteration, this, start_node, end_node-1, std::ref(for_others)));
			start_node = end_node;
		}
		for(auto& t : thread_list) {
			if(t.joinable())
				t.join();
		};
		thread_list.clear();
	*/
	const auto & resume = look_for_suspicious(ng, PERIODS);

	// DEBUG
	for(auto iter=resume.begin(); iter != resume.end(); ++iter){
		print_separator(iter->first);
		print_map(iter->second);
		//cout << iter->first << " -> " << iter->second << endl;
	}

}
