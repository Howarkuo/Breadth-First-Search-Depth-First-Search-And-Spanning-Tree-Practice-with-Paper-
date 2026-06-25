#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// Structure to represent a selected spanning tree edge
struct Edge {
    string src;
    string dest;
};

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

    // Build the Adjacency List
    unordered_map<string, vector<string>> adjacencyList;
    string u, v;
    int weight;
    while (file >> u >> v >> weight) {
        // Undirected graph setup
        adjacencyList[u].push_back(v);
        adjacencyList[v].push_back(u);
    }
    file.close();

    // BFS Variables
    string startNode = "A";
    unordered_set<string> visited;
    queue<string> q;
    vector<Edge> spanningTreeEdges;

    // Initialize BFS
    q.push(startNode);
    visited.insert(startNode);

    // Run Breadth First Search
    while (!q.empty()) {
        string current = q.front();
        q.pop();

        // Explore all neighbors layer by layer
        for (const string& neighbor : adjacencyList[current]) {
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
                
                // Record the edge mapping to our Spanning Tree path
                spanningTreeEdges.push_back({current, neighbor});
            }
        }
    }

    // Output the resulting path
    cout << "BFS Spanning Tree Edges (Starting from node " << startNode << "):" << endl;
    for (const auto& edge : spanningTreeEdges) {
        cout << edge.src << " -> " << edge.dest << endl;
    }

    return 0;
}
