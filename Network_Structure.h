#pragma once
#include<algorithm>
#include"O_D.h"
using namespace std;


class Arc {
private:
	int InitNode;
	int TermNode;
	double Capacity;
	double FreeTravelTime;
	double CurrentTravelTime;
	double PreFlow;
	double CurrentFlow;
public:
	Arc(string data[]) {
		InitNode = atoi(data[0].c_str());
		TermNode = atoi(data[1].c_str());
		Capacity = atof(data[2].c_str());
		FreeTravelTime = atof(data[4].c_str());
		CurrentTravelTime = FreeTravelTime;
		PreFlow = 0;
		CurrentFlow = 0;
	}
	int getInitNode() { return InitNode; }
	int getTermNode() { return TermNode; }
	double getCapacity() { return Capacity; }
	double getFreeTravelTime() { return FreeTravelTime; }
	double getCurrentTravelTime() { return CurrentTravelTime; }
	double getPreFlow() { return PreFlow; }
	double getCurrentFlow() { return CurrentFlow; }
	//初始化各边流量
	void resetCurrentFlow() {
		PreFlow = CurrentFlow;
		CurrentFlow = 0;
	}
	//输送流量
	void addCurrentFlow(double addFlow) {
		CurrentFlow += addFlow;
	}
	//更新各边流量
	void updateCurrentFlow(double newCurrentFlow) {
		CurrentFlow = newCurrentFlow;
	}
	//更新各边的通行时间
	void updateCurrentTravelTime() {
		CurrentTravelTime = FreeTravelTime * (1 + 0.15 * pow(CurrentFlow / Capacity, 4));	//通行时间与流量相关
	}
};

class ArcInput {
public:
	string data[10];
	ArcInput(string data_path) {
		_csvInput.open(data_path);
	}
	~ArcInput() {
		_csvInput.close();
	}
	int readline() {
		string _Oneline;
		getline(_csvInput, _Oneline);
		if (_Oneline.empty())
			return EXIT_FAILURE;
		istringstream _Readstr(_Oneline);
		string _partOfstr;							//定义一行数据中的各个字符串
		for (int i = 0; i < 10; i++) {
			getline(_Readstr, _partOfstr, ',');
			data[i] = _partOfstr;
		}


		return EXIT_SUCCESS;
	}
private:
	ifstream _csvInput;
};

vector<Arc>* LoadArcList(string data_path);

class Path {
private:
	vector<int> PassNodes;
	vector<Arc*> PassArcs;
	double TravelTime;

public:
	Path(int Origin) {
		PassNodes.push_back(Origin);
		TravelTime = 0;
	}
	int getStartNode() { return PassNodes.front(); }
	int getEndNode() { return PassNodes.back(); }
	vector<int> getPassNodes() { return PassNodes; }
	vector<Arc*> getPassArcs() { return PassArcs; }
	double getTravelTime() { return TravelTime; }
	void passArc(Arc* arc) {
		if (arc != NULL) {
			PassNodes.push_back(arc->getTermNode());
			PassArcs.push_back(arc);
			TravelTime += (arc->getCurrentTravelTime());
		}
	}
	friend bool operator!=(Path& p1, Path& p2) {
		vector<int> &Nodes1 = p1.PassNodes;
		vector<int> &Nodes2 = p2.PassNodes;
		if (Nodes1.size() != Nodes2.size())return true;
		for (int NodeNum = 0; NodeNum < Nodes1.size(); NodeNum++) {
			if (Nodes1[NodeNum] != Nodes2[NodeNum]) return true;
		}
		return false;	
	}
	friend Path operator+(Path& p1, Path& p2) {
		Path newPath = p1;
		if (p1.getEndNode() == p2.getStartNode()) {
			vector<Arc*> PassArcs = p2.getPassArcs();
			for (int ArcNum = 0; ArcNum < PassArcs.size(); ArcNum++) {
				newPath.passArc(PassArcs[ArcNum]);
			}
		}
		return newPath;
	}
	friend ostream& operator<<(ostream& out, Path& p) {
		vector<int> Nodes = p.getPassNodes();
		for (int i = 0; i < Nodes.size(); i++) {
			out << Nodes[i] << "\t";
		}
		return out;
	}
};

class NetworkStructure {
private:
	int NodeNumber;
	int ArcNumber;
	vector<vector<Arc*>> Network;
	
public:
	NetworkStructure(vector<Arc>& ArcList) {
		ArcNumber = ArcList.size();
		NodeNumber = ArcList.back().getInitNode();
		for (int node = 1; node <= NodeNumber; node++) {
			vector<Arc*> aNode;
			for (int arc = 0; arc < ArcNumber; arc++) {
				if (ArcList[arc].getInitNode() == node) {
					aNode.push_back(&ArcList[arc]);
				}
			}
			Network.push_back(aNode);
		}
		cout << "网络构建成功" << endl;
		cout << "点的数量：" << NodeNumber << endl;
		cout << "弧的数量：" << ArcNumber << endl;
		cout << "--------------------" << endl;
	}
	int getNodeNumber() { return NodeNumber; }
	int getArcNumber() { return ArcNumber; }
	//vector<Arc*> getArcFromNode(int Node) { return Network[Node-1]; }
	/*
	bool existArc(int Origin, int Destination) {
		for (int ArcNum = 0; ArcNum < Network[Origin - 1].size(); ArcNum++) {
			if (Network[Origin - 1][ArcNum]->getTermNode() == Destination) return true;
		}
		return false;
	}
	*/
	Arc* findArc(int Origin,int Destination) {
		for (int ArcNum = 0; ArcNum < Network[Origin - 1].size(); ArcNum++) {
			if (Network[Origin - 1][ArcNum]->getTermNode() == Destination) return Network[Origin - 1][ArcNum];
		}
		return NULL;
	}

	void resetNetworkFlow() {
		for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
			for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
				Network[NodeNum][ArcNum]->resetCurrentFlow();
			}
		}
	}
	void updateNetworkFlowTime() {
		for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
			for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
				Network[NodeNum][ArcNum]->updateCurrentTravelTime();
			}
		}
	}

	//交通流算法
	void Label_Correcting(vector<int> Origin, vector<vector<Path>>& ShortestPaths);
	vector<vector<Path>> MultiThread_Label_Correcting(vector<vector<ODpair>>& ODpairs);
	vector<vector<Path>> SingleThread_Label_Correcting(vector<vector<ODpair>>& ODpairs);
	
	void All_or_Nothing(vector<vector<ODpair>>& ODpairs, vector<vector<Path>>& ShortestPaths);
	double Caculate_Derivative(double alpha);
	double Find_Alpha();
	void Move_CurrentFlow();
	typedef vector<vector<Path>> (NetworkStructure::* ShortestPathAlgorithm)(vector<vector<ODpair>>& ODpairs);
	double Caculate_Delta(vector<vector<ODpair>>& ODpairs, vector<vector<Path>>& ShortestPaths, ShortestPathAlgorithm Algorithm);
	void FW_Algorithm(vector<vector<ODpair>>& ODpairs, ShortestPathAlgorithm Algorithm, ofstream& fout);

};

