#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <vector>
#include <utility>
#include <iomanip>
using namespace std;

class AdjacencyList {
private:
    map<string, double> final;
    map<string, vector<pair<string, double>>> adjList; //node, vec of pairs

public:
    void pageRank(int p) {

        std::map<std::string, double> pageRank;
        int count = 0;
        for (const auto& entry : adjList) {
            count++;
        } // get # of pages

        // Initialize page ranks to 1.0/n for all pages
        for (const auto& entry : adjList) {
            pageRank[entry.first] = 1.0 / count;
        }

        for (int iteration = 0; iteration < p; ++iteration) {
            std::map<std::string, double> newPageRank;

            // Calculate new page ranks based on existing ranks
            for (const auto &entry: adjList) {
                const std::string &page = entry.first;
                const std::vector<std::pair<std::string, double>> &pointingPages = entry.second;
                const int numPointingPages = pointingPages.size();

                for (const auto &pointingPage: pointingPages) {
                    const std::string &pageName = pointingPage.first;
                    const double weight = pointingPage.second;

                    newPageRank[pageName] += (pageRank[page] * weight) / numPointingPages;
                }
            }

            double delta = 0.0; // Change in page ranks

            // Apply damping factor and calculate delta
            for (auto &entry: newPageRank) {
                const std::string &page = entry.first;
                const double rank = entry.second;

                entry.second = rank + ((1.0 ) / adjList.size());

                delta += std::abs(entry.second - pageRank[page]);
            }

            pageRank = newPageRank;

        }
        for (auto fit = pageRank.begin(); fit != pageRank.end() ; ++fit) {
            cout << fit->first << " " << setprecision(3) << fit->second << endl;;
        }

        /*   int num_nodes = adjList.size();
           map<string, double> ranks;
           for(auto it = adjList.begin(); it != adjList.end(); ++it){
              ranks[it->first] = 1.0/num_nodes; //initialize)
           }


           for(int power = 0; power < p ; power++) {
              vector<double> ranks_new(num_nodes,0.0);
               for(auto it = adjList.begin(); it != adjList.end(); ++it){
                   int num_pages = it->first.size();
                   for (int count = 0; count < num_pages; ++count) {
                       ranks_new[count] += ranks[it->first] / num_pages;
                   }

               }

               double delta = 0.0; // Change in page ranks

               // Update page ranks and calculate delta
               for (int page = 0; page < num_nodes; ++page) {
                   ranks_new[page] = ranks_new[page] / num_nodes;
                   delta += std::abs(ranks_new[page] - ranks[page]);
                   ranks[page] = ranks_new[page];
               }

*/

    }
    void addToList(string nameFrom, const pair<string, double>& temp){
        adjList[nameFrom].push_back(temp);
        bool exist = repeat(temp.first);
        if(!exist){ //look at pointing to, create if not there in list
           cout << "temp from only receiving: " << temp.first << endl;
            pair<string, double> holder (temp.first,0.0);
            adjList[temp.first].push_back(holder);
        }

    }

    bool repeat(const string& _name){
        for(auto it = adjList.begin(); it != adjList.end(); ++it){
            if( it->first == _name){
                return true; //already in ranks map
            }
        }
        return false;
    }


};

int main() {
    int n;  // # of lines to read
    cin >> n;

    int p; // # of power iter.
    cin >> p;

    double initial = 1.0; // 1/total # nodes(vertices)

    AdjacencyList* adj = new AdjacencyList();

    for(int i = 0; i < n; i++){ // read all lines
        string line;
        getline(cin, line);
        istringstream in(line);

        string from; //++ out_deg
        in >> from;

        string to;  //++ in_degree
        in >> to;

        pair<string, double> add (to, initial); //initialize
        adj->addToList(from,add);
        //add vertices/edge (from -> to) to adj list
    }

    adj->pageRank(p);

    return 0;
}
