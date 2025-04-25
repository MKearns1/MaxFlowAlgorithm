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

string InputFileName() {
    cin >> userinput;
    userinput = userinput + ".txt";
    ifstream s("benchmarks/" + userinput);
    if (!s) {
        cout << "\nFile can not be found.\n";
        return "";
    }
    return userinput;
}

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

int CreateGraph(string FileName, vector<vector<Edge*>>& AdjacencyList);

int bfs(int s, int t, vector<Edge*>& EdgeTo, vector<vector<Edge*>>& AdjacencyList, bool PrintStats) {

    vector<bool> visited (EdgeTo.size(), false);

    visited[s] = true;

    fill(EdgeTo.begin(), EdgeTo.end(), nullptr);

    queue<pair<int, int>> q;
    q.push({ s, INT_MAX });

    while (!q.empty()) {

        int currentNode = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int i = 0; i < AdjacencyList.at(currentNode).size(); i++) //Check every Edge connectected to the current node
        {
            int TargetNode = AdjacencyList.at(currentNode).at(i)->target;
            int ResidualEdgeCapacity = AdjacencyList.at(currentNode).at(i)->capacity - AdjacencyList.at(currentNode).at(i)->flow;
            

            if(!visited[TargetNode])    //Check that the Edge's target node hasn't been visited and has some capacity left
            {
                if (ResidualEdgeCapacity > 0)
                {
                    visited[TargetNode] = true;

                    EdgeTo.at(TargetNode) = AdjacencyList.at(currentNode).at(i); // we got to the current target node using this edge on the adjacencey list

                    int BottleNeck = (flow < ResidualEdgeCapacity) ? flow : ResidualEdgeCapacity;   //Set the bottleneck of this path to the smallest value
                 
                    if (AdjacencyList.at(currentNode).at(i)->target == t)  //if one of the edges leads to the target node return the max amount of flow that was possible on this path
                    {

                        return BottleNeck;
                    }
                    else { q.push({ TargetNode, BottleNeck }); } //if still not reached target node add more destination nodes
                }               
            }
        }
    }
    return 0;       //No flow can be passed because theres no more remaining paths to take so return 0
}

int MaxFlow(int s, int t, vector<vector<Edge*>>& AdjacencyList, bool PrintStats) {

    int flow = 0;
    vector<Edge*> EdgeTo(t+1);     //stores the edge taken to get to each node (so EdgeTo[s] would be null)
    int new_flow = bfs(s, t, EdgeTo, AdjacencyList, PrintStats);
    int i = 0;
    while (new_flow != 0) // as long as the bfs function can find a new path
    {
        flow += new_flow;
        int CurrentTarget = t;
        vector<int> path;
        
        if (PrintStats) {
            cout << "Iteration: " << i << endl;
            cout << "bottleneck was " << new_flow << endl;

            cout << "Current Max Flow is " << flow << endl;
        }
        while (CurrentTarget != s)  //Iterates through each edge in the path from the target to the source
        {
            Edge* prev = EdgeTo[CurrentTarget];

            path.push_back(CurrentTarget);

           EdgeTo[CurrentTarget]->flow += new_flow;             //Increase the flow of each edge by the bottleneck found by bfs
           EdgeTo[CurrentTarget]->Inverse->flow -= new_flow;    //Reduce the flow of the reverse edge by the same amount

            CurrentTarget = prev->Inverse->target;                  //Set the current target to the previous node.

        }
        if (PrintStats) {
            cout << s << "->";
            for (int i = path.size() - 1; i >= 0; --i) {
                cout << path[i];
                if (i != 0) cout << "->";
            }cout<<'\n' << endl;
        }
        new_flow = bfs(s, t, EdgeTo, AdjacencyList, PrintStats);
        i++;
    }
    
    return flow;
}

int main() {

    cout << "Please Enter the name of the file you would like to read (without the .txt)" << endl;
    bool printGraphStats = false;

    string userinput;
    userinput = InputFileName();

    while (userinput == "")
    {
        userinput = InputFileName();
    }

    auto gstart = chrono::high_resolution_clock::now();   

    vector<vector<Edge*>> AdjacencyList;

    int NumberOfNodes = CreateGraph(userinput, AdjacencyList);
    int TargetNode = NumberOfNodes - 1;
    auto gend = chrono::high_resolution_clock::now();

    auto start = chrono::high_resolution_clock::now();

    if (NumberOfNodes != 0) {
        cout << "\nCalculating max flow using Edmonds-Karp...\n";
        int maxFlow = MaxFlow(0, TargetNode, AdjacencyList,printGraphStats);
        cout << "Max Flow: " << maxFlow << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> CreationTime = gend - gstart;
    chrono::duration<double> elapsedTime = end - start;
    cout << "Time Taken to create graph- " << CreationTime.count() << "s" << endl;
    cout << "Time Taken to find max flow- " << elapsedTime.count() << "s" << endl;


    if (printGraphStats) {

        cout << endl;
        for (int i = 0; i < AdjacencyList.size(); i++)
        {
            cout << "node " << i << " has connections to ";
            for (Edge* e : AdjacencyList.at(i))
            {
                cout << e->target << " ";
            }
            cout << endl;
        }
    }

    return 0;
}

int CreateGraph(string FileName, vector<vector<Edge*>>& AdjacencyList) {

    int NumberOfNodes;

    string line;
    ifstream inputfile("benchmarks/" + FileName);
    getline(inputfile, line);
    istringstream firstLine(line);
    firstLine >> NumberOfNodes;

    AdjacencyList.resize(NumberOfNodes);

    while (getline(inputfile, line)) {
        if (line.empty())
            continue;

        istringstream ss(line);
        int From, To, Capacity;
        ss >> From >> To >> Capacity;

        if (From < 0 || To < 0 || Capacity <= 0 || From >= NumberOfNodes || To >= NumberOfNodes || From == To) //Checking that every line has no impossible values e.g an edge between a node and itself
        {
            cout << "\nInvalid edge detected. Graph construction cancelled.\n";
            return 0;
        }

        Edge* NewEdge = new Edge(Capacity, 0, To);
        Edge* ReverseEdge = new Edge(0, 0, From);

        NewEdge->Inverse = ReverseEdge;
        ReverseEdge->Inverse = NewEdge;

        AdjacencyList.at(From).push_back(NewEdge);
        AdjacencyList.at(To).push_back(ReverseEdge);

    }
    return NumberOfNodes;
}

