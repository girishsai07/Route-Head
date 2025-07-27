#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

const int INF = numeric_limits<int>::max();

class Vertex {
public:
  int id; 
  vector<tuple<int, int, string>> adjacentEdges; 

  Vertex(int id) : id(id) {}
};

class Graph {
public:
  vector<Vertex> vertices;
  unordered_map<string, int> locationToIndex;
  unordered_map<int, string> indexToLocation;

  Graph(int numVertices) {
    vertices.reserve(numVertices);
    for (int i = 0; i < numVertices; ++i) {
      vertices.emplace_back(i);
    }
  }

  void addEdge(int src, int dest, int weight, const string &directionSrcToDest,
               const string &directionDestToSrc) {
    vertices[src].adjacentEdges.push_back({dest, weight, directionSrcToDest});
    vertices[dest].adjacentEdges.push_back(
        {src, weight, directionDestToSrc}); 
  }

  void addLocation(const string &location, int index) {
    locationToIndex[location] = index;
    indexToLocation[index] = location;
  }

  int getLocationIndex(const string &location) {
    return locationToIndex[location];
  }

  string getLocationName(int index) { return indexToLocation[index]; }
};

class Dijkstra {
public:
  static tuple<vector<int>, vector<int>, int> shortestPath(Graph &graph,
                                                           int start, int end) {
    vector<int> distances(graph.vertices.size(), INF);
    vector<int> predecessors(graph.vertices.size(), -1); 
    vector<bool> visited(graph.vertices.size(), false);
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>> pq;

    distances[start] = 0;
    pq.push(make_pair(0, start));

    while (!pq.empty()) {
      int currIndex = pq.top().second;
      pq.pop();

      if (visited[currIndex]) {
        continue;
      }

      visited[currIndex] = true;

      for (const auto &neighbor : graph.vertices[currIndex].adjacentEdges) {
        int nextIndex = get<0>(neighbor);
        int weight = get<1>(neighbor);

        if (distances[nextIndex] > distances[currIndex] + weight) {
          distances[nextIndex] = distances[currIndex] + weight;
          predecessors[nextIndex] = currIndex; 
          pq.push(make_pair(distances[nextIndex], nextIndex));
        }
      }
    }

    vector<int> shortestPath;
    if (distances[end] == INF) {
      cout << "No path exists between " << start << " and " << end << endl;
      return {shortestPath, predecessors, -1};
    }

    
    for (int at = end; at != -1; at = predecessors[at]) {
      shortestPath.push_back(at);
    }
    reverse(shortestPath.begin(), shortestPath.end());

    int totalCost = distances[end];
    return {shortestPath, predecessors, totalCost};
  }
};

struct SplayNode {
  string buildingName;
  SplayNode *left;
  SplayNode *right;
  SplayNode(const string &name)
      : buildingName(name), left(nullptr), right(nullptr) {}
};

class SplayTree {
private:
  SplayNode *root;

  SplayNode *rightRotate(SplayNode *x) {
    SplayNode *y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
  }

  SplayNode *leftRotate(SplayNode *x) {
    SplayNode *y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
  }

  SplayNode *splay(SplayNode *root, const string &key) {
    if (!root || root->buildingName == key)
      return root;
    if (root->buildingName > key) {
      if (!root->left)
        return root;
      if (root->left->buildingName > key) {
        root->left->left = splay(root->left->left, key);
        root = rightRotate(root);
      } else if (root->left->buildingName < key) {
        root->left->right = splay(root->left->right, key);
        if (root->left->right)
          root->left = leftRotate(root->left);
      }
      return root->left ? rightRotate(root) : root;
    } else {
      if (!root->right)
        return root;
      if (root->right->buildingName > key) {
        root->right->left = splay(root->right->left, key);
        if (root->right->left)
          root->right = rightRotate(root->right);
      } else if (root->right->buildingName < key) {
        root->right->right = splay(root->right->right, key);
        root = leftRotate(root);
      }
      return root->right ? leftRotate(root) : root;
    }
  }

public:
  SplayTree() : root(nullptr) {}

  void insert(const string &name) {
    if (!root) {
      root = new SplayNode(name);
      return;
    }
    root = splay(root, name);
    if (root->buildingName == name)
      return;
    SplayNode *newNode = new SplayNode(name);
    if (root->buildingName > name) {
      newNode->right = root;
      newNode->left = root->left;
      root->left = nullptr;
    } else {
      newNode->left = root;
      newNode->right = root->right;
      root->right = nullptr;
    }
    root = newNode;
  }

  string search(const string &name) {
    root = splay(root, name);
    return root->buildingName;
  }
};

void printDirections(Graph &graph, const vector<int> &shortestPath,
                     const vector<int> &predecessors) {
  for (size_t i = 0; i < shortestPath.size() - 1; ++i) {
    int curr = shortestPath[i];
    int next = shortestPath[i + 1];

    for (const auto &edge : graph.vertices[curr].adjacentEdges) {
      if (get<0>(edge) == next) {
        cout << "Go " << get<2>(edge) << " for " << get<1>(edge)
             << " meters from " << graph.getLocationName(curr) << " to "
             << graph.getLocationName(next) << "." << endl;
        break;
      }
    }
  }
}

void printSearchHistory(stack<pair<string, string>> &searchHistory) {
  stack<pair<string, string>> tempStack = searchHistory;
  cout << "\nSearch History (Last 5 searches):\n";
  while (!tempStack.empty()) {
    pair<string, string> search = tempStack.top();
    tempStack.pop();
    cout << "From: " << search.first << " To: " << search.second << endl;
  }
}

int main() {
  string locations[] = {
    "sap_college", "ac_tech_college", "ac_tech_canteen", "main_gate", "anna_statue", 
    "red_building", "s&h", "cs_department", "it_department", "knowledge_park", 
    "ece_department", "ncc_building", "eee_department", "tag_audi", 
    "mechanical_department", "thazham", "ceg_canteen", "international_hostels", 
    "gurunath", "senior_mess", "hostel_office", "health_center", "ceg_tech_forum", 
    "alumni_center", "cuic", "kottur_gate", "chemistry_department", "vivek_audi", 
    "ground", "sbi", "civil_department", "acoe", "coffee_hut", "mess_road", 
    "physics_department", "eee_junction", "civil_road", "guest_house_road"
  };

  stack<pair<string, string>> searchHistory;
  do {
    cout << "Menu:\n";
    for (int i = 0; i < 38; i++) {
      cout << i << ". " << locations[i] << endl;
    }
    SplayTree Splay;
    ifstream input("hii.txt");
    int numvertices, numedges;
    input >> numvertices >> numedges;

    Graph campusGraph(numvertices);
    for (int i = 0; i < numedges; i++) {
      int src, dest, weight;
      string srcdir, desdir;
      input >> src >> dest >> weight >> srcdir >> desdir;
      campusGraph.addEdge(src, dest, weight, srcdir, desdir);
    }
    for (int i = 0; i < numvertices; i++) {
      string location;
      int vertex;
      input >> location >> vertex;
      campusGraph.addLocation(location, vertex);
      Splay.insert(location);
    }
    input.close();

    string startLocation, endLocation;

    while (true) {
      cout << "ENTER START LOCATION: ";
      cin >> startLocation;

      while (campusGraph.locationToIndex.count(startLocation) == 0) {
        string suggestion = Splay.search(startLocation);
        cout << "DID YOU MEAN THIS? " << suggestion << " (y/n): ";
        char response;
        cin >> response;
        if (response == 'y' || response == 'Y') {
          startLocation = suggestion;
        } else {
          cout << "Please enter a valid start location: ";
          cin >> startLocation;
        }
      }

      cout << "ENTER DESTINATION: ";
      cin >> endLocation;

      while (campusGraph.locationToIndex.count(endLocation) == 0) {
        string suggestion = Splay.search(endLocation);
        cout << "DID YOU MEAN THIS? " << suggestion << " (y/n): ";
        char response;
        cin >> response;
        if (response == 'y' || response == 'Y') {
          endLocation = suggestion;
        } else {
          cout << "Please enter a valid destination: ";
          cin >> endLocation;
        }
      }

      int start = campusGraph.getLocationIndex(startLocation);
      int end = campusGraph.getLocationIndex(endLocation);

    
      tuple<vector<int>, vector<int>, int> result = Dijkstra::shortestPath(campusGraph, start, end);
      vector<int> shortestPath = get<0>(result);
      vector<int> predecessors = get<1>(result);
      int totalCost = get<2>(result);

    
      cout << "The shortest path from " << startLocation << " to " << endLocation << " is: ";
      for (int vertex : shortestPath) {
        cout << campusGraph.getLocationName(vertex) << " ";
        if (vertex != end) {
          cout << "-> ";
        }
      }
      cout << endl;

      
      printDirections(campusGraph, shortestPath, predecessors);

    
      cout << "Total cost: " << totalCost << " meters." << endl;

      
      searchHistory.push({startLocation, endLocation});
      if (searchHistory.size() > 5) {
        searchHistory.pop();
      }

      
      cout << "Do you want to see the search history? (y/n): ";
      char showHistory;
      cin >> showHistory;
      if (showHistory == 'y' || showHistory == 'Y') {
        printSearchHistory(searchHistory);
      }
    }
  } while (true);

  return 0;
}