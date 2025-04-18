#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

string userinput;
int NumNodes, EndNode;
bool validGraph =true ;
//vector<Node*> NodeList;

int ReadNumberOfNodes(string FileName);
void InputFileName();


class Edge 
{
public:

	int from, to, capacity;


	Edge(int f, int t, int c)
	{
		from = f; to = t; capacity = c;
	}
};



class Node
{
public:
	int  ID;
	//int capacity;
	//Node& adjacent;

	vector<Edge*> edges;



	Node(int id)
	{
		ID = id;
		//capacity = c;
		//adjacent = n;
	}
};


vector<Node*> NodeList;

int main()
{
	

	InputFileName();

	int NumNodes = ReadNumberOfNodes(userinput);

	for (int i = 0; i < NumNodes; i++)
	{
		NodeList.push_back(new Node(i));
	}



	ifstream Inputfile(userinput);
	string line;
	getline(Inputfile, line);   //THIS IS JUST TO SKIP THE FIRST LINE REMEMBER TO REMOVE

	while (getline(Inputfile, line))
	{
		if (line.empty())
			continue;

		istringstream ss(line);

		int From, To, Capacity;

		ss >> From >> To>> Capacity;

		//cout << "From " << From << " To " << To << " Capacity " << Capacity << '\n';

		if (From < 0 ||  To < 0 || Capacity <= 0)
		{
			validGraph = false;
			cout << "\n One or more elements had a negative value, graph construction cancelled \n";
			break;
		}
		else if (From > NumNodes - 1 || To > NumNodes - 1)
		{
			validGraph = false;
			cout << "\n An edge was defined between a node outside of the graph scope, graph construction cancelled \n";
			break;
		}
		else if (From == To)
		{
			validGraph = false;
			cout << "\n An edge was defined between itself, graph construction cancelled \n";
			break;
		}
		
		Edge* newEdge = new Edge(From, To, Capacity);
		NodeList[From]->edges.push_back(newEdge);
	}


	if (validGraph)
	{


		cout << "\nlist size " << NodeList.size();

		for (Node* n : NodeList)
		{
			cout << "Current Node ID: " << n->ID << endl;
			for (Edge* e : n->edges)
			{
				cout << " One Edge from " << e->from << " to " << e->to << " with capacity " << e->capacity << endl;
			}
		}
	}


	//string Text;

	//ifstream Files("Example.txt");
	//
	//while (getline (Files, Text))
	//{
	//	//cout << Text.substr(0,1) <<endl;
	//	Text.substr(1);
	//}
	////Files.close();
	////Files.open("Example.txt",std::ios_base::in);

	//cout << ReadNumberOfNodes("Example.txt");

}


int ReadNumberOfNodes(string FileName)
{
	//string FileName = FN + ".txt";

	string line;

	ifstream inputfile(FileName);

	getline(inputfile, line);
	istringstream firstLine(line);
	int NumberOfNodes;
	firstLine >> NumberOfNodes;

	EndNode = NumberOfNodes - 1;
	return NumberOfNodes;
}

string ReadConnections(string FileName)
{
	return "s";
}


void InputFileName()
{
	cin >> userinput;
	userinput = userinput + ".txt";

	ifstream s(userinput);

	if (!s)
	{
		cout << "\nFile can not be found. \n";
	}
}

