// LamportsLogicalClocks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//#include "pch.h"
#include<sstream> 
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <queue>
#include <stack>
#include <cstdbool>

using namespace std;

const int EMPTY = -1;

struct Event
{
	string event;
	int LC;
	int xpos;
	int ypos;
};


//Verify functions

bool isNotFull(vector<vector<int>> &LCtemp);	//, int rsize, int csize);

void printMatrix(vector<vector<string>> &matrix);

vector<vector<string>> populateMatrix(vector<string> &lines);

vector<vector<int>> populateLCMatrix(vector<vector<string>> &matrix);

void  emptySends(vector<Event> &sends, vector<Event> &events);

void placeInternalEvents(vector<vector<string>> &matrix, vector<Event> &events);



//Universal Functions
void orderEvents(vector<Event> &recieves, bool LC = true);

bool sortEventByLC(Event first, Event second);

bool sortEventByXY(Event first, Event second);

bool sortEventByEvent(Event first, Event last);

vector<string> getTextFromFile(string fileName);



// Calculate Functions

void printMatrix(vector<vector<int>> &LCTemp);

vector<vector<string>> populateMatrix(vector<vector<int>> &LCMatrix);

vector<vector<int>> populateLCMatrix(vector<string> &lines);

int max(int first, int second);

Event sendRecieveMatch(vector<Event> &sends, string recieve);



int main()
{

	//ifstream test;
	string fileName;
	cout << "Enter filename to run Lamports Logical Clock Calculate Algorithm" << endl;
	getline(cin, fileName);

	vector<string> lines = getTextFromFile(fileName);

	char algo;
	cout << "To use algorithms Calculate/ Verify type C or V" << endl;
	cin >> algo;

	if (toupper(algo) == 'V')
	{
		//populates LCMatrix
		vector<vector<int>> LCMatrix = populateLCMatrix(lines);
		vector<vector<string>> matrix = populateMatrix(LCMatrix);


		vector<Event> events;
		vector<Event> sends;
		vector<Event> recieves;


		//populates the 3 vector of Eventes
		for (size_t i = 0; i < LCMatrix.size(); i++)
		{
			for (size_t j = 0; j < LCMatrix[i].size(); j++)
			{
				Event curr;
				curr.LC = LCMatrix[i][j];
				curr.xpos = i;
				curr.ypos = j;

				Event next;
				//this checks to see if end of vector 
				if (j == LCMatrix[i].size() - 1)
				{
					//add check for duplicates
					events.push_back(curr);
					break;
				}
				else
				{
					//Event next;
					next.LC = LCMatrix[i][j + 1];
					next.xpos = i;
					next.ypos = j + 1;
				}

				//checks to see if fist element is read event
				if (j == 0 && LCMatrix[i][j] != 1)
				{
					if (curr.LC + 1 != next.LC)
					{
						cerr << "INCORRECT INPUT" << endl;
						exit(1);
					}

					recieves.push_back(curr);


				}
				//checks to see if element is a potenial send event also handles 0's
				else if (curr.LC + 1 != next.LC)
				{

					//events.push_back(curr);
					sends.push_back(curr);
					recieves.push_back(next);
					j++;

				}
				//all other events are treated as internal events
				else
				{
					events.push_back(curr);
				}


			}

		}

		//orders vectors based on Logical clock values
		orderEvents(sends);
		orderEvents(recieves);
		orderEvents(events);

		bool found = true;
		int msgNum = 1;
		while (recieves.size() != 0 && found == true)
		{

			Event recv = recieves[0];

			// handles all zeros
			if (recv.LC == 0)
			{
				matrix[recv.xpos][recv.ypos] = "NULL";
				recieves.erase(recieves.begin());
				found = true;
			}
			else
			{

				for (size_t i = 0; i < sends.size(); i++)
				{

					Event sent = sends[i];
					if (recv.LC - 1 == sent.LC)
					{

						//r.append(to_string(i));
						matrix[recv.xpos][recv.ypos] = "r" + to_string(msgNum);
						recieves.erase(recieves.begin());
						//i--;

						//s.append(to_string(i));
						matrix[sent.xpos][sent.ypos] = "s" + to_string(msgNum);
						sends.erase(sends.begin() + i);
						//j--;
						msgNum++;
						found = true;
						break;
					}
					else
						found = false;
				}

				if (!found)
				{
					for (size_t i = 0; i < events.size(); i++)
					{
						Event event = events[i];
						if (recv.LC - 1 == event.LC)
						{

							//r.append(to_string(i));
							matrix[recv.xpos][recv.ypos] = "r" + to_string(msgNum);
							recieves.erase(recieves.begin());
							//i--;

							//s.append(to_string(i));
							matrix[event.xpos][event.ypos] = "s" + to_string(msgNum);
							events.erase(events.begin() + i);
							//k--;
							msgNum++;

							found = true;

							break;

							//sends.erase(iter);
						}
						else
							found = false;

					}
				}
			}

		}

		if (!found)
		{
			cerr << "INCORRECT INPUT" << endl;
			exit(1);
		}
		else
		{

			emptySends(sends, events);

			//order Events based on row postions in order to get correct event order
			orderEvents(events, false);

			//populates matrix with internal events 
			placeInternalEvents(matrix, events);

			//prints matrix 2D vector of strings
			printMatrix(matrix);


		}
	}
	else if (toupper(algo) == 'C')
	{
		//populates matrices
		vector<vector<string>> matrix = populateMatrix(lines);
		vector<vector<int>>LCMatrix = populateLCMatrix(matrix);

		//event vectors
		vector<Event> recieves;
		vector<Event> sends;
		vector<Event> events;

		for (size_t i = 0; i < matrix.size(); i++)
		{
			for (size_t j = 0; j < matrix[i].size(); j++)
			{
				string str = matrix[i][j];

				Event tmp;
				tmp.event = str;
				tmp.xpos = i;
				tmp.ypos = j;

				if (str[0] == 's')
					sends.push_back(tmp);
				else if (str[0] == 'r')
					recieves.push_back(tmp);
				else
					events.push_back(tmp);
			}

		}

		while (isNotFull(LCMatrix))
		{
			//handles internal events
			if (events.size() != 0)
			{
				for (size_t i = 0; i < events.size(); i++)
				{
					Event tmp = events[i];
					if (tmp.ypos == 0)
					{
						LCMatrix[tmp.xpos][tmp.ypos] = 1;
						events.erase(events.begin() + i);
						i--;
					}
					else if (LCMatrix[tmp.xpos][tmp.ypos - 1] != EMPTY)
					{
						tmp.LC = LCMatrix[tmp.xpos][tmp.ypos - 1] + 1;
						LCMatrix[tmp.xpos][tmp.ypos] = tmp.LC;
						events.erase(events.begin() + i);
						i--;
					}
					else if (tmp.event == "NULL")
					{
						tmp.LC = 0;
						LCMatrix[tmp.xpos][tmp.ypos] = tmp.LC;
						events.erase(events.begin() + i);
						i--;
					}
				}
			}

			//handles sends
			for (size_t i = 0; i < sends.size(); i++)
			{
				Event tmp = sends[i];
				if (tmp.LC > EMPTY)
				{
					continue;
				}
				else if (tmp.ypos == 0)
				{
					sends[i].LC = 1;
					LCMatrix[tmp.xpos][tmp.ypos] = sends[i].LC;
				}
				else if (LCMatrix[tmp.xpos][tmp.ypos - 1] != EMPTY)
				{
					sends[i].LC = LCMatrix[tmp.xpos][tmp.ypos - 1] + 1;
					LCMatrix[tmp.xpos][tmp.ypos] = sends[i].LC;
				}

			}

			//handles recieves without s
			if (recieves.size() == 1)
			{
				Event tmp = recieves.front();

				//Event send = sendRecieveMatch(sends, tmp.event);

				if (tmp.ypos == 0)
				{
					Event send = sendRecieveMatch(sends, tmp.event);

					if (send.LC <= EMPTY)
					{
						cerr << "Invalid Input" << endl;
						exit(1);
					}
					else
					{
						LCMatrix[tmp.xpos][tmp.ypos] = send.LC + 1;
						recieves.erase(recieves.begin());

					}


				}
				else if (LCMatrix[tmp.xpos][tmp.ypos - 1] != EMPTY)
				{
					int k = LCMatrix[tmp.xpos][tmp.ypos - 1];

					Event send = sendRecieveMatch(sends, tmp.event);

					if (send.LC <= EMPTY)
					{
						cerr << "Invalid Input" << endl;
						exit(1);
					}
					else
					{
						LCMatrix[tmp.xpos][tmp.ypos] = send.LC + 1;
						recieves.erase(recieves.begin());

					}
				}
				else
				{
					cerr << "Invalid Input" << endl;
					exit(1);
				}
			}
			else
			{
				//handles recieves
				for (size_t i = 0; i < recieves.size(); i++)
				{
					Event tmp = recieves[i];

					if (tmp.ypos == 0)
					{
						Event send = sendRecieveMatch(sends, tmp.event);

						if (send.LC <= EMPTY)
						{
							continue;

						}
						else
						{
							LCMatrix[tmp.xpos][tmp.ypos] = send.LC + 1;
							recieves.erase(recieves.begin() + i);
							i--;
						}



					}
					else if (LCMatrix[tmp.xpos][tmp.ypos - 1] != EMPTY)
					{
						int k = LCMatrix[tmp.xpos][tmp.ypos - 1];

						Event send = sendRecieveMatch(sends, tmp.event);

						if (send.LC <= EMPTY)
						{
							continue;
						}
						else
						{
							LCMatrix[tmp.xpos][tmp.ypos] = max(k, send.LC) + 1;
							recieves.erase(recieves.begin() + i);
							i--;
						}
							
					}


				}
			}


		}


		printMatrix(LCMatrix);


	}


	return 0;
}


#pragma region Verifier Functions

/*
Author: Carlos Meza
Description: Function that places all internal events into 2d matrix
Input: 2D matrix of strings and a vector<Event> events
Output: 2D matrix populated with internal events
*/
void placeInternalEvents(vector<vector<string>> &matrix, vector<Event> &events)
{
	char let = 'a';
	for (size_t i = 0; i < events.size(); i++, let++)
	{
		Event first = events[i];
		matrix[first.xpos][first.ypos] = let;

	}
}

/*
Author: Carlos Meza
Description: Places all unused sends into event vector 
			 meaning that these sends are internal evenss
Input: vector of sends Events and vector of Event events
Output: All unused send Events get pushed into vector<Events> events.
*/
void  emptySends(vector<Event> &sends, vector<Event> &events)
{
	if (sends.size() == 1)
	{
		events.push_back(*sends.begin());
		sends.erase(sends.begin());
	}
	else
	{
		for (size_t i = 0; i < sends.size(); i++)
		{
			events.push_back(*sends.begin());
			sends.erase(sends.begin());
			i--;

		}
	}
}

/*
Author: Carlos Meza
Description: Boolean function that is used for orderEvent function and sorts by LC
Input: 2 Events
Output: True if first Event is less than 2nd one else false
*/
bool sortEventByLC(Event first, Event second)
{
	return (first.LC < second.LC);
}
/*
Author: Carlos Meza
Description: Boolean function that is used to for orderEvent function and sorts by xpos
Input: 2 Events
Output: True if first Event is less than 2nd one else false
*/
bool sortEventByXY(Event first, Event second)
{
	return first.xpos < second.xpos;
}

bool sortEventByEvent(Event first, Event last)
{
	return first.event.back() < last.event.back();
}

/*
Author: Carlos Meza
Description: A soritng function that orders vector<Event> by function passed to it 
Input: Vectpr<Event> and bool to determine how to sort IF True than sorts by LC else sorts by xpos
Output: ordered vector of Eventes
*/
void orderEvents(vector<Event> &recieves, bool LC)
{
	if (LC)
		sort(recieves.begin(), recieves.end(), sortEventByLC);
	else
		sort(recieves.begin(), recieves.end(), sortEventByXY);
}


/*
Author:Carlos Meza
Descriptoin: populates LCMatrix based on text from file stored in vector
Input: vector<string> lines
Output: N x M matrix filled with ints
*/
vector<vector<string>> populateMatrix(vector<vector<int>> &LCMatrix)
{
	vector<vector<string>>matrix(LCMatrix.size());
	for (size_t i = 0; i < LCMatrix.size(); i++)
	{
		matrix[i] = vector<string>(LCMatrix[i].size(), "");

	}
	return matrix;
}

/*
Author: Carlos Meza
Description: printMatrix
Input: 2d vector, rowSize, colSize
Output: prints 2d Vector
*/
void printMatrix(vector<vector<string>> &matrix)//, int rowSize, int colSize)
{
	ofstream out("outputV.txt");
	
	if (!out)
	{
		cerr << "Unable to create output txt file" << endl;
		exit(1);
	}
	else
	{
		for (size_t i = 0; i < matrix.size(); i++)
		{
			for (size_t j = 0; j <= matrix[i].size() - 1; j++)
			{
				cout << matrix[i][j] << " ";
				out << matrix[i][j] << " ";
			}
			cout << endl;
			out << endl;

		}

	}

	out.close();

}

/*
Author:Carlos Meza
Descriptoin: populates LCMatrix based on text from file stored in vector
Input: vector<string> lines
Output: N x M matrix filled with ints
*/
vector<vector<int>> populateLCMatrix(vector<string> &lines)
{
	vector<vector<int>> matrix(lines.size());
	for (size_t i = 0; i < lines.size(); i++)
	{
		string line = lines[i];
		stringstream str(line);

		int tmp;
		while (str >> tmp)
		{
			matrix[i].push_back(tmp);

		}

	}
	return matrix;


}

#pragma endregion

#pragma region Calculate Functions
/*
Author:Carlos Meza
Description: populates an N x M matrix
Input: vector of strings
Output: N x M string matrix
*/
vector<vector<string>> populateMatrix(vector<string> &lines)
{
	vector<vector<string>> matrix(lines.size());
	for (size_t i = 0; i < lines.size(); i++)
	{
		string line = lines[i];
		stringstream str(line);

		string tmp;
		while (str >> tmp)
		{
			matrix[i].push_back(tmp);

		}

	}
	return matrix;
}

/*
Author:Carlos Meza
Description: populates an N x M matrix
Input: N x M string matrix
Output: N x M int matrix
*/
vector<vector<int>> populateLCMatrix(vector<vector<string>> &matrix)
{
	vector<vector<int>>LCMatrix(matrix.size());
	for (size_t i = 0; i < LCMatrix.size(); i++)
	{
		LCMatrix[i] = vector<int>(matrix[i].size(), EMPTY);

	}
	return LCMatrix;
}

/*
Author: Carlos Meza
Function: isNotFull
Description: checks to see if int 2d array is filled
Input: 2dArray of max size 10 x 10, row size, column size
Output: boolean value that returns false if 2d array is filled, true otherwise
*/
bool isNotFull(vector<vector<int>> &LCtemp)//, int rsize, int csize)
{


	for (size_t i = 0; i < LCtemp.size(); i++)
	{
		for (size_t j = 0; j < LCtemp[i].size(); j++)
		{
			if (LCtemp[i][j] == EMPTY)
				return true;
		}
	}
	return false;

}

/*
Author:Carlos Meza
Description: max
Input: two integers
Output: returns the value of the two integers
*/
int max(int first, int second)
{
	if (first >= second)
		return first;
	else
		return second;


}

/*
Author: Carlos Meza
Description: sendRecieveMatch
Input: a vector of send events, a string for recieve event
Output: returns the send event corresponding with recieve event
		if not found will return an EMPTY event
Note: utilizes an iterator to loop through vector and if send event
	  is found we erase it from the vector
*/
Event sendRecieveMatch(vector<Event> &sends, string recieve)
{
	vector<Event>::iterator i;
	for (i = sends.begin(); i < sends.end(); i++)
	{
		Event tmp = *i;
		if (tmp.event.back() == recieve.back())
		{

			//sends.erase(i);
			return tmp;
		}
	}

	Event tmp;
	tmp.LC = EMPTY;
	return tmp;


}
#pragma endregion

#pragma region Universal Functions

vector<string> getTextFromFile(string fileName)
{
	ifstream test;
	test.open(fileName);
	vector<string> lines;

	if (!test)
	{
		cerr << "Unable to open file" << endl;
		exit(0);
	}
	else
	{
		string line;
		while (test.good())
		{
			getline(test, line);
			lines.push_back(line);
		}
	}

	return lines;

}


/*
Author: Carlos Meza
Description: printMatrix
Input: 2d vector, rowSize, colSize
Output: prints 2d Vector
*/
void printMatrix(vector<vector<int>> &LCtemp)//, int rowSize, int colSize)
{
	ofstream out("outputC.txt");
	if (!out)
	{
		cerr << "Unable to open output txt file " << endl;
		exit(1);
	}
	else
	{
		for (size_t i = 0; i < LCtemp.size(); i++)
		{
			for (size_t j = 0; j <= LCtemp[i].size() - 1; j++)
			{
				int tmp = LCtemp[i][j];
				cout << tmp << " ";
				out << tmp << " ";
			}
			cout << endl;
			out << endl;

		}
	}
	out.close();



}


#pragma endregion


/*while (isNotFull(LCMatrix))
{

	for (int i = 0; i < LCMatrix.size(); i++)
	{

		for (int j = 0; j <= LCMatrix[i].size() - 1; j++)
		{

			string tmp = matrix[i][j];
			//Rule 1
			if (j == 0 && tmp[0] != 'r')
			{
				LCMatrix[i][j] = 1;
				if (tmp[0] == 's')
				{
					//this saves the LC value of send message to an event
					Event send;
					send.LC = LCMatrix[i][j];		// +1;
					send.event_type = tmp;
					sendEvents.push_back(send);
				}
			}
			//Rule 2
			else if (j == 0 && tmp[0] == 'r')
			{
				Event send = sendRecieveMatch(sendEvents, tmp);
				if (send.LC == EMPTY)
				{
					break;
				}
				else
				{
					//int k = LCMatrix[i][j];

					//if (k < 0)
						//LCMatrix[i][j] = max(k, send.LC) + 1;
					LCMatrix[i][j] = send.LC + 1;
				}


			}
			//Rule 3
			else if (j != 0 && tmp[0] != 'r')
			{
				int k = LCMatrix[i][j - 1];
				LCMatrix[i][j] = k + 1;
				if (tmp[0] == 's')
				{
					//this saves the LC value of send message to an event
					Event send;
					send.LC = k + 1;
					send.event_type = tmp;
					sendEvents.push_back(send);
				}
				else if (tmp == "NULL")
				{
					LCMatrix[i][j] = 0;

				}

			}
			//Rule 4
			else if (j != 0 && tmp[0] == 'r')
			{
				Event send = sendRecieveMatch(sendEvents, tmp);
				if (send.LC == EMPTY)
				{
					break;
				}
				else
				{
					int k = LCMatrix[i][j - 1];
					LCMatrix[i][j] = max(k, send.LC) + 1;
				}

			}
			else if (tmp == "NULL")
			{
				LCMatrix[i][j] = 0;
			}

		}

	}
}*/