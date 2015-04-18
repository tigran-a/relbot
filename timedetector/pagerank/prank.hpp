// 
// 
// Page Rank class
// 
// prank.hpp
#ifndef prank_hpp
#define prank_hpp

#include "nodegraph.hpp"
#include <thread>
#include <mutex>



#define THREADS_ENABLED
//#undef THREADS_ENABLED
using namespace boost;
using namespace std;

class PageRank{
	private:
		NodeGraph& g;
		size_t nvert; // number of verticles
		double alpha; 
		double max_distance;
		int max_steps;
		set<NodeGraph::Vertex> infected;
		vector<double> prank_prev, prank;
#ifdef THREADS_ENABLED
		vector<std::mutex *> locks;
		mutex dangling_lock;
#endif
		vector<std::thread> thread_list;

		double vdistance(const vector<double> &  p, const vector<double> & q) const;
		void do_iteration(NodeGraph::Vertex start, NodeGraph::Vertex end, double& for_others);
		
	public: 
		PageRank(NodeGraph& g, double alpha, int max_steps, double max_distance): g(g), alpha(alpha), max_steps(max_steps), max_distance(max_distance){
		};
		vector<double>& calculate(int nthreads);
		void set_infected_nodes(vector<NodeGraph::Vertex>& known);
};

#endif

