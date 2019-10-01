#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono; 


int X[4] = {-1, 1, 0, 0};
int Y[4] = {0, 0, -1, 1};
int monotonic = 1;

set<vector<vector<int>>> closedList1, closedList2, closedList3;

struct Node{
	Node *parent;
	int f, g, h;
	vector<vector<int>> configuration;
	Node(int g, int h): configuration(vector<vector<int>>(3, vector<int>(3))), f(g + h), g(g), h(h){}
};

stack <Node*> optimalPathH1, optimalPathH2, optimalPathH3;

int displacementCount(Node* current_state, Node* goal_state, int heuristic){
    int count = 0;
    int flag = 0;
    switch(heuristic){
        case 1 :
            count = 0;
            break;
        case 2 : 
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    if((current_state->configuration)[i][j] == 0) continue;
                    if((current_state->configuration)[i][j] != (goal_state->configuration)[i][j]) count++;
                }
            }
            break;
        case 3 :    
            flag = 0;
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    flag = 0;
                    int source = (current_state->configuration)[i][j];
                    if(source == 0) continue;
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            if((goal_state->configuration)[k][l] == source){
                                flag = 1;
                                count += abs(abs(k - i) + abs(l - j));
                                break;
                            }
                        }
                        if(flag == 1) break;
                    }
                }
            }
            break;
        case 4 :
            flag = 0;
            for(int i = 0; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    flag = 0;
                    int source = (current_state->configuration)[i][j];
                    if(source == 0) continue;
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            if((goal_state->configuration)[k][l] == source){
                                flag = 1;
                                count += pow(abs(k - i), 7) + pow(abs(l - j), 7);
                                break;
                            }
                        }
                        if(flag == 1) break;
                    }
                }
            }
            break;
        }
    return count;
}

bool isValid(int i, int j){
	if(i >= 0 && i < 3 && j >= 0 && j < 3)
		return true;

	return false;
}

void printState(Node *state){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if(state->configuration[i][j] == -48)
				state->configuration[i][j] = 0;
			cout<<state->configuration[i][j]<<"\t";
		}
		cout<<"\n";
	}
}

stack<Node*> printOptimalPath(Node *goal){
    stack<Node*> optimalPath;
    optimalPath.push(goal);
	Node *parent = goal->parent;
	while(parent != NULL){
        optimalPath.push(parent);
		parent = parent->parent;
	}

	return optimalPath;
}

pair<int, int> findBlankSpace(Node *state){
	int i = 0, j = 0;
	for(; i < 3; i++){
		for(j = 0; j < 3; j++){
			if(state->configuration[i][j] == 0)
				return {i, j};
		}
	}
    return {-1, -1};
}

void findChildren(Node *parent, 
				  pair<int, int> coordinatesOfBlankSpace, 
                  priority_queue<pair<int, Node*>, vector<pair<int, Node*>>, greater<pair<int, Node*>>> &q, 
                  Node* goal,
                  int heuristic){
	int x = coordinatesOfBlankSpace.first;
	int y = coordinatesOfBlankSpace.second;

	// cout<<"coordinates of blank space "<<x<<" "<<y<<"\n";
	for(int k = 0; k < 4; k++){
		int i = x+X[k];
		int j = y+Y[k];

		if(isValid(i, j)){
			// cout<<"child :"<<i<<", "<<j<<"\n";
			Node *child = new Node((parent->g)+1, 0);
			child->parent = parent;
			child->configuration = parent->configuration;
			swap((child->configuration[i][j]), (child->configuration[x][y]));
			// printState(child, "Child State");
            child->h = displacementCount(child, goal, heuristic);
            child->f = child->g + child->h;
			q.push({child->f, child});
            if(heuristic != 1 && heuristic != 4 && parent->h > child->h + 1){
                cout << "Heuristic :" << heuristic << "\n";
                monotonic = 0;
                cout << "Parent h-value : " << parent->h << "\t Child h-value : " << child->h << "\tCost parent->child : 1\n";            
            }
		}
	}
}

void printTable(int nodesExplored, int costOfOptimalPath, int executionTime, stack<Node*> optimalPath){
    cout << "TotalStatesExplored\tOptimalPathStates\tOptimalCost\tTotalExecutionTime (in microsecond)\n";
    cout <<"\t" << nodesExplored << "\t\t\t" << optimalPath.size() << "\t\t\t" <<  costOfOptimalPath << "\t\t\t" <<  executionTime << "\n";
    cout << "Optimal Path : \n" ;
    while(!optimalPath.empty()){
        printState(optimalPath.top());
        optimalPath.pop();
        cout << "\n";
    }
}

void printMonotonic(stack<Node*> optimalPath){
    Node* currentNode = optimalPath.top();
    optimalPath.pop();
    cout << "Parent h-value : " << currentNode->h << " ";
    while(!optimalPath.empty()){
        Node* child = optimalPath.top();
        optimalPath.pop();
        cout << "Child h-value : " << child->h << " cost parent->child : 1\n"; 
        if(!optimalPath.empty()) cout << "Parent h-value : " << child->h << " ";
    }
}

void aStarSearchHeuristicOne(Node *start, Node *goal){
    for(int heuristic = 1; heuristic <=4 ; heuristic++){
        monotonic = 1;
        start->h = displacementCount(start, goal, heuristic);
        start->f = start->g + start->h;
        auto start_time = high_resolution_clock::now(); 
        priority_queue<pair<int, Node*>, vector<pair<int, Node*>>, greater<pair<int, Node*>>> openList;
        set<vector<vector<int>>> closedList;
        stack<Node*> optimalPath;
        openList.push({start->h+start->g,start});
        int found = -1, optimalPathSize = 0;

        while(!openList.empty()){
            auto curr = openList.top();
            openList.pop();
            if(closedList.find(curr.second->configuration) != closedList.end())
                continue;
            //printState(curr.second);
            if(curr.second->configuration == goal->configuration){
                goal = curr.second;
                found = 0;
                break;
            }
            findChildren(curr.second, findBlankSpace(curr.second), openList, goal, heuristic);
            closedList.insert(curr.second->configuration);
        }
            auto stop_time = high_resolution_clock::now(); 

        if(found == -1){
            cout<<"Goal Node could not be reached"<<"\n";
            //printState(start);
            //printState(goal);
            cout<<"Total Number of nodes explored before termination : "<<closedList.size()<<"\n";
        } else{
            auto duration = duration_cast<microseconds>(stop_time - start_time); 
            cout<<"Goal Node reached \n";

            cout<<"Total Number of nodes explored before termination : "<<closedList.size()<<"\n";
            optimalPath =  printOptimalPath(goal);
            cout<<"Total number of nodes on optimal path : "<<optimalPath.size()<<"\n";
            printTable(closedList.size(), goal->f, duration.count(), optimalPath);
            if(monotonic) {
                cout << "Monotonically Restricted\n";
                printMonotonic(optimalPath);
            } else {
                cout << "Monotonic Restriction Not Followed ";
            }
        }
        switch(heuristic){
            case 1:
            closedList1 = closedList;
            optimalPathH1 = optimalPath;
            break;
            case 2:
            closedList2 = closedList;
            optimalPathH2 = optimalPath;
            break;
            case 3:
            closedList3 = closedList;
            optimalPathH3 = optimalPath;
            break;
        }
    }
}

void readFromFile(string fileName, Node *startNode, Node *goalNode){
	fstream file;
	string word;
	file.open(fileName.c_str());

	int i = 0, j = 0, start = -1, goal = -1;
	while(file >> word){
		if(word.compare("Start") == 0 || word.compare("state") == 0|| word.compare("Goal") == 0)
			continue;
		if(start == -1){
			startNode->configuration[i][j] = word[1]-'0';
			j++;
			if(i == 2 && j == 3){
				start = 1;
				i = 0;
				j = 0;
			}
			if(j == 3){
				j = 0;
				i++;
			}
		} else if(goal == -1){
			goalNode->configuration[i][j] = word[1]-'0';
			j++;
			if(i == 2 && j == 3){
				goal = 1;
			}
			if(j == 3){
				j = 0;
				i++;
			}
		}
    }
}

int main(){
	//reading the input from the file
	string word, filename;
	cout<<"Enter file name\n";
	cin >> filename;
	Node *startNode = new Node(0, 0);
	Node *goalNode = new Node(0, 0);
	readFromFile(filename, startNode, goalNode);
	startNode->parent = NULL;
    cout << "Start State \n";
	printState(startNode);
    cout << "\nGoal State \n\n";
	printState(goalNode);
	aStarSearchHeuristicOne(startNode, goalNode);
    bool h2SubsetOfh1 = includes(closedList1.begin(), closedList1.end(), closedList2.begin(), closedList2.end());
    if(h2SubsetOfh1){
        cout <<"Expansion : h1 > h2 (h2 is a subset of h1) : true" << endl;
    } else if(optimalPathH1.size() == optimalPathH2.size() && !h2SubsetOfh1){
        cout << "Multiple optimal paths exists, hence h1 > h2 : false\n";
    }
    bool h3SubsetOfh2 = includes(closedList2.begin(), closedList2.end(), closedList3.begin(), closedList3.end());
    if(h3SubsetOfh2){
        cout <<"Expansion : h2 > h3 (h3 is a subset of h2) : true" << endl;
    } else if(optimalPathH2.size() == optimalPathH3.size() && !h3SubsetOfh2){
        cout << "Multiple optimal paths exists, hence h2 > h3 : false\n";
    }
	return 0;
}
