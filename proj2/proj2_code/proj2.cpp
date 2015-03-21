#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>       /* sqrt */
using namespace std;

enum Status {UNSOLVED, SOLVED, CONTRADICTION};
string _status[3] = {"Unsolved", "Solved", "Contradiction"};

struct Cell {
	int i, j, k;
	Cell(): i(0), j(0), k(0){}
	Cell(int i1, int j1, int k1): i(i1), j(j1), k(k1){}

	int chain; //which chain this cell is in, by index to chains list

	bool operator==(const Cell& n)
	{
		if (i != n.i)
			return false;
		if (j != n.j)
			return false;
		if (k != n.k)
			return false;

		return true;
	}






	bool locEquals(const Cell& n)
	{
		if (i != n.i)
			return false;
		if (j != n.j)
			return false;

		return true;
	}
};

ostream& operator<<(ostream& os, const Cell& n)
{
	os << "Cell: (" << n.i << "," << n.j << "," << n.k <<")" <<endl;
	return os;
}

struct Puzzle {
	vector<vector<Cell> > disjunction;
	vector<Cell> evidence;
	Status status;
	void clearAll()
	{
		disjunction.clear();
		evidence.clear();
		status=UNSOLVED;
	}
};

//helpers

//remove vector<cell> from disjunct (a vector<vector<Cell> >)
/*vector<vector<Cell> > removeDisjunctFrom(vector<vector<Cell> > v, vector<Cell> d)
{

	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == d)
		{
			v.erase(v.begin() + i);
			return v;
		}
	}

}*/

vector<Cell> generateNegFacts(Cell c, int n, vector<vector<Cell> > chains)
{
	cout << "genNeg for :" << c;
	vector<Cell> negFacts = vector<Cell>();
	c.chain = -1;//no chain specified yet
	for (int i = 0; i < n; i++)//every one in same row
	{
		if (i != c.i)
		{
			Cell negCell = Cell(i, c.j, c.k);
			negFacts.push_back(negCell);
		}
	}


	for (int j = 0; j < n; j++)//for each in same col
	{
		if (j != c.j)
		{
			Cell negCell = Cell(c.i, j, c.k);
			negFacts.push_back(negCell);
		}
	}

	for (int k = 0; k < n; k++)//this cell can contain nothing other than k
	{
		if (k != c.k)
		{
			Cell negCell = Cell(c.i, c.j, k);
			negFacts.push_back(negCell);
		}
	}
	

	//find which chain this cell is in
	for (int j = 0; j < chains.size(); j++)
	{
		if (c.chain != -1)
			break;
		//for each cell in each chain
		for (int i = 0; i < chains[j].size(); i++)
		{
			if (c.locEquals(chains[j][i]))
			{
				c.chain = j;
				break;
			}
		}
	}

	

	//now, we know what chain c is in
	///go through each location, and create neg fact that their k can value c.k
	for (int h = 0; h < n; h++)
	{
		Cell negCell = Cell(chains[c.chain][h].i, chains[c.chain][h].j, c.k);
		if (!c.locEquals(negCell))
		{
			negFacts.push_back(negCell);
		}
	}
	//cout << negFacts.size() << endl;
	if (negFacts.size() >(n*n))
	{
		cout << "BAD, too many negs" << endl << endl;
	}
	return negFacts;
}

//does this vector<Cell> contain this cell? //return index
int vecContainsCell(vector<Cell> vec, Cell cell)
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == cell)
		{
			//cout << cell; 
			return i;
		}
	}
	//cout << "not contains cell" << endl;
	return -1;
}

// This is for Problem 1
Puzzle resolveStrimko(Puzzle &puzzle) {
	vector<vector<Cell> > disjuncts = puzzle.disjunction; //A
	vector<Cell>  facts = puzzle.evidence; //B
	
	//initialize old with initial facts
	vector<Cell> oldVec = vector<Cell>();
	oldVec = facts;
	//

	cout << facts.size() << " " << disjuncts.size() << endl;
	int nSquared = disjuncts.size() / 4;
	int n = sqrt(nSquared);
	cout << "N: " << n << endl;

	//we need to get chain locations from last n disjuncts in disjuncts
	vector<vector<Cell> > chains = vector<vector<Cell> >();
	for (int i = disjuncts.size() - 1; i > disjuncts.size() - n - 1; i--)
	{
		chains.push_back(disjuncts[i]);
		//for each cell in this chain, update its chain number to i, later
	}
	cout << "num chains " << chains.size() << endl;

	while (facts.size() < nSquared)
	{
		cout << "current facts: " << facts.size() << endl;
		//newSet = null;
		vector<Cell> newVec= vector<Cell>();
		newVec.clear();
		//


		//for all c in old
		for (int j = 0; j < oldVec.size(); j++)
		{
			//C = negative facts for empty cells in row i, col j, and chain ij**
			vector<Cell> negFacts = vector<Cell>(); //C
			negFacts.clear();
			negFacts = generateNegFacts(oldVec[j], n, chains);
			//
			for (int i = 0; i < disjuncts.size(); i++)
			{
				//each disjunct[i] that matches a negFact, remove that negFact from disjunct[i]
				for (int x = 0; x < negFacts.size(); x++)
				{
					int idx = vecContainsCell(disjuncts[i], negFacts[x]);
					if (idx != -1)
					{
						//remove negfacts[x] from disjuncts[i]
						disjuncts[i].erase(disjuncts[i].begin() + idx);
						

					}
				}

				if (disjuncts[i].size() == 0)//if length of d is 0
				{
					cout << "CONTRADICTION" << endl;
					return Puzzle();
				}
			}
			


			//for all disjunct d in A
			for (int i = 0; i < disjuncts.size(); i++)
			{
				if (disjuncts[i].size() == 1)
				{
					//remove this disjunct from A and add it to new (unless d already in B)
					
					if (vecContainsCell(facts, disjuncts[i][0]) == -1) //does not contain already
					{
						if (vecContainsCell(newVec, disjuncts[i][0]) == -1) //newvec shouldnt have it either!
						{
							cout << "ADDING TO NEW: " << disjuncts[i][0] << endl;
							newVec.push_back(disjuncts[i][0]);//only one elem
							//	i = 0;
						}
					}
					disjuncts.erase(disjuncts.begin() + i);
					i--;
				}
			}//end for


		}//end old for
		if (newVec.size() == 0)
		{
			cout << "UNSOLVED" << endl;
			return Puzzle(); //UNSOLVED
		}
		facts.insert(facts.end(), newVec.begin(), newVec.end());//add new to B
		oldVec = newVec;//set old = new
		//return Puzzle();
	}//end while
	cout << "SOLVED" << endl;

	puzzle.evidence = facts;
	return puzzle;
}

// This is for Problem 2
Puzzle solveStrimko(Puzzle &puzzle) {
	return Puzzle();
}

/// Do not change codes below

void error() {
	cout << "run \'proj1 problem_number\' or \'proj1 problem_number size_id\'" <<endl;
	cout << "\tproblem_number: 1 or 2" <<endl;
	cout << "\tsize_id: 4_1~4_5, 5_1~5_10, 6_1~6_5, 7_1~7_3" <<endl;
}

int main(int argc, char**argv) {

	if (argc < 2){
		error(); return 0;
	}
	vector<string> puzzleNameList;
	if (argc >= 3) {
		puzzleNameList.push_back(argv[2]);
	} else {
		string puzzleNameList_arr[] = {"4_1","4_2","4_3","4_4","4_5","5_1","5_2","5_3","5_4","5_5","5_6","5_7","5_8","5_9","5_10","6_1","6_2","6_3","6_4","6_5","7_1","7_2","7_3"};
		for (int i=0; i<23; ++i)
			puzzleNameList.push_back(puzzleNameList_arr[i]);
	}
	string inputName, outputName, puzzleName; 
	for (int puzzleId=0; puzzleId<puzzleNameList.size(); ++puzzleId){
		puzzleName = puzzleNameList[puzzleId];
#ifdef _WIN32
		inputName = "puzzles\\" + puzzleName + ".txt";
		outputName = "puzzles\\" + puzzleName + ".out";
#else
		inputName = "puzzles/" + puzzleName + ".txt";
		outputName = "puzzles/" + puzzleName + ".out";
#endif
		Puzzle puzzle;
		puzzle.clearAll();
		try {
			ifstream fin;
			fin.open(inputName.c_str());
			if (!fin.is_open())
				throw -1;
			int N, nDis, nEvi;
			fin >> N >> nDis >> nEvi;
			int loc_i, loc_j, num_k;
			for (int i=0; i<nDis; ++i){
				vector<Cell> this_dist;
				this_dist.clear();
				for( int j=0; j<N; ++j) {
					fin >> loc_i >> loc_j >> num_k;
					this_dist.push_back(Cell(loc_i, loc_j, num_k));
				}
				puzzle.disjunction.push_back(this_dist);
			}
			for (int i=0; i<nEvi; ++i)
			{
				fin >> loc_i >> loc_j >> num_k;
				puzzle.evidence.push_back(Cell(loc_i, loc_j, num_k));
			}
			fin.close();
		} catch (...) {
			cout << "Error while loading the puzzle file..." << endl;
			return 1;
		}
		cout << "Puzzle is loaded from " << inputName << endl;	
		Puzzle solution;
		
		switch (argv[1][0]){
			case '1': solution = resolveStrimko(puzzle); break;
			case '2': solution = solveStrimko(puzzle); break;
			case '0': solution = puzzle; break;
			default: error(); return 0;
		}
		try {
			ofstream fout;
			fout.open(outputName.c_str());
			fout << _status[solution.status] << endl;
			fout << solution.evidence.size() << endl;
			for(int i=0; i<solution.evidence.size(); ++i)
			{
				fout << solution.evidence[i].i << " " << solution.evidence[i].j << " " << solution.evidence[i].k << endl;
			}
			fout.close();
		} catch (...){
			cout << "Error while saving the results..." << endl;
			return 1;
		}
		cout << "Solution is saved in " << outputName << endl;	
	}
	return 0;
}
