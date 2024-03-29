//
// Created by usuario on 13/12/23.
//

#include "graph.h"
#include <ranges>

Graph::Graph() {
    nodes.push_back(0);
}

int Graph::add_node()
{
    nodes.push_back(nodes.size());
    return nodes.size()-1;
}

int Graph::add_edge(int n1, int n2) {
    if (std::ranges::find(nodes, n1) != nodes.end() and
        std::ranges::find(nodes, n2) != nodes.end() and
        std::ranges::find(edges, std::make_pair(n1, n2)) == edges.end())
    {
        edges.emplace_back(n1, n2);
        return 1;
    }
     else return -1;
}


void Graph::print()
{
    std::cout << "Nodos del grafo: ";
    for (const auto &n : nodes)
    {
        std::cout<< n << " " ;
    }

    std::cout<<std::endl;

    std::cout << "Arcos del grafo: ";
    for (const auto &e : edges)
    {
        std::cout<< e.first << " "  << e.second << "  ";
    }

    std::cout<<std::endl;

}

int Graph::node_count() const {
    return nodes.size();
}