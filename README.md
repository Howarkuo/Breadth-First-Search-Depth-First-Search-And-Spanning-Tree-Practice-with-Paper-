# Breadth First Search, Depth First Search And Spanning Tree 

![Screenshot 2026-06-25 at 7.22.26 PM](https://hackmd.io/_uploads/ryadkq9MMg.png)
![Screenshot 2026-06-25 at 7.22.34 PM](https://hackmd.io/_uploads/rkSFJ5cGzg.png)
```c++
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

```
## How to count Edges and Vertices for the Graph
- graph.txt
- Count Vertices / Nodes: Unique letters (A,B..) : 9
- Count Edges: Unique letters (A,B..) :each line represents exactly one edge connecting two vertices (with the third column showing its weight).

Simply count the total number of rows/lines of data you have:

A  B  2 (Edge 1)

A  C  5 (Edge 2)

B  D  9 (Edge 3)

B  E  3 (Edge 4)

C  F  8 (Edge 5)

C  G  11 (Edge 6)

D  H  6 (Edge 7)

E  F  1 (Edge 8)

E  I  13 (Edge 9)

G  I  7 (Edge 10)

H  I  15 (Edge 11)

Count: There are 11 edges total.
```
A B 2
A C 5
B D 9
B E 3
C F 8
C G 11
D H 6
E F 1
E I 13
G I 7
H I 15
```

- Explanation & Output
```
DFS Spanning Tree Edges (Starting from node A):
A -> B
B -> D
D -> H
H -> I
I -> E
E -> F
F -> C
C -> G
```

![Screenshot 2026-06-25 at 8.19.33 PM](https://hackmd.io/_uploads/H1G16q5MGx.png)


![Screenshot 2026-06-25 at 8.19.04 PM](https://hackmd.io/_uploads/ryVa2q9Mfx.png)



- Maximum Common Subgraph (MCS) via an Association Graph: Instead of traditional structural alignment, pairs or sets of 3D structures are broken down, matched locally, and mapped into a massive modular Association Graph.

- Modified Depth-First Search (DFS): Used to traverse this association graph to extract globally coherent configurations (Frame Graphs).

- Local Clique Identification: Embedded inside the DFS step to check if newly added association nodes are mutually compatible (clique condition) under a strict global distance deviation threshold.

```c++

C++ Architecture Implementation
C++
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <set>
#include <algorithm>

// Represents a 3D atom/residue coordinate from the structural files
struct Point3D {
    double x, y, z;
    double distance(const Point3D& other) const {
        return std::sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y) + (z-other.z)*(z-other.z));
    }
};

// A node in an individual pMHC Surface Graph
struct SurfaceNode {
    int id;
    std::string label; // Amino acid or physicochemical class token
    Point3D coords;
};

// An Association Node represents a matched pair/set of nodes across input structures 
// As cited in Simões et al. (2026), it pairs equivalent structural elements
struct AssocNode {
    int id;
    SurfaceNode g1_node;
    SurfaceNode g2_node;
};

class MHCXGraphProcessor {
private:
    std::vector<AssocNode> assocNodes;
    std::map<int, std::vector<int>> assocAdjList; // Adjacency list for the Association Graph
    double globalDistanceThreshold;

    // Checks if a collection of Association Nodes forms a valid Clique 
    // satisfies the global consistency rule described in Figure 1C of the paper
    bool isCoherentClique(const std::vector<int>& currentFrame, int candidateIdx) {
        const AssocNode& candidate = assocNodes[candidateIdx];
        
        for (int activeIdx : currentFrame) {
            const AssocNode& active = assocNodes[activeIdx];
            
            // Calculate distance in Structure 1 and Structure 2
            double distG1 = active.g1_node.coords.distance(candidate.g1_node.coords);
            double distG2 = active.g2_node.coords.distance(candidate.g2_node.coords);
            
            // Global Consistency: The variation between structural distances must be under the threshold
            if (std::abs(distG1 - distG2) > globalDistanceThreshold) {
                return false; 
            }
        }
        return true;
    }

    // Modified Depth-First Search combined with Local Clique Selection
    void modifiedDFS(int u, std::vector<int>& currentFrame, std::vector<bool>& visited, 
                     std::vector<std::vector<int>>& maximalFrames) {
        
        visited[u] = true;
        currentFrame.push_back(u);

        bool extended = false;
        // Explore neighbors in the association structure
        for (int neighbor : assocAdjList[u]) {
            if (!visited[neighbor]) {
                // Local Clique Search constraints the DFS progression dynamically
                if (isCoherentClique(currentFrame, neighbor)) {
                    extended = true;
                    modifiedDFS(neighbor, currentFrame, visited, maximalFrames);
                }
            }
        }

        // If it's a leaf/dead-end for coherent expansion, record the Frame Graph if substantial
        if (!extended && currentFrame.size() >= 3) { 
            maximalFrames.push_back(currentFrame);
        }

        // Backtrack
        currentFrame.pop_back();
        visited[u] = false; 
    }

public:
    MHCXGraphProcessor(double threshold) : globalDistanceThreshold(threshold) {}

    // Step 1: Construct the Association Graph based on localized similarities
    void buildAssociationGraph(const std::vector<SurfaceNode>& graph1, const std::vector<SurfaceNode>& graph2) {
        int assocId = 0;
        // 1. Generate Association Nodes by token compatibility (Simplified matching rule)
        for (const auto& n1 : graph1) {
            for (const auto& n2 : graph2) {
                if (n1.label == n2.label) { // Token match/Triad alignment criteria
                    assocNodes.push_back({assocId++, n1, n2});
                }
            }
        }

        // 2. Build edges where local compatibility thresholds align
        for (size_t i = 0; i < assocNodes.size(); ++i) {
            for (size_t j = i + 1; j < assocNodes.size(); ++j) {
                // Pre-condition: Nodes cannot map to duplicates of the same exact source node
                if (assocNodes[i].g1_node.id != assocNodes[j].g1_node.id && 
                    assocNodes[i].g2_node.id != assocNodes[j].g2_node.id) {
                    
                    // Connect tentatively; global coherence checks are pushed to clique pruning
                    assocAdjList[assocNodes[i].id].push_back(assocNodes[j].id);
                    assocAdjList[assocNodes[j].id].push_back(assocNodes[i].id);
                }
            }
        }
        std::cout << "[Association Graph Built] Nodes: " << assocNodes.size() << std::endl;
    }

    // Extraction Engine for Maximum Common Subgraph using Frame Graphs
    void extractCoherentFrameGraphs() {
        std::vector<std::vector<int>> maximalFrames;
        std::vector<bool> visited(assocNodes.size(), false);

        for (size_t i = 0; i < assocNodes.size(); ++i) {
            std::vector<int> currentFrame;
            modifiedDFS(i, currentFrame, visited, maximalFrames);
        }

        // Output Resulting Coherent Subgraphs
        std::cout << "\n--- Extracted Coherent Frame Graphs (MCS Approximations) ---" << std::endl;
        for (size_t i = 0; i < maximalFrames.size(); ++i) {
            std::cout << "Frame Graph #" << i + 1 << " (Size " << maximalFrames[i].size() << "): ";
            for (int nodeId : maximalFrames[i]) {
                std::cout << "(" << assocNodes[nodeId].g1_node.label << ":" 
                          << assocNodes[nodeId].g1_node.id << "<->" << assocNodes[nodeId].g2_node.id << ") ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    // Mock surface graph 1 (e.g., Target pMHC structure)
    std::vector<SurfaceNode> pMHC_1 = {
        {0, "ARG", {1.0, 2.0, 3.0}},
        {1, "GLY", {4.0, 5.0, 6.0}},
        {2, "LEU", {7.0, 8.0, 9.0}}
    };

    // Mock surface graph 2 (e.g., Screened Cross-Reactive Candidate)
    std::vector<SurfaceNode> pMHC_2 = {
        {0, "ARG", {1.1, 2.1, 3.1}},
        {1, "GLY", {4.2, 5.1, 6.0}},
        {2, "LEU", {7.1, 7.9, 9.2}}
    };

    // Instantiate with global distance threshold = 1.5 Angstroms
    MHCXGraphProcessor processor(1.5);
    
    processor.buildAssociationGraph(pMHC_1, pMHC_2);
    processor.extractCoherentFrameGraphs();

    return 0;
```
Complexity & Design Considerations
Maximum Common Subgraph (MCS) Mitigation: Finding the exact MCS is an NP-complete problem. By building an Association Graph out of local features first and relying on the isCoherentClique rule during DFS traversal, the problem scales efficiently for structural interfaces without having to execute general, unbounded graph isomorphism strategies.

The Clique Condition: As detailed in Simões et al. (2026), the combination of local distance checks inside a backtracking configuration space effectively strips out geometric incoherence, yielding perfectly rigid cross-reactive structural matches.