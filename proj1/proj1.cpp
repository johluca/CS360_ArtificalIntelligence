#include <queue>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include "problem.h"

#include <time.h>


using namespace std;
/// you can add whatever helper functions/variables you need here.

struct Node
{
public:
	pair<int, int> point;
	int f;
	int g;

	vector<pair<int, int> > unvisitedTargs;

	vector<pair<int, int> > cameFromVec;

	Node(pair<int, int> pt, int fi, int gi)
	{
		point = pt;
		f = fi;
		g = gi;
	}

	Node(pair<int, int> pt, int fi, int gi, vector<pair<int,int> > uv, vector<pair<int, int> > cf)
	{
		point = pt;
		f = fi;
		g = gi;
		unvisitedTargs = uv;
		cameFromVec = cf;
	}

	bool operator==(const Node& n)
	{
		if (point != n.point)
		{
			return false;
		}
		if (unvisitedTargs.size() != n.unvisitedTargs.size())
		{
			return false;
		}

		for (int i = 0; i < unvisitedTargs.size(); i++)
		{
			bool match = false;
			for (int j = 0; j < n.unvisitedTargs.size(); j++)
			{
				if (unvisitedTargs[i] == n.unvisitedTargs[j])
					match = true;
			}
			if (!match)
				return false;
		}

		return true;

	}
};

struct CompareNode {
	bool operator()(Node  p1, Node  p2) {
		
		return p1.f > p2.f;
	}
};

int getManhattanDist(pair<int, int> s, pair<int, int> g)
{
	int xDist = g.first - s.first;
	if (xDist < 0)
		xDist *= -1;
	int yDist = g.second - s.second;
	if (yDist < 0)
		yDist *= -1;

	int sum = xDist + yDist;
	return sum;
}

Node* getNodeWithMinF(vector<Node*> v)
{
	int min = v[0]->f;
	Node* node = v[0];
	
	for (int i = 0; i < v.size(); i++)
	{
		//std::cout << "PROBLEM: " << i << endl;
		if (v[i]->f < min)
		{
			node = v[i];
		}
	}

	return node;
}

vector<Node> removeNodeFrom(vector<Node> v, Node n)
{
	
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == n)
		{
			v.erase(v.begin() + i);
			return v;
		}
	}

}

int getNodeGFromPoint(vector<Node*> v, pair<int, int> n)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i]->point == n)
		{
			return v[i]->g;
		}
	}
	
	return -1;//good?
}

int getGFromVec(vector<Node*> v, Node n)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (*v[i] == n)
		{
			return v[i]->g;
		}
	}

	return -1;//good?
}

bool vecContainsPoint(vector<Node* > v, pair<int, int> p)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i]->point == p)
		{
			return true;
		}
	}
	return false;
}

bool vecContainsNode(vector<Node* > v, Node n)//uses == op for q3. compares unvisited list and point
{
	for (int i = 0; i < v.size(); i++)
	{
		if (*v[i] == n)//deref pointer
		{
			return true;
		}
	}
	return false;
}

bool vecContains(vector<Node* > v, Node n)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i]->point == n.point)
		{
			return true;
		}
	}
	return false;
}

vector<string> myGetPath(map<pair<int, int>, pair<int, int> > &parent, pair<int, int> goal)
{
	vector<string> path;
	pair<int, int> state = goal;
	int dx, dy;

	//cout << "parent: " << state.second << " " << state.first << endl;
	while (parent[state] != state)
	{

		dx = state.first - parent[state].first;
		dy = state.second - parent[state].second;
		if (dx>0)
			path.push_back("South");
		else if (dx<0)
			path.push_back("North");
		else if (dy>0)
			path.push_back("East");
		else
			path.push_back("West");
		state = parent[state];
	}
	reverse(path.begin(), path.end());
	//cout << "SIZE OF PATH: " << path.size() << endl;
	return path;
}



vector<string> getPathVec(vector<pair<int, int> > &parent, pair<int, int> goal)
{
	vector<string> path;
	pair<int, int> state = goal;
	int dx, dy;

	for (int i = 0; i < parent.size(); i++) {
		dx = parent[i].first - state.first;
		dy = parent[i].second - state.second;
		if (dx>0)
			path.push_back("South");
		else if (dx<0)
			path.push_back("North");
		else if (dy>0)
			path.push_back("East");
		else
			path.push_back("West");
		state = parent[i];

	}
	//reverse(path.begin(), path.end());
	return path;
}

//get manhattan distance from current to farthest unvisited target
int farthestTargetHeur(pair<int, int> p, vector<pair<int, int> > uv)
{
	int maxDist = 0;
	for (int i = 0; i < uv.size(); i++)
	{
		int currDist = getManhattanDist(p, uv[i]);
		if (currDist > maxDist)
		{
			maxDist = currDist;
		}
	}
	//cout << "HEUR: " << maxDist << endl;;
	return maxDist;
}

//check if we have found allll nodes
bool areWeDone(Node &current)
{
	//if current is an unvisited goal, remove from unv goals list
	for (int i = 0; i < current.unvisitedTargs.size(); i++)
	{
		if (current.point == current.unvisitedTargs[i])
		{
			//cout << "visited goal: " << current.point.second << " " << current.point.first << endl;
			//cout << "visited a goal" << endl;
			current.unvisitedTargs.erase(current.unvisitedTargs.begin() + i);
			if (current.unvisitedTargs.size() == 0)
			{
				return true;
			}
			else if (current.unvisitedTargs.size() < 0)
			{
				cout << "THIS IS WEIRD AND WRONG" << endl;
			}
		}
	}
	return false;
}


vector<string> questionOne(Problem &problem)
{
	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node > closedVec;
	vector<Node > openVec;
	priority_queue<Node, vector<Node>, CompareNode> openList;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	//f_score[start] = g_score[start] + nearestNeghborHeur(start, goals);
	int startF = g_score[start] + farthestTargetHeur(start, goals);


	openVec.push_back(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	openList.push(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	//numgoals left + manDist to closest

	vector<string> goalToPath;
	vector<string> actualPath;



	while (!openList.empty())
	{
		//Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f
		Node current = openList.top();
		openList.pop();
		/* if current Point is a goal, remove it, then, if goals is empty, return*/
		/*/
		for (int i = 0; i < goals.size(); i++)
		{
		if (current->point == goals[i])
		{
		//got goal
		cout << "FOUND GOAL: " << goals[i].second << " " << goals[i].first << endl;

		goalToPath = myGetPath(cameFrom, goals[i]);
		actualPath.insert(actualPath.end(), goalToPath.begin(), goalToPath.end());
		cameFrom.clear();
		goals.erase(goals.begin() + i);

		if (goals.size() == 0)
		{
		cout << "SOLUTION" << endl;
		return actualPath;

		//return vector<string> path
		}
		}

		}
		*/
		/*
		if (current->point == goals[0])
		{
		return myGetPath(cameFrom, goals[0]);
		}
		*/
		openVec = removeNodeFrom(openVec, current);
		if (openVec.size() != openList.size())
		{
			cout << "baderror" << endl;
		}
		//cout << "OVsize: " << openVec.size() << " " << "OLsize: " << openList.size() << endl;

		//check if at goal state (all visited) also, update current's unvisited vector
		if (areWeDone(current))
		{
			cout << "WE ARE DONE " << current.cameFromVec.size() << endl;
			return getPathVec(current.cameFromVec, current.point);
			//return vector<string>();
		}

		closedVec.push_back(current);


		vector<pair<int, int> > neighbors = problem.getSuccessors(current.point);
		for (int i = 0; i < neighbors.size(); i++)
		{

			int tempG = current.g + 1;
			int tempF = farthestTargetHeur(neighbors[i], current.unvisitedTargs) + tempG;
			Node tempNode(neighbors[i], tempF, tempG, current.unvisitedTargs, current.cameFromVec);

			vector<pair<int, int> > tempCameFrom = current.cameFromVec;
			tempCameFrom.push_back(tempNode.point);

			tempNode.cameFromVec = tempCameFrom;

			int neighborG = -1; //only used if not in openvec



			//Node* cNode = *closedIter;
			//Node* oNode = *openIter;




			vector <Node>::iterator openIter = find(openVec.begin(), openVec.end(), tempNode);
			vector <Node>::iterator closedIter = find(closedVec.begin(), closedVec.end(), tempNode);
			cout << "tg: " << tempG << " cG: " << closedIter->g << endl;
			if (closedIter != closedVec.end())
			{
				cout << "firstclosed" << endl;
				if (tempG < closedIter->g)
				{
					cout << "closeG" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}

			else if (openIter != openVec.end())
			{
				cout << "firstop" << endl;
				if (tempG < openIter->g)
				{
					cout << "opg" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}
			else
			{
				cout << "else" << endl;

				openList.push(tempNode);
				openVec.push_back(tempNode);
			}

			////
			/*
			neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG



			if (vecContainsNode(closedVec, tempNode) && tempG < neighborG )//change this to op== overloaded
			{
			//update neighbor with lower g val
			//cout << "MEG: " << tempG << endl;
			setNodeGVal(closedVec, tempNode, tempG);

			}
			else if (vecContainsNode(openVec, tempNode) && tempG < neighborG)
			{
			//update neighbor with lower g val
			//cout << "MEG: " << tempG << endl;
			setNodeGVal(openVec, tempNode, tempG);
			}
			else
			{
			//add neighbor to open list with g val curr+1
			int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
			int nG = tempG;
			openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			}
			*/
			//rewrite ends here
			/*
			if (vecContainsNode(closedVec, tempNode))//change this to op== overloaded
			{
			continue;
			}

			int neighborG = -1; //only used if not in openvec
			if (vecContainsNode(openVec, tempNode))//if im in open, use my g to compare below
			{
			neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG

			}
			int tempG = current->g + 1;
			//int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
			if ((!vecContainsNode(openVec, tempNode)) || tempG < neighborG)
			{

			cameFrom[neighbors[i]] = current->point;
			int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
			int nG = tempG;
			if (!vecContainsPoint(openVec, neighbors[i]))
			{
			openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			}
			}*/
		}
	}
	cout << "FAILURE" << endl;
	return vector<string>();
	/*
	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node* > closedVec;
	vector<Node* > openVec;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	f_score[start] = g_score[start] + getManhattanDist(start, goals[0]);
	int startF = g_score[start] + getManhattanDist(start, goals[0]);
	openVec.push_back(new Node(start, startF, 0));

	std::cout << "Startf: " << startF << endl;

	while (!openVec.empty())
	{
	//std::cout << "OPENSIZE: "<< openVec.size() << endl;
	Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f


	if (current->point == goals[0])
	{
	return myGetPath(cameFrom, goals[0]);
	//return the path
	}

	openVec = removeNodeFrom(openVec, current);
	//std::cout << "OPENSIZE: " << openVec.size() << endl;
	closedVec.push_back(current);

	//std::cout << "CURR: X " << current->point.first << " Y " << current->point.second << endl;
	//look at neighbors
	vector<pair<int, int> > neighbors = problem.getSuccessors(current->point);
	for (int i = 0; i < neighbors.size(); i++)
	{
	//	std::cout << "sz " << neighbors.size() << endl;

	if (vecContainsPoint(closedVec, neighbors[i]))
	{
	continue;
	//std::cout << "TEST :" << i <<endl;
	}


	int tempG = current->g + 1;//super wrong

	int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
	//std::cout << "G: " << neighborG << " "<< tempG << endl;
	//if neighbor[i] is in openset, check if it/s g is less than tempG
	//if neighbor not in open or tempG < neighbor.g
	if ((!vecContainsPoint(openVec, neighbors[i])) || tempG < neighborG) //OR tempG < neighborG
	{

	//std::cout << "G: " << tempG << " "<< neighborG << endl;
	cameFrom[neighbors[i]] = current->point;
	int nF = tempG + getManhattanDist(neighbors[i], goals[0]);
	int nG = tempG;
	//sstd::cout << "f: "<< nF << endl;
	if (!vecContainsPoint(openVec, neighbors[i]))
	{
	//std::cout << "TEST" << endl;
	openVec.push_back(new Node(neighbors[i], nF, nG));
	}
	}
	}
	}
	return vector<string>();
	}

	/****************/
}
/*Q2 STUFF BELOW*/
/****************/
//modified manhattan heuristic to pick goals, then normal man is used to go to goal
/*int wallManhattanHeur(pair<int, int> s, pair<int, int> g, Problem &problem)
{
	int heur = 0;
	int finalFirst = 0;
	pair<int, int>* tempP;

	cout << "start: " << s.second << " " << s.first << endl;
	cout << "finish: " << g.second << " " << g.first << endl;

	if (g.first < s.first)//goal y is less than current y
	{
		//cout << "GOAL Y SMALLER: " << g.first <<" "<< s.first << endl;
		for (int i = s.first; i > g.first; i--)
		{
			
			tempP = new pair<int, int>(i, s.second); 
			//cout << "POINT: Y " << tempP->first << " X " << tempP->second << endl;
			if (problem.isWall(*tempP))//if this is a wall, increment heur value
			{
				//cout << " ISWALL " << endl;
				heur++;
			}
			finalFirst = i;
			
		}
		 //assume that start is less than finish
	}
	else//goal y is greater than current y
	{
		//cout << "GOAL Y GREATER" << g.first << " " << s.first << endl;
		for (int i = s.first; i <= g.first; i++)
		{
			tempP = new pair<int, int>(i, s.second);
			//cout << "POINT1B: Y " << tempP->first << " X " << tempP->second << endl;
			if (problem.isWall(*tempP))//if this is a wall, increment heur value
			{
				//cout << " ISWALL " << endl;
				heur++;
			}
			finalFirst = i;
		}
		
	}
	// if goal x is less than moved point's x
	if (g.second < tempP->second)
	{
		
		for (int j = tempP->second; j > g.second; j--)
		{
			
			pair<int, int> p(tempP->first, j);
			//cout << "POINT2: Y " << p.first << " X " << p.second << endl;
			
			if (problem.isWall(p))//if this is a wall, increment heur value
			{
				//cout << " ISWALL " << endl;
				heur++;
			}
		}
	}
	else//if goal x is greater than moved point's x
	{
		for (int j = tempP->second; j < g.second; j++)
		{
			
			pair<int, int> p(tempP->first, j);
			//cout << "POINT2B: Y " << p.first << " X " << p.second << endl;
			if (problem.isWall(p))//if this is a wall, increment heur value
			{
				//cout << " ISWALL " << endl;
				heur++;
			}
		}
	}

	cout << "HEUR: " << heur << endl;
	return heur;
}
*/

vector<string> questionTwo(Problem &problem)
{

	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node > closedVec;
	vector<Node > openVec;
	priority_queue<Node, vector<Node>, CompareNode> openList;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	//f_score[start] = g_score[start] + nearestNeghborHeur(start, goals);
	int startF = g_score[start] + farthestTargetHeur(start, goals);


	openVec.push_back(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	openList.push(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	//numgoals left + manDist to closest

	vector<string> goalToPath;
	vector<string> actualPath;



	while (!openList.empty())
	{
		//Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f
		Node current = openList.top();
		openList.pop();
		/* if current Point is a goal, remove it, then, if goals is empty, return*/
		/*/
		for (int i = 0; i < goals.size(); i++)
		{
		if (current->point == goals[i])
		{
		//got goal
		cout << "FOUND GOAL: " << goals[i].second << " " << goals[i].first << endl;

		goalToPath = myGetPath(cameFrom, goals[i]);
		actualPath.insert(actualPath.end(), goalToPath.begin(), goalToPath.end());
		cameFrom.clear();
		goals.erase(goals.begin() + i);

		if (goals.size() == 0)
		{
		cout << "SOLUTION" << endl;
		return actualPath;

		//return vector<string> path
		}
		}

		}
		*/
		/*
		if (current->point == goals[0])
		{
		return myGetPath(cameFrom, goals[0]);
		}
		*/
		openVec = removeNodeFrom(openVec, current);
		if (openVec.size() != openList.size())
		{
			cout << "baderror" << endl;
		}
		//cout << "OVsize: " << openVec.size() << " " << "OLsize: " << openList.size() << endl;

		//check if at goal state (all visited) also, update current's unvisited vector
		if (areWeDone(current))
		{
			cout << "WE ARE DONE " << current.cameFromVec.size() << endl;
			return getPathVec(current.cameFromVec, current.point);
			//return vector<string>();
		}

		closedVec.push_back(current);


		vector<pair<int, int> > neighbors = problem.getSuccessors(current.point);
		for (int i = 0; i < neighbors.size(); i++)
		{

			int tempG = current.g + 1;
			int tempF = farthestTargetHeur(neighbors[i], current.unvisitedTargs) + tempG;
			Node tempNode(neighbors[i], tempF, tempG, current.unvisitedTargs, current.cameFromVec);

			vector<pair<int, int> > tempCameFrom = current.cameFromVec;
			tempCameFrom.push_back(tempNode.point);

			tempNode.cameFromVec = tempCameFrom;

			int neighborG = -1; //only used if not in openvec



			//Node* cNode = *closedIter;
			//Node* oNode = *openIter;




			vector <Node>::iterator openIter = find(openVec.begin(), openVec.end(), tempNode);
			vector <Node>::iterator closedIter = find(closedVec.begin(), closedVec.end(), tempNode);
			//cout << "tg: " << tempG << " cG: " << closedIter->g << endl;
			if (closedIter != closedVec.end())
			{
				//cout << "firstclosed" << endl;
				if (tempG < closedIter->g)
				{
					//cout << "closeG" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}

			else if (openIter != openVec.end())
			{
				//cout << "firstop" << endl;
				if (tempG < openIter->g)
				{
					//cout << "opg" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}
			else
			{
				//cout << "else" << endl;

				openList.push(tempNode);
				openVec.push_back(tempNode);
			}

			////
			/*
			neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG



			if (vecContainsNode(closedVec, tempNode) && tempG < neighborG )//change this to op== overloaded
			{
			//update neighbor with lower g val
			//cout << "MEG: " << tempG << endl;
			setNodeGVal(closedVec, tempNode, tempG);

			}
			else if (vecContainsNode(openVec, tempNode) && tempG < neighborG)
			{
			//update neighbor with lower g val
			//cout << "MEG: " << tempG << endl;
			setNodeGVal(openVec, tempNode, tempG);
			}
			else
			{
			//add neighbor to open list with g val curr+1
			int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
			int nG = tempG;
			openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			}
			*/
			//rewrite ends here
			/*
			if (vecContainsNode(closedVec, tempNode))//change this to op== overloaded
			{
			continue;
			}

			int neighborG = -1; //only used if not in openvec
			if (vecContainsNode(openVec, tempNode))//if im in open, use my g to compare below
			{
			neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG

			}
			int tempG = current->g + 1;
			//int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
			if ((!vecContainsNode(openVec, tempNode)) || tempG < neighborG)
			{

			cameFrom[neighbors[i]] = current->point;
			int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
			int nG = tempG;
			if (!vecContainsPoint(openVec, neighbors[i]))
			{
			openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			}
			}*/
		}
	}
	cout << "FAILURE" << endl;
	return vector<string>();
	/*
	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	int numGoals = goals.size();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node* > closedVec;
	vector<Node* > openVec;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	f_score[start] = g_score[start] + getManhattanDist(start, goals[0]);
	int startF = g_score[start] + getManhattanDist(start, goals[0]);

	map<int, vector<string> > goalToPath;

	vector<string> actualPath;
	//openVec.push_back(new Node(start, startF, 0));

	std::cout << "Startf: " << startF << endl;
	for (int i = 0; i < numGoals; i++)//go through all targets
	{
		cout <<"current for goal: "<<  goals[i].second << goals[i].first << endl;
		openVec.clear();
		startF = actualPath.size() +getManhattanDist(start, goals[i]);
		openVec.push_back(new Node(start, startF, actualPath.size()));
		while (!openVec.empty())
		{
			//std::cout << "OPENSIZE: "<< openVec.size() << endl;
			Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f

			//if cameFrom size is larger than the smallest, you can stop (something)

			if (current->point == goals[i])
			{
				goalToPath[i] = myGetPath(cameFrom, goals[i]);//onlys keep the smallest
				
				cout<< "DIST TO goal*: " << i << " "<< goalToPath[i].size()<< endl;
				cameFrom.clear();
				closedVec.clear();

				
				if (i == goals.size() - 1)
				{
					int minPath = 999999999;
					int minIndex;
					for (int j = 0; j < goalToPath.size(); j++)
					{
						if (goalToPath[j].size() < minPath)
						{
							minPath = goalToPath[j].size();
							minIndex = j;
		
						}
					}
					actualPath.insert(actualPath.end(), goalToPath[minIndex].begin(), goalToPath[minIndex].end());
					goalToPath.clear();
					//no more looking at this particular target
					cout << "We got: " << goals[minIndex].second << goals[minIndex].first << endl;
					//start from this goal
					start = goals[minIndex];
					goals.erase(goals.begin() + minIndex);
					cout << "newgoalsize: " << goals.size();

					i = -1;
					if (goals.empty())
					{
						cout << "SUCCESS: " << actualPath.size() <<endl;
						return actualPath;
					}
					break;
				}

				break;
				//return the path
			}

			openVec = removeNodeFrom(openVec, current);
			//std::cout << "OPENSIZE: " << openVec.size() << endl;
			closedVec.push_back(current);

			//std::cout << "CURR: X " << current->point.first << " Y " << current->point.second << endl;
			//look at neighbors
			vector<pair<int, int> > neighbors = problem.getSuccessors(current->point);
			for (int i = 0; i < neighbors.size(); i++)
			{
				//	std::cout << "sz " << neighbors.size() << endl;

				if (vecContainsPoint(closedVec, neighbors[i]))
				{
					continue;
					//std::cout << "TEST :" << i <<endl;
				}


				int tempG = current->g + 1;//super wrong

				int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
				//std::cout << "G: " << neighborG << " "<< tempG << endl;
				//if neighbor[i] is in openset, check if it/s g is less than tempG
				//if neighbor not in open or tempG < neighbor.g
				if ((!vecContainsPoint(openVec, neighbors[i])) || tempG < neighborG) //OR tempG < neighborG
				{

					//std::cout << "G: " << tempG << " "<< neighborG << endl;
					cameFrom[neighbors[i]] = current->point;
					int nF = tempG + getManhattanDist(neighbors[i], goals[0]);
					int nG = tempG;
					//sstd::cout << "f: "<< nF << endl;
					if (!vecContainsPoint(openVec, neighbors[i]))
					{
						//std::cout << "TEST" << endl;
						openVec.push_back(new Node(neighbors[i], nF, nG));
					}
				}
			}
		}
	}
	cout << "FAILURE" << endl;
	return vector<string>();


	/*
	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals(); //4 corners

	vector<Node* > closedVec;
	vector<Node* > openVec;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	f_score[start] = g_score[start] + getManhattanDist(start, goals[0]);
	int startF = g_score[start] + getManhattanDist(start, goals[0]);
	openVec.push_back(new Node(start, startF, 0));

	std::cout << "Startf: " << startF << endl;

	//do initial goal pick
	int minHeur = 99999;
	int currentGoalIdx = 0;
	int goalsFoundCount = 0; //terminate when this equals 4
	//vector <pair<int, int> > goalsFound; //keep track of which goals already acquired
	bool pickNewGoal = true;

	vector<string> path;


	while (!openVec.empty())
	{

		
		//std::cout << "OPENSIZE: "<< openVec.size() << endl;
		Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f

		//cout << "CURRENT :" << current->point.second << " " << current->point.first << endl;

		if (current->point == goals[currentGoalIdx])
		{

			cout << "Found Goal: " << goals[currentGoalIdx].second << " " << goals[currentGoalIdx].first << endl;
			//append vector to current path vector
			
		
			vector<string> toBeAdded = 	myGetPath(cameFrom, goals[currentGoalIdx]);
			cameFrom.clear();

			//path.insert(path.end(), toBeAdded.begin(), toBeAdded.end());
			goalsFoundCount++;
			goals.erase(goals.begin() + currentGoalIdx);//get out of goal list when I find you!
			
			
			//goalsFound.push_back(current->point);
			if (goalsFoundCount == 4)
			{
				cout << "** ALL GOALS FOUND ** " << endl;
				return toBeAdded;//hmmm
			}
			
			pickNewGoal = true;
		}
		//USE HEURISTIC TO PICK NEW GOAL
		if (pickNewGoal)
		{

			//clear closed set
			closedVec.clear();
			minHeur = 99999;
			for (int j = 0; j < goals.size(); j++)
			{
				//cout << "J: " << j << endl;
				//cout << "Goal: " << goals[j].second << " " << goals[j].first << endl;
				int heur = wallManhattanHeur(current->point, goals[j], problem);
				if (heur < minHeur)
				{
					minHeur = heur;
					currentGoalIdx = j;
					//cout << "Current Goal: " << goals[j].second << " " << goals[j].first << "  heur: " << heur <<  endl;
					pickNewGoal = false;
					//break;
				}
			}
		}


		openVec = removeNodeFrom(openVec, current);
		//std::cout << "OPENSIZE: " << openVec.size() << endl;
		closedVec.push_back(current);

		//std::cout << "CURR: X " << current->point.first << " Y " << current->point.second << endl;
		//look at neighbors
		vector<pair<int, int> > neighbors = problem.getSuccessors(current->point);
		for (int i = 0; i < neighbors.size(); i++)
		{
			//	std::cout << "sz " << neighbors.size() << endl;

			if (vecContainsPoint(closedVec, neighbors[i]))
			{
				continue;
				//std::cout << "TEST :" << i <<endl;
			}


			int tempG = current->g + 1;//super wrong

			int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
			//std::cout << "G: " << neighborG << " "<< tempG << endl;
			//if neighbor[i] is in openset, check if it/s g is less than tempG
			//if neighbor not in open or tempG < neighbor.g
			if ((!vecContainsPoint(openVec, neighbors[i])) || tempG < neighborG) //OR tempG < neighborG
			{

				//std::cout << "G: " << tempG << " "<< neighborG << endl;
				cameFrom[neighbors[i]] = current->point;//TODO not appending camefrom right
				
			//	cout << "Map size: " << cameFrom.size() << endl;
				
				int nF = tempG + getManhattanDist(neighbors[i], goals[0]);
				int nG = tempG;
				//sstd::cout << "f: "<< nF << endl;
				if (!vecContainsPoint(openVec, neighbors[i]))
				{
					//std::cout << "TEST" << endl;
					openVec.push_back(new Node(neighbors[i], nF, nG));
				}
			}
		}
	}
	cout << "FAILURE" << endl;
	return vector<string>();
	*/
	
}
int multGoalHeuristic(pair<int, int> c, vector<pair<int, int> > goals)
{
	int min = getManhattanDist(c, goals[0]);
	for (int i = 0; i < goals.size(); i++)
	{
		if (getManhattanDist(c, goals[i]) < min)
		{
			min = getManhattanDist(c, goals[i]);
		}
	}
	return  min + goals.size();
}

int nearestNeghborHeur(pair<int, int> c, vector<pair<int, int> > goals)
{
	vector<pair<int, int> > unVisited = goals;
	int min = getManhattanDist(c, goals[0]);
	int minIndex = 0;
	//start with full list, find one closest to point c, add dist to dist, repeat
	pair<int, int> current = c;
	//int min = 99999;
	int total = 0;

	while (!unVisited.empty())
	{
		for (int i = 0; i < unVisited.size(); i++)
		{
			if (getManhattanDist(current, goals[i]))
			{
				min = getManhattanDist(current, goals[i]);
				minIndex = i;
			}
		}
		total += min;
		current = unVisited[minIndex];
		unVisited.erase(unVisited.begin() + minIndex);
	}
	
	return total;
	//have min dist to goal, and what goal it is
}


Node* setNodeGVal(vector<Node* > &vec, Node n, int nG)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (*vec[i] == n)
		{
			vec[i]->g = nG;
			return vec[i];
		}
	}
}

vector<string> questionThree(Problem &problem)
{

	

	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node > closedVec;
	vector<Node > openVec;
	priority_queue<Node, vector<Node>, CompareNode> openList;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	//f_score[start] = g_score[start] + nearestNeghborHeur(start, goals);
	int startF = g_score[start] + farthestTargetHeur(start, goals);


	openVec.push_back( Node(start, startF, 0, goals, vector<pair<int,int> >()));
	openList.push( Node(start, startF, 0, goals, vector<pair<int,int> >()));
	//numgoals left + manDist to closest

	vector<string> goalToPath;
	vector<string> actualPath;



	while (!openList.empty())
	{
		//Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f
		Node current = openList.top();
		openList.pop();
		/* if current Point is a goal, remove it, then, if goals is empty, return*/

		openVec = removeNodeFrom(openVec, current);
		if (openVec.size() != openList.size())
		{
			cout << "baderror" << endl;
		}
		//cout << "OVsize: " << openVec.size() << " " << "OLsize: " << openList.size() << endl;

		//check if at goal state (all visited) also, update current's unvisited vector
		if (areWeDone(current))
		{
			cout << "WE ARE DONE " << current.cameFromVec.size() << endl;
			return getPathVec(current.cameFromVec, current.point);
			//return vector<string>();
		}

		closedVec.push_back(current);


		vector<pair<int, int> > neighbors = problem.getSuccessors(current.point);
		for (int i = 0; i < neighbors.size(); i++)
		{

			int tempG = current.g + 1;
			int tempF = farthestTargetHeur(neighbors[i], current.unvisitedTargs) + tempG;
			Node tempNode(neighbors[i], tempF, tempG, current.unvisitedTargs, current.cameFromVec);

			vector<pair<int, int> > tempCameFrom = current.cameFromVec;
			tempCameFrom.push_back(tempNode.point);

			tempNode.cameFromVec = tempCameFrom;

			int neighborG = -1; //only used if not in openvec
			
			
			
			//Node* cNode = *closedIter;
			//Node* oNode = *openIter;

			


			vector <Node>::iterator openIter = find(openVec.begin(), openVec.end(), tempNode);
			vector <Node>::iterator closedIter = find(closedVec.begin(), closedVec.end(), tempNode);
			cout << "tg: " << tempG << " cG: " << closedIter->g << endl;
			if (closedIter != closedVec.end())
			{
				cout << "firstclosed" << endl;
				if (tempG < closedIter->g)
				{
					cout << "closeG" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}
			
			else if (openIter != openVec.end())
			{
				cout << "firstop" << endl;
				if (tempG < openIter->g)
				{
					cout << "opg" << endl;
					tempNode.g = tempG;
					tempNode.f = tempG + farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs);
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}
			else
			{
				cout << "else" << endl;

				openList.push(tempNode);
				openVec.push_back(tempNode);
			}

			////
			/*
			neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG



			if (vecContainsNode(closedVec, tempNode) && tempG < neighborG )//change this to op== overloaded
			{
				//update neighbor with lower g val
				//cout << "MEG: " << tempG << endl;
				setNodeGVal(closedVec, tempNode, tempG);
				
			}
			else if (vecContainsNode(openVec, tempNode) && tempG < neighborG)
			{
				//update neighbor with lower g val
				//cout << "MEG: " << tempG << endl;
				setNodeGVal(openVec, tempNode, tempG);
			}
			else
			{
				//add neighbor to open list with g val curr+1
				int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
				int nG = tempG;
				openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
				openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
			}
			*/
			//rewrite ends here
			/*
			if (vecContainsNode(closedVec, tempNode))//change this to op== overloaded
			{
				continue;
			}

			int neighborG = -1; //only used if not in openvec
			if (vecContainsNode(openVec, tempNode))//if im in open, use my g to compare below
			{
				neighborG = getGFromVec(openVec, tempNode);// tempNode.g;//WRONG
				
			}
			int tempG = current->g + 1;
			//int neighborG = getNodeGFromPoint(openVec, neighbors[i]);
			if ((!vecContainsNode(openVec, tempNode)) || tempG < neighborG)
			{

				cameFrom[neighbors[i]] = current->point;
				int nF = tempG + farthestTargetHeur(neighbors[i], tempNode.unvisitedTargs);
				int nG = tempG;
				if (!vecContainsPoint(openVec, neighbors[i]))
				{
					openVec.push_back(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
					openList.push(new Node(neighbors[i], nF, nG, current->unvisitedTargs, tempCameFrom));
				}
			}*/
		}
	}
	cout << "FAILURE" << endl;
	return vector<string>();
}

vector<string> questionFour(Problem &problem)
{

	clock_t t1, t2;
	t1 = clock(); //START CLOCK FOR TESTING
	//code goes here

	//get weight to use for testing
	float weight;
	cin >> weight;
	cout << "using weight: " << weight << endl;

	pair<int, int> start = problem.getStartState();
	vector<pair<int, int> > goals = problem.getGoals();

	std::cout << "start: " << start.first << start.second << endl;

	vector<Node > closedVec;
	vector<Node > openVec;
	priority_queue<Node, vector<Node>, CompareNode> openList;

	map<pair<int, int>, pair<int, int> > cameFrom;

	map<pair<int, int>, int > g_score;
	map<pair<int, int>, int > f_score;
	g_score[start] = 0;
	int gScore = 0;
	//f_score[start] = g_score[start] + nearestNeghborHeur(start, goals);
	int startF = g_score[start] + farthestTargetHeur(start, goals) * weight;


	openVec.push_back(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	openList.push(Node(start, startF, 0, goals, vector<pair<int, int> >()));
	//numgoals left + manDist to closest

	vector<string> goalToPath;
	vector<string> actualPath;



	while (!openList.empty())
	{
		//Node* current = getNodeWithMinF(openVec);// start; // = node in open with lowest f
		Node current = openList.top();
		openList.pop();
		/*
		if (current->point == goals[0])
		{
		return myGetPath(cameFrom, goals[0]);
		}
		*/
		openVec = removeNodeFrom(openVec, current);
		if (openVec.size() != openList.size())
		{
			cout << "baderror" << endl;
		}
		//cout << "OVsize: " << openVec.size() << " " << "OLsize: " << openList.size() << endl;

		//check if at goal state (all visited) also, update current's unvisited vector
		if (areWeDone(current))
		{
			cout << "WE ARE DONE " << current.cameFromVec.size() << endl;
			t2 = clock();//END CLOCK
			float diff((float)t2 - (float)t1);
			cout << "SECS: "<<  diff/CLOCKS_PER_SEC << endl;
			return getPathVec(current.cameFromVec, current.point);
			//return vector<string>();
		}

		closedVec.push_back(current);


		vector<pair<int, int> > neighbors = problem.getSuccessors(current.point);
		for (int i = 0; i < neighbors.size(); i++)
		{

			int tempG = current.g + 1;
			int tempF = weight * farthestTargetHeur(neighbors[i], current.unvisitedTargs) + tempG;
			Node tempNode(neighbors[i], tempF, tempG, current.unvisitedTargs, current.cameFromVec);

			vector<pair<int, int> > tempCameFrom = current.cameFromVec;
			tempCameFrom.push_back(tempNode.point);

			tempNode.cameFromVec = tempCameFrom;

			int neighborG = -1; //only used if not in openvec


			vector <Node>::iterator openIter = find(openVec.begin(), openVec.end(), tempNode);
			vector <Node>::iterator closedIter = find(closedVec.begin(), closedVec.end(), tempNode);
			if (closedIter != closedVec.end())
			{
				if (tempG < closedIter->g)
				{
					tempNode.g = tempG;
					tempNode.f = (weight * farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs)) + tempG;
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}

			else if (openIter != openVec.end())
			{
				if (tempG < openIter->g)
				{
					tempNode.g = tempG;
					tempNode.f =( weight * farthestTargetHeur(tempNode.point, tempNode.unvisitedTargs)) + tempG;
					openList.push(tempNode);
					openVec.push_back(tempNode);
				}
			}
			else
			{

				openList.push(tempNode);
				openVec.push_back(tempNode);
			}


		}
	}

	cout << "FAILURE" << endl;
	return vector<string>();	
}


/// Do not change codes below
vector<string> getPath(map<pair<int, int>, pair<int,int> > &parent, pair<int, int> goal)
{
	vector<string> path;
	pair<int, int> state = goal;
	int dx, dy;
	while (parent[state] != state)
	{
		dx = state.first - parent[state].first;
		dy = state.second - parent[state].second;
		if (dx>0)
			path.push_back("South");
		else if (dx<0)
			path.push_back("North");
		else if (dy>0)
			path.push_back("East");
		else
			path.push_back("West");
		state = parent[state];
	}
	reverse(path.begin(), path.end());
	return path;
}

vector<string> questionZero(Problem &problem)
{
	// A simple BFS to find the path from the start to the first goal.
	queue<pair<int, int> > q;
	map<pair<int, int>, pair<int, int> > parent;
	pair<int, int> start = problem.getStartState();
	std::cout << "start: " << start.first << start.second << endl;
	vector<pair<int, int> > goals = problem.getGoals();
	if (goals.size() <= 0)
		return vector<string>();
	pair<int, int> goal = goals[0];
	q.push(start);
	parent[start] = start;
	while (!q.empty())
	{
		pair<int, int> thisState=q.front();
		q.pop();
		vector<pair<int, int> > successors = problem.getSuccessors(thisState);
		for(int i=0; i<successors.size(); ++i)
		{
			if (parent.count(successors[i]) == 0)
			{
				q.push(successors[i]);
				parent[successors[i]] = thisState;	
			}
		}
		if (parent.count(goal) != 0)
		{
			return getPath(parent, goal);
		}
	}
	return vector<string>();
}

void error()
{
	cout <<"run \'proj1 layout_name question_number\'" <<endl;
}

int main(int argc, char**argv)
{
	if (argc<3)	
	{
		error();
		return 0;
	}
	vector<string> _board;
	_board.clear();
	string namePattern = argv[1];
	string inputName; 
#ifdef _WIN32
	inputName = "layouts\\" + namePattern + ".lay";
#else
	inputName = "layouts/" + namePattern + ".lay";
#endif
	string outputName = namePattern + ".out";
	string queryName = namePattern + "_stats.out";
	try {
		ifstream fin;
		fin.open(inputName.c_str());
		while (!fin.eof())
		{
			string str;
			getline(fin, str);
			if (!fin.eof())
				_board.push_back(str);
		}
		fin.close();
	} catch (...) {
		cout << "Error while loading the layout file..." << endl;
		return 1;
	}
	Problem problem(_board);
	vector<string> _path;
	switch (argv[2][0]){
		case '1': _path = questionOne(problem); break;
		case '2': _path = questionTwo(problem); break;
		case '3': _path = questionThree(problem); break;
		case '4': _path = questionFour(problem); break;
		default: _path = questionZero(problem);
	}
	try {
		ofstream fout;
		fout.open(outputName.c_str());
		for(int i=0; i<_path.size(); ++i)
		{
			fout << _path[i] << endl;
		}
		fout.close();
	} catch (...){
		cout << "Error while saving the results..." << endl;
		return 1;
	}
	cout << "Number of expanded states: " << problem.getExpansionCounts() << endl;
	cout << "Results are saved in " << outputName << endl; 
	problem.dumpQueries(queryName);
	return 0;
}
