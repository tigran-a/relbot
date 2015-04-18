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
// prototype
#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include "misc.hpp"
#include "nodegraph.hpp"
#include "prank.hpp"

using namespace boost;
using namespace std;





void test(){
	NodeGraph ng;
	PageRank pr(ng, 0.85, 10, 0.02);
	NodeGraph::Vertex v0, v1, v2;
	string s;
	s= string("1.2.3.4");
	v0 = ng.add_node(s);
	v0 = ng.add_node(s);
	v0 = ng.add_node(s);
	s = string("dead:beef::1");
	v2 = v1 = ng.add_node(s);
	s = string("10.0.0.3");
	//cin >> s;
	ng.add_node(s);
	ng.add_link(v0,v1);
	ng.add_link(v0,v1);
	ng.add_link(v0,v1);
	ng.add_link(v0,v1);
	v1 = ng.add_node(string("13.32.42.31"));
	ng.add_link(v0,v1);
	ng.add_link(v2,v1);
	ng.print_nodes();
	ng.print_edges();
	ng.print_adjacent_vertices(v0);
	cout << "Num of vertices: " << ng.get_num_vertices() << endl;

	vector<double> & rank=	pr.calculate(1);
	cout << "The pranks are: " <<endl; 
	for (int v = 0; v< rank.size(); v++)
		cout << rank[v] << "\t";


}

void test2(){
	NodeGraph ng;
	PageRank pr(ng, 0.85, 1000, (double) 0.002);
	ng.add_node(string("10.0.0.1"));
	ng.add_node(string("10.0.0.2"));
	ng.add_node(string("10.0.0.3"));
	ng.add_node(string("10.0.0.4"));
	ng.add_node(string("10.0.0.5"));
	ng.add_node(string("10.0.0.6"));
	ng.add_node(string("10.0.0.7"));
	ng.add_link(0,1);
	ng.add_link(1,6);
	ng.add_link(2,1);
	ng.add_link(2,3);
	ng.add_link(3,2);
	ng.add_link(3,4);
	ng.add_link(4,5);
	ng.add_link(4,6);
	ng.add_link(6,2);
	ng.add_link(6,1);
	ng.print_nodes();
	ng.print_edges();
	ng.print_adjacent_vertices(6);
	cout << "Num of vertices: " << ng.get_num_vertices() << endl;
	vector<NodeGraph::Vertex> infected;
        infected.push_back(3);	
	pr.set_infected_nodes(infected);

	vector<double> & rank=	pr.calculate(3);
	cout << "The pranks are: " <<endl; 
	for (int v = 0; v< rank.size(); v++)
		cout << rank[v] << "\t";


}


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


void out_time_deltas(NodeGraph& ng){
  cout << "Num of vertices: " << ng.get_num_vertices() << endl;


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

int main()
{
  //test2();
  //exit(0);
  ///*
  NodeGraph ng;
  read_bidir_flows(ng);

//  ng.print_nodes();
//  ng.print_edges();

  out_time_deltas(ng);
  return EXIT_SUCCESS;
  
  PageRank pr(ng, 0.85, 1000, (double) 0.00000002);
  cout << "Num of vertices: " << ng.get_num_vertices() << endl;

  vector<double> & rank=	pr.calculate(3);
  ///*
  cout << "The pranks are: " <<endl; 
  for (int v = 0; v< rank.size(); v++)
  cout << v<<":"<<rank[v] << endl;
  //*/
  double sum = 0;
  double maximum = 0;
  for (double r : rank){
	  sum += r;
	  if (r > maximum) maximum = r;
  }
  cout << "Sum of ranks: " << sum << endl;
  cerr << "Max rank: " << maximum <<endl;

  return EXIT_SUCCESS;
}


