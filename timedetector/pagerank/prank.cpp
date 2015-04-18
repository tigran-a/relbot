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
#include "prank.hpp"


double PageRank::vdistance(const vector<double> &  p, const vector<double> & q) const{
	double result = 0.0;
	for (int i = 0; i< p.size(); i++)
		result += pow(q[i]-p[i], 2);
	return sqrt(result);
}
void PageRank::do_iteration(NodeGraph::Vertex start, NodeGraph::Vertex end, double& for_others){
	for (NodeGraph::Vertex v = start; v<= end; v++){
		double beta = this->alpha;
		auto inf = infected.find(v);
		if (inf != infected.end()){ // if v is known to be infected
			beta = 0.75+ beta/4;
		}

		// for each it's adjacent node
		int num_adj = g.get_num_adjacent_vertices(v);
		double to_all;
		if (num_adj == 0){// dangling node
			to_all = prank_prev[v]/nvert; // no coefficient, as all rank value is spread across all other nodes
		} else {
			to_all = 1.*(1-beta)*prank_prev[v]/(nvert-num_adj); // this will be added to rank of each node (kind of dangling part)
			for (auto av = g.get_adjacent_vertices(v); av.first != av.second ; ++av.first){
				//add to its rank a bit of v's one
#ifdef THREADS_ENABLED
				locks[*av.first]->lock();
#endif
				prank[*av.first] += beta * prank_prev[v]/ num_adj;  // here we may add coeff if v is an infected node? 
				prank[*av.first] -= to_all;  // adjustment, because we will add it to all ndoes
#ifdef THREADS_ENABLED
				locks[*av.first]->unlock();
#endif
				//// adding (1-alpha)stuff to the current host
				//prank[v] += (1.-beta)/nvert;
			}
		}
#ifdef THREADS_ENABLED
		dangling_lock.lock();
#endif
		for_others += to_all;
#ifdef THREADS_ENABLED
		dangling_lock.unlock();
#endif

	}	
}

vector<double>& PageRank::calculate(int nthreads){
	nvert = g.get_num_vertices();
#ifdef THREADS_ENABLED
	locks.reserve(nvert+1);
	locks.resize(nvert, NULL);
	for (NodeGraph::Vertex v = 0; v< nvert; v++){
		locks[v] = new mutex;
	}
#endif

	double a_bit = 1./nvert;

	prank_prev.reserve(nvert+1); 
	prank.reserve(nvert+1);
	prank.resize(nvert, 0.0);
	prank_prev.resize(nvert, a_bit);

	/*
	   fill(prank.begin(), prank.end(), 0.0);
	   for (NodeGraph::Vertex v = 0; v< nvert; v++)
	   prank_prev[v]=a_bit;
	//fill(prank_prev.begin(), prank.end(), a_bit); //crashes afterwards ?!
	*/	

	for (int steps = 0; steps < this->max_steps; steps++){
		//graph_traits< NodeGraph::Graph >::adjacency_iterator start, end;
		//cerr << "================= STEP " << steps << " =================" <<endl;

		double for_others = 0; //will be added to all ranks 

		// here need some parallelism: treating a group of nodes...
		// for each node
#ifdef THREADS_ENABLED
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
#else

		do_iteration(0, nvert-1, for_others);
#endif

		// adding dangling stuff
		for (NodeGraph::Vertex v = 0; v< nvert; ++v)
			prank[v] += for_others;

		// after each step we can get tribute from all nodes to the infected ones;
		// this tribute should decrease with each next step. 
		// For example (1- rank of all infected)/ step  / number-of-infected-nodes
		// can be sent to every infected node and proportionally removed from the others.
		//
		// This might be somehow reflected in a 'modified' formula for pagerank iteration

		//debug
		/*
		   cerr << "Prev: " << prank_prev.size() << endl;
		   for (NodeGraph::Vertex v = 0; v< nvert; v++)
		   cerr << prank_prev[v] << "\t";
		   cerr << endl << "curr: " <<prank.size() << endl;
		   for (NodeGraph::Vertex v = 0; v< nvert; v++)
		   cerr << prank[v] << "\t";
		   cerr << endl << "------------------------" << endl;


		   cout << endl << "========================" << endl;
		   */

		if (vdistance(prank, prank_prev) < this -> max_distance) {
			cout << "N of iterations: " <<  steps << " Distance: " << vdistance(prank, prank_prev) << endl;;
			break;
		}
		prank_prev = std::move(prank); 
		prank.reserve(nvert+1);
		prank.resize(nvert);
		fill(prank.begin(), prank.end(), 0);
	}	


#ifdef THREADS_ENABLED
	for (mutex* lock: locks) delete lock;
	locks.clear();
#endif
	return prank_prev;
}

void PageRank::set_infected_nodes(vector<NodeGraph::Vertex>& known){
	copy(known.begin(), known.end(), inserter(infected, infected.begin()));
}
