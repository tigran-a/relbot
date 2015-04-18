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
#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include "misc.hpp"
#include "nodegraph.hpp"


NodeGraph::NodeGraph(){
	pm = get(vertex_name_t(), g); 
	timez = get(timez_t(), g);
}
NodeGraph::Vertex NodeGraph::getTarget(Edge e){
	return target(e,g);
}
NodeGraph::Vertex NodeGraph::getSource(Edge e){
	return source(e,g);
}

NodeGraph::Vertex NodeGraph::add_node(const string& ip){
	// adds a node and returns its id/descriptor 
	auto found = node_lookup.find(ip);
	if (found == node_lookup.end()) // if not found 
		return node_lookup[ip] = add_vertex(ip, g); //adding new node; saving to lookup and return id
	else //already there
		return found -> second; // return found id
}
int NodeGraph::get_num_adjacent_vertices(NodeGraph::Vertex& v){
	return distance(adjacent_vertices(v, g));
}
pair<graph_traits< NodeGraph::Graph >::adjacency_iterator,graph_traits< NodeGraph::Graph >::adjacency_iterator>  NodeGraph::get_adjacent_vertices(NodeGraph::Vertex& v){
	return adjacent_vertices(v, g);
}
graph_traits< NodeGraph::Graph >::vertices_size_type NodeGraph::get_num_vertices(){
	return num_vertices(g);
}
void NodeGraph::add_link(const NodeGraph::Vertex& src, const NodeGraph::Vertex& dest, const ts_t ts ){
	// if ts is 0, no ts is added to the prop. list
	Edge e;
	bool is_new;
	tie(e, is_new) =  add_edge(src, dest, g);
	if (ts != 0 ) {
		timez[e].push_back(ts);
	}

	/*
	   cerr << " New edge: " << is_new << endl; 
	   cerr << " Edge: " << e << endl; 
	   for (unsigned long int t : timez[e]){
	   cerr << t << "\t"; 
	   }
	   cerr << endl;
	   */
}

string NodeGraph::get_node_name(NodeGraph::Vertex v){
	return pm[v];
}
vector<ts_t>& NodeGraph::get_edge_times(Edge & e){
	return timez[e];
}

void NodeGraph::print_nodes(){
	print_separator(string("The nodes are:"));
	for (auto vs = vertices(g); vs.first != vs.second; ++vs.first){
		cout << *vs.first << " (" << pm[*vs.first] << ")" << endl;
	}
	print_separator();
}
void NodeGraph::print_edges(){
	print_separator(string("The edges are:"));
	for (auto es = edges(g); es.first != es.second; ++es.first){
		cout << *es.first << endl;
	}
	print_separator();
}

void NodeGraph::print_adjacent_vertices(NodeGraph::Vertex v){
	print_separator(string("The adjacent vertices for some vertex:"));
	for (auto av = this-> get_adjacent_vertices(v); av.first != av.second ; ++av.first)
		cout << *av.first  << " ";
	cout << endl;
	print_separator();
}


