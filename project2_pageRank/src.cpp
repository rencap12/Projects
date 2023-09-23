#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>
#include <utility>
#include <iomanip>

using namespace std;

// Function to calculate the out-degree of a page from the adjacency list
int calculateOutDegree(const string& page, const map<string, vector<pair<string, double>>>& adjacencyList) {
    auto it = adjacencyList.find(page);
    if (it != adjacencyList.end()) {
        return it->second.size();
    }
    return 0;
}

// Function to calculate the page rank using power iteration
map<string, double> calculatePageRank(const map<string, vector<pair<string, double>>>& adjacencyList, int p) {
    const int numPages = adjacencyList.size();
    const double initialPageRank = 1.0 / numPages;

    const double tolerance = 1e-6; // Tolerance for convergence
    const int maxIterations = 100; // Maximum number of iterations

    // Initialize all page ranks to 1/N
    map<string, double> pageRank;
    for (const auto& page : adjacencyList) {
        pageRank[page.first] = initialPageRank;
    }

    for (int iteration = 0; iteration < p; ++iteration) {
        map<string, double> newPageRank;

        // Calculate new page ranks based on existing ranks
        for (const auto& node : adjacencyList) {
            const string& currentPage = node.first;
            double currentPageRank = 0.0;

            for (const auto& neighbor : node.second) {
                const string& neighborPage = neighbor.first;
                int outDegree = calculateOutDegree(neighborPage, adjacencyList);

                currentPageRank += pageRank[neighborPage] / outDegree;
            }

            newPageRank[currentPage] = currentPageRank;
        }

        double delta = 0.0; // Change in page ranks

        // Update page ranks and calculate delta
        for (const auto& page : adjacencyList) {
            const string& currentPage = page.first;
            newPageRank[currentPage] = newPageRank[currentPage] / numPages;
            delta += abs(newPageRank[currentPage] - pageRank[currentPage]);
            pageRank[currentPage] = newPageRank[currentPage];
        }

     //   if (delta < tolerance) {
       //     cout << "Converged after " << iteration + 1 << " iterations.\n";
         //   break;
        //}
    }

    return pageRank;
}

int main() {
    // Example adjacency list
    int n;  // # of lines to read
    cin >> n;

    int p; // # of power iter.
    cin >> p;

    map<string, vector<pair<string, double>>> adjacencyList;

    string from;
    string to;
    for(int i = 0; i < n; i++) { // read all lines
        string line;
        getline(cin, line);
        istringstream in(line);

       // string from; //++ out_deg
        in >> from;

     //   string to;  //++ in_degree
        in >> to;

        pair<string, double> add (to, 1.0); //initialize
        adjacencyList[from].push_back(add);

    }
        map<string, double> pageRank = calculatePageRank(adjacencyList, p);

        //add vertices/edge (from -> to) to adj list


   // map<string, double> pageRank = calculatePageRank(adjacencyList);

    // Output the page ranks
    cout << "Page Ranks:\n";
    for (const auto& rank : pageRank) {
        cout << "Page " << rank.first << ": " << rank.second << "\n";
    }

    return 0;
}
