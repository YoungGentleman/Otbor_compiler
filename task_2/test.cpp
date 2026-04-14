#include "phi_insertion.hpp"
#include <iostream>

int main() {
    const int N = 7;
    const int START = 0;

    PhiNodePlacer placer(N, START);

    placer.addEdge(0, 1); placer.addEdge(1, 2); placer.addEdge(1, 3);
    placer.addEdge(2, 4); placer.addEdge(3, 4); placer.addEdge(3, 5);
    placer.addEdge(4, 1); placer.addEdge(4, 5); placer.addEdge(5, 6);

    placer.setInitialDefs({2, 3, 4});

    auto phiNodes = placer.compute();

    std::cout << "PHI nodes: { ";
    bool first = true;
    for (int nd : phiNodes) {
        if (!first) std::cout << ", ";
        std::cout << nd;
        first = false;
    }
    std::cout << " }\n";
    return 0;
}