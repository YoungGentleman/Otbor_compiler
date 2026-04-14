// tests.cpp
#include "cycle.hpp"
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

void run_test(const std::string& name, const std::string& input) {
    std::cout << "===" << name << "===\n";

    auto& gc = GraphCreator::getInstance();
    
    
    for (auto& p : gc.node_register) delete p.second;
    gc.node_register.clear();

    gc.readEdgesAndBuildGraph(input);

    Graph graph;
    graph.setChilds(&gc);
    graph.analize_loops();


    std::map<int, int> dfs_to_cid;       
    std::map<int, std::set<std::string>> blocks;
    std::map<int, std::pair<std::string, std::string>> info;        // cid -> {header_id, type}
    int next_cid = 1;

    for (auto& pair : gc.node_register) {
        Node* node = pair.second;
        if (node->DFS_number == -1) continue;

        Node* rep = graph.FIND(node);
        int rep_dfs = rep->DFS_number;

        int cid = 0;
        
        if (rep_dfs > 0 && rep->type != "nonheader") {
            if (dfs_to_cid.find(rep_dfs) == dfs_to_cid.end()) {
                dfs_to_cid[rep_dfs] = next_cid++;
            }
            cid = dfs_to_cid[rep_dfs];
        }
        blocks[cid].insert(node->id);
        if (cid > 0 && info.find(cid) == info.end()) {
            info[cid] = {rep->id, rep->type};
        }
    }

    std::cout << "Loop blocks:";
    for (auto& [cid, nodes] : blocks) {
        std::cout << " [" << cid << "]->{";
        bool first = true;
        for (const auto& n : nodes) {
            if (!first) std::cout << ", ";
            std::cout << n;
            first = false;
        }
        std::cout << "}";
        if (cid > 0) {
            std::cout << ", header: " << info[cid].first << ", " << info[cid].second;
        }
    }
    std::cout << "\n";

    std::cout << "Loop tree: {";
    std::vector<std::pair<int, int>> tree_edges;
    for (auto& [header_dfs, cid] : dfs_to_cid) {
        Node* h_node = gc.findDFS(header_dfs);
        int parent_dfs = h_node->header; 
        
        int parent_cid = 0;
        if (parent_dfs > 0 && dfs_to_cid.count(parent_dfs)) {
            parent_cid = dfs_to_cid[parent_dfs];
        }
        if (parent_cid != cid) {
            tree_edges.emplace_back(parent_cid, cid);
        }
    }
    std::sort(tree_edges.begin(), tree_edges.end());
    for (size_t i = 0; i < tree_edges.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "{" << tree_edges[i].first << "," << tree_edges[i].second << "}";
    }
    std::cout << "}\n\n";
}

int main() {
    
    run_test("Test 1", "{start, 1},{1,2},{2,1},{2,3},{3,1},{3,4},{4, stop}");
    run_test("Test 2", "{start,1},{1,2},{1,3},{2, 4},{3, 1},{3, stop},{4, 2},{4, stop}");
    run_test("Test 3", "{start,1},{1,2},{2,3},{2,4},{3, 4},{4, stop}");
    std::cout << "Tests finihed.\n";
    return 0;
}