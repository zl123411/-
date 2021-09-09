#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
using namespace std;

class ODpair {
private:
	int Origin;
	int Destination;
	double Demand;
public:
	ODpair() {};
	ODpair(int Origin,int Destination,double Demand) {
		this->Origin = Origin;
		this->Destination = Destination;
		this->Demand = Demand;
	}
	int getOrigin() { return Origin; }
	int getDestination() { return Destination; }
	double getDemand() { return Demand; }
};


class ODInput {
public:
	string data[10];
	string Preline;
	ODInput(string data_path) {
		_csvInput.open(data_path);
	}
	~ODInput() {
		_csvInput.close();
	}
	int readline() {
		string _Oneline;
		getline(_csvInput, _Oneline);
		if (Preline.empty() && _Oneline.empty())
			return EXIT_FAILURE;
		istringstream _Readstr(_Oneline);
		string _partOfstr;							//定义一行数据中的各个字符串
		for (int i = 0; i < 10; i++) {
			getline(_Readstr, _partOfstr, ',');
			data[i] = _partOfstr;
		}
		Preline = _Oneline;

		return EXIT_SUCCESS;
	}
private:
	ifstream _csvInput;
};

vector<vector<ODpair>>* LoadODpairs(string data_path);