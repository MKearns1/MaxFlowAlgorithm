#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>

using namespace std;



string userinput;
int NumNodes, EndNode;
bool validGraph = true;

int ReadNumberOfNodes(string FileName);
void InputFileName();

vector<vector<int>> capacity;
vector<vector<int>> adj;


class Edge
{
public:
    int capacity;
    int flow;
    int target;
    Edge* Inverse;

    Edge(int c, int f, int t)
    {
        capacity = c;
        flow = f;
        target = t;
    }

};

vector<vector<Edge*>> AdjacenceyList;

int bfs(int s, int t, vector<Edge*>& previousNode) {

    bool* visited = new bool[previousNode.size()];
    for (int x = 0; x < previousNode.size(); x++)
    {
        visited[x] = false;
    }
        
    vector<int> flow;

    fill(previousNode.begin(), previousNode.end(), nullptr);
   // previousNode[s] = -2;
    visited[s] = true;
    queue<pair<int, int>> q;
    q.push({ s, INT_MAX });
    //cout << "PATH TO " << t << " is ";
    while (!q.empty()) {
        int currentNode = q.front().first;
        int flow = q.front().second;
        q.pop();

        //for (int next : adj[currentNode]) {
        //   // cout << parent[next] << endl;
        //    if (parent[next] == -1 && capacity[currentNode][next] > 0) {
        //        parent[next] = currentNode;
        //        int new_flow = min(flow, capacity[currentNode][next]);
        //        if (next == t)
        //            return new_flow;
        //        q.push({ next, new_flow });
        //    }
        //}

        for (int i = 0; i < AdjacenceyList.at(currentNode).size(); i++)
        {
            int TargetNode = AdjacenceyList.at(currentNode).at(i)->target;
            int ResidualEdgeCapacity = AdjacenceyList.at(currentNode).at(i)->capacity - AdjacenceyList.at(currentNode).at(i)->flow;
            

            if(!visited[TargetNode])
            {
                if (ResidualEdgeCapacity > 0)
                {
                    //cout << TargetNode << "->";
                    visited[TargetNode] = true;

                    previousNode.at(TargetNode) = AdjacenceyList.at(currentNode).at(i);
                    int BottleNeck;
                    if (flow < AdjacenceyList.at(currentNode).at(i)->capacity)
                    {
                        BottleNeck = flow;
                    }
                    else 
                    {
                        BottleNeck = AdjacenceyList.at(currentNode).at(i)->capacity;
                    }
                    if (AdjacenceyList.at(currentNode).at(i)->target == t)
                    {
                        cout << "bottleneck was " << BottleNeck<<endl;
                        return BottleNeck;
                    }
                    else { q.push({ TargetNode, BottleNeck }); }
                }               
            }
        }
    }
    return 0;
}

int edmondsKarp(int s, int t) {
    int flow = 0;
    vector<Edge*> previousNode(NumNodes);
    int new_flow = bfs(s, t, previousNode);

    while (new_flow) {
        flow += new_flow;
        int cur = t;
        vector<int> path;

        while (cur != s) {
            Edge* prev = previousNode[cur];
           // AdjacenceyList.at(prev).at(cur);

            path.push_back(cur);


            //AdjacenceyList.
            previousNode[cur]->capacity -= new_flow;
            previousNode[cur]->Inverse->capacity += new_flow;
            cur = prev->Inverse->target;
           /* capacity[prev][cur] -= new_flow;
            capacity[cur][prev] += new_flow;
            cur = prev;*/
        }
        cout << s<<"->";
        for (int i = path.size() - 1; i >= 0; --i) {
            cout << path[i];
            if (i != 0) cout << "->";
        }cout << endl;

        new_flow = bfs(s, t, previousNode);
    }
    
    return flow;
}

int main() {
    InputFileName();

    auto start = chrono::high_resolution_clock::now();

    NumNodes = ReadNumberOfNodes(userinput);

    AdjacenceyList.resize(NumNodes);

    capacity.assign(NumNodes, vector<int>(NumNodes, 0));
    adj.assign(NumNodes, vector<int>());

    ifstream Inputfile("benchmarks/" + userinput);
    string line;
    getline(Inputfile, line); // Skip first line

    while (getline(Inputfile, line)) {
        if (line.empty())
            continue;

        istringstream ss(line);
        int From, To, Capacity;
        ss >> From >> To >> Capacity;

        if (From < 0 || To < 0 || Capacity <= 0 || From >= NumNodes || To >= NumNodes || From == To) {
            validGraph = false;
            cout << "\nInvalid edge detected. Graph construction cancelled.\n";
            break;
        }

        capacity[From][To] += Capacity;
        adj[From].push_back(To);
        adj[To].push_back(From); // Add reverse path for residual graph

        Edge* NewEdge = new Edge(Capacity, 0, To);
        Edge* ReverseEdge = new Edge(0, 0, From);

        NewEdge->Inverse = ReverseEdge;
        ReverseEdge->Inverse = NewEdge;

        AdjacenceyList.at(From).push_back(NewEdge);
        AdjacenceyList.at(To).push_back(ReverseEdge);

    }

    bool printGraphStats = false;

    if (printGraphStats) {

        /*for (vector<int> n : adj)
        {
            for (int i : n)
            {
                cout << " " << i;
            }
            cout << endl;
        }
        for (vector<int> n : capacity)
        {
            for (int i : n)
            {
                cout << " " << i;
            }
            cout << endl;
        }*/


        for (int i = 0; i < AdjacenceyList.size();i++)
        {
            cout << "node " << i << " has connections to ";
            for (Edge* e : AdjacenceyList.at(i))
            {
                cout << e->target <<" ";
            }
            cout << endl;
        }


    }
    if (validGraph) {
        cout << "\nCalculating max flow using Edmonds-Karp...\n";
        int maxFlow = edmondsKarp(0, EndNode);
        cout << "Max Flow: " << maxFlow << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedTime = end - start;
    cout << "Time Taken- " << elapsedTime.count() << "s" << endl;

    return 0;
}

int ReadNumberOfNodes(string FileName) {
    string line;
    ifstream inputfile("benchmarks/" + FileName);
    getline(inputfile, line);
    istringstream firstLine(line);
    int NumberOfNodes;
    firstLine >> NumberOfNodes;
    EndNode = NumberOfNodes - 1;
    return NumberOfNodes;
}

void InputFileName() {
    cin >> userinput;
    userinput = userinput + ".txt";
    ifstream s("benchmarks/" + userinput);
    if (!s) {
        cout << "\nFile can not be found.\n";
    }
}
