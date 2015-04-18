// nodegraph.hpp
//
// Graph class
// prototype, SnT, 2014
//
//
#ifndef nodegraph_hpp
#define nodegraph_hpp

#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace boost;

// edge properties
struct timez_t {
	typedef edge_property_tag kind;
};
namespace boost {
	enum edge_timez_t { edge_timez };
	BOOST_INSTALL_PROPERTY(edge, timez);
};

typedef unsigned long int ts_t;

//vecS for vertex list
//setS for out edges  //vecS would eat much less memory
// directedS

class NodeGraph{
	public:
		typedef boost::property<boost::vertex_name_t, std::string> NameProp;
        typedef property<timez_t, vector<ts_t>> TimezProp;
		typedef boost::adjacency_list<boost::setS, boost::vecS, boost::directedS, NameProp, TimezProp> Graph;
		typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
		typedef boost::graph_traits < Graph >::edge_descriptor Edge;


	private:
		Graph g;
		property_map< Graph, vertex_name_t >::type pm; // ip by id
        property_map< Graph, timez_t>::type timez;
		map<string, Vertex> node_lookup; //map for looking vertex id by its ip
	public: 
		NodeGraph();
		Vertex getTarget(Edge e);
		Vertex getSource(Edge e);
		Vertex add_node(const string& ip);
		int get_num_adjacent_vertices(Vertex& v);
		pair<graph_traits< Graph >::adjacency_iterator,graph_traits< Graph >::adjacency_iterator>  get_adjacent_vertices(Vertex& v);
		graph_traits< Graph >::vertices_size_type get_num_vertices();
		void add_link(const Vertex& src, const Vertex& dest, const ts_t ts = 0);
		string get_node_name(Vertex v);
		vector<ts_t>& get_edge_times(Edge & e);
		void print_nodes();
		void print_edges();

		template<typename Func>
			void process_edges(Func f){
				for (auto es = edges(g); es.first != es.second; ++es.first){
					f(*es.first);
				}
			}
		template<typename Func>
			void process_nodes(Func f){
				for (auto vs = vertices(g); vs.first != vs.second; ++vs.first){
					f(*vs.first);
				}
			}
		template<typename Func>
			void process_out_edges(Vertex v, Func f){
				for (auto oes = out_edges(v, g); oes.first != oes.second; ++oes.first){
					f(*oes.first);
				}
			}

		void print_adjacent_vertices(Vertex v);

};


#endif

