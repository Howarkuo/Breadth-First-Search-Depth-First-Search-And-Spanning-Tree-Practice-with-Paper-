#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Structure to represent a selected spanning tree edge
struct Edge {
    string src;
    string dest;
};

// Global / Helper variables for tracking DFS state
unordered_map<string, vector<string>> adjacencyList;
unordered_set<string> visited;
vector<Edge> spanningTreeEdges;

// Recursive DFS Function that records the Spanning Tree path
void dfsSpanningTree(const string& currentNode) {
    visited.insert(currentNode);

    // Explore all neighbors of the current node
    for (const string& neighbor : adjacencyList[currentNode]) {
        if (visited.find(neighbor) == visited.end()) {
            // Record this edge as part of the spanning tree path
            spanningTreeEdges.push_back({currentNode, neighbor});
            
            // Recursively dive deeper into the unvisited neighbor
            dfsSpanningTree(neighbor);
        }
    }
}

int main() {
    string fileName = "graph.txt";
    ifstream file(fileName);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << fileName << endl;
        return 1;
    }

    // Read the first line (Total Vertices and Edges counts)
    int numVertices, numEdges;
    if (!(file >> numVertices >> numEdges)) {
        cerr << "Error: Invalid file format." << endl;
        return 1;
    }

    // Read each subsequent line representing an edge
    string u, v;
    int weight;
    while (file >> u >> v >> weight) {
        // Since it's an undirected graph, add connections both ways
        adjacencyList[u].push_back(v);
        adjacencyList[v].push_back(u);
    }
    file.close();

    // Select a starting node
    string startNode = "A";

    // Generate the spanning tree using DFS
    dfsSpanningTree(startNode);

    // Output the selected path
    cout << "DFS Spanning Tree Edges (Starting from node " << startNode << "):" << endl;
    for (const auto& edge : spanningTreeEdges) {
        cout << edge.src << " -> " << edge.dest << endl;
    }

    return 0;
}
