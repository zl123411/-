#pragma once
#include"Network_Structure.h"
#include<queue>
#include<thread>
#define Infinity 1e8
#define error 1e-4
#define MaxIteration 50
const int hardware_threads = thread::hardware_concurrency();

//Label Correcting�㷨
void NetworkStructure:: Label_Correcting(vector<int> Origins, vector<vector<Path>>& ShortestPaths) {
	for (int NodeNum = 0; NodeNum < Origins.size(); NodeNum++) {
		int Origin = Origins[NodeNum];
		//���ǩ
		vector<double> NodeLabels(NodeNumber, Infinity);
		vector<bool> ExistLabels(NodeNumber, false);
		NodeLabels[Origin - 1] = 0;

		//·������
		queue<int> PathQueue;
		PathQueue.push(Origin);
		ExistLabels[Origin - 1] = true;

		while (!PathQueue.empty()) {
			int StartNode = PathQueue.front();
			Path extendPath = ShortestPaths[NodeNum][StartNode - 1];

			for (int ArcNum = 0; ArcNum < Network[StartNode - 1].size(); ArcNum++) {
				//·������
				Path aPath = extendPath;
				aPath.passArc(Network[StartNode - 1][ArcNum]);
				//�����ڵ�
				int EndNode = aPath.getEndNode();
				double newLableValue = aPath.getTravelTime();

				if (newLableValue < NodeLabels[EndNode - 1]) {
					ShortestPaths[NodeNum][EndNode - 1] = aPath;
					NodeLabels[EndNode - 1] = newLableValue;
					if (!ExistLabels[EndNode - 1]) {
						PathQueue.push(EndNode);
						ExistLabels[EndNode - 1] = true;
					}
				}
			}
			PathQueue.pop();
			ExistLabels[StartNode - 1] = false;
		}
	}
}

vector<vector<Path>> NetworkStructure::MultiThread_Label_Correcting(vector<vector<ODpair>>& ODpairs) {
	const int OriNumber = ODpairs.size();
	//���̼߳���(��������������·)
	const int OneThreadNodeNumber = OriNumber / hardware_threads + 1;
	vector<thread> threads(hardware_threads);
	vector<vector<vector<Path>>> ShortestPaths(hardware_threads);	//������õ����·����
	int ThreadNum = 0;
	int OriNum = 0;
	for (ThreadNum; ThreadNum < hardware_threads; ThreadNum++) {
		vector<int>Origins;
		for (OriNum; OriNum < OriNumber; OriNum++) {
			int Origin = ODpairs[OriNum][0].getOrigin();
			if (Origins.size() < OneThreadNodeNumber) {
				ShortestPaths[ThreadNum].push_back(vector<Path>(NodeNumber, Origin));
				Origins.push_back(Origin);
			}
			else break;
		}
		threads[ThreadNum] = thread(&NetworkStructure::Label_Correcting, this, Origins, ref(ShortestPaths[ThreadNum]));
		if (OriNum == OriNumber) break;
	}
	for (int i = 0; i <= ThreadNum; i++) {
		threads[i].join();
	}

	vector<vector<Path>> All_ShortestPaths;
	for (int i = 0; i <= ThreadNum; i++) {
		for (int j = 0; j < ShortestPaths[i].size(); j++) {
			All_ShortestPaths.push_back(ShortestPaths[i][j]);
		}
	}
	/*
			for (int OriNum = 0; OriNum < OriNumber; OriNum += hardware_threads) {
				int run = 0;
				for (run; run < hardware_threads; run++) {
					if (OriNum + run < OriNumber) {
						int Origin = ODpairs[OriNum + run][0].getOrigin();
						threads[run] = thread(&NetworkStructure::Label_Correcting, this, Origin, ref(All_ShortestPaths[OriNum + run]));
					}
					else break;
				}
				for (int t = 0; t < run; t++) {
					threads[t].join();
				}
			}
			*/
	return All_ShortestPaths;
}

vector<vector<Path>> NetworkStructure::SingleThread_Label_Correcting(vector<vector<ODpair>>& ODpairs) {
	vector<int> All_Origins;	//�������
	vector<vector<Path>> All_ShortestPaths;
	for (int OriNum = 0; OriNum < ODpairs.size(); OriNum++) {
		int Origin = ODpairs[OriNum][0].getOrigin();
		All_ShortestPaths.push_back(vector<Path>(NodeNumber,Origin));
		All_Origins.push_back(Origin);
	}
	/*
	for (int OriNum = 0; OriNum < ODpairs.size(); OriNum++) {
		int Origin = ODpairs[OriNum][0].getOrigin();
		vector<Path> Origin_ShortestPaths(NodeNumber, Path(Origin));
		Label_Correcting(Origin, Origin_ShortestPaths);
		All_ShortestPaths.push_back(Origin_ShortestPaths);
	}
	*/
	Label_Correcting(All_Origins, All_ShortestPaths);
	return All_ShortestPaths;
}

//All_or_Nothing
void NetworkStructure::All_or_Nothing(vector<vector<ODpair>>& ODpairs, vector<vector<Path>>& ShortestPaths){
	//������ʼ��
	resetNetworkFlow();
	//��������
	for (int OriNum = 0; OriNum < ODpairs.size(); OriNum++) {
		for (int DesNum = 0; DesNum < ODpairs[OriNum].size(); DesNum++) {
			int Destination = ODpairs[OriNum][DesNum].getDestination();
			double Demand = ODpairs[OriNum][DesNum].getDemand();
			vector<Arc*> PassArcs = ShortestPaths[OriNum][Destination - 1].getPassArcs();
			for (int ArcNum = 0; ArcNum < PassArcs.size(); ArcNum++) {
				PassArcs[ArcNum]->addCurrentFlow(Demand);
			}
		}
	}
}

double NetworkStructure::Caculate_Derivative(double alpha) {
	double DerivativeValue = 0;
	for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
		for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
			double t0 = Network[NodeNum][ArcNum]->getFreeTravelTime();
			double x_a = Network[NodeNum][ArcNum]->getPreFlow();
			double y_a = Network[NodeNum][ArcNum]->getCurrentFlow();
			double c_a = Network[NodeNum][ArcNum]->getCapacity();
			DerivativeValue += t0 * (1 + 0.15 * pow((x_a + alpha * (y_a - x_a)) / c_a, 4)) * (y_a - x_a);
		}
	}
	return DerivativeValue;
}


double NetworkStructure::Find_Alpha() {
	//���ֲ���
	double left = 0;
	double right = 1;
	double alpha = (left + right) / 2;
	//if (Caculate_Derivative(left) >= 0) return left;
	//if (Caculate_Derivative(right) <= 0) return right;
	double DerivativeValue = Caculate_Derivative(alpha);
	while (fabs(DerivativeValue) > error)
	{
		if (DerivativeValue < 0) {
			left = alpha;
			alpha = (left + right) / 2;
		}
		else if (DerivativeValue > 0) {
			right = alpha;
			alpha = (left + right) / 2;
		}
		else {
			return alpha;
		}
		DerivativeValue = Caculate_Derivative(alpha);
	}
	cout << "�ҵ����ŵ�alpha=" << alpha << "������Ϊ" << DerivativeValue << endl;
	return alpha;
}

void NetworkStructure::Move_CurrentFlow() {
	double alpha = Find_Alpha();
	for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
		for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
			double x_a = Network[NodeNum][ArcNum]->getPreFlow();
			double y_a = Network[NodeNum][ArcNum]->getCurrentFlow();
			Network[NodeNum][ArcNum]->updateCurrentFlow(x_a + alpha * (y_a - x_a));
		}
	}
}

double NetworkStructure::Caculate_Delta(vector<vector<ODpair>>& ODpairs, vector<vector<Path>>& ShortestPaths, ShortestPathAlgorithm Algorithm) {
	double upper = 0;
	double downer = 0;
	//�ҵ�x_n+1����µ����·
	ShortestPaths = (this->*Algorithm)(ODpairs);
	for (int OriNum = 0; OriNum < ODpairs.size(); OriNum++) {
		for (int DesNum = 0; DesNum < ODpairs[OriNum].size(); DesNum++) {
			int Destination = ODpairs[OriNum][DesNum].getDestination();
			double Demand = ODpairs[OriNum][DesNum].getDemand();
			upper += ShortestPaths[OriNum][Destination - 1].getTravelTime() * Demand;
		}
	}
	for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
		for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
			downer += Network[NodeNum][ArcNum]->getCurrentTravelTime() * Network[NodeNum][ArcNum]->getCurrentFlow();
		}
	}
	return fabs(upper - downer) / downer;
}

void NetworkStructure::FW_Algorithm(vector<vector<ODpair>>& ODpairs, ShortestPathAlgorithm Algorithm, ofstream& fout) {
	cout << "�������߳�����" << hardware_threads << endl;
	cout << "--------------------" << endl;
	vector<vector<Path>>ShortestPaths = (this->*Algorithm)(ODpairs);		//��ʼ�����·
	All_or_Nothing(ODpairs, ShortestPaths);									//����x_0
	updateNetworkFlowTime();												//����t^0_a
	ShortestPaths = (this->*Algorithm)(ODpairs);							//�������·
	cout << "������ʼ�����" << endl;	
	/*
	cout << "Arc\t" << "Flow\t" << "TravelTime" << endl;
	for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
		for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
			cout << "(" << Network[NodeNum][ArcNum]->getInitNode() << "," << Network[NodeNum][ArcNum]->getTermNode() << ")\t"
				 << Network[NodeNum][ArcNum]->getCurrentFlow() << "\t" << Network[NodeNum][ArcNum]->getCurrentTravelTime() << endl;
		}
	}
	*/
	cout << "--------------------" << endl;

	int iteration = 0;
	double Delta = Infinity;
	while (Delta > error ) {
		iteration++;
		cout << "��" << iteration << "�ε���" << endl;
		fout << "��" << iteration << "�ε���" << endl;
		All_or_Nothing(ODpairs, ShortestPaths);								//����y_n
		Move_CurrentFlow();													//����x_n+1
		updateNetworkFlowTime();											//����t^n+1_a
		Delta = Caculate_Delta(ODpairs, ShortestPaths, Algorithm);			//�������
		cout << "���Ϊ��" << Delta << endl;
		fout << "���Ϊ��" << Delta << endl;
		cout << "--------------------" << endl;
		fout << "--------------------" << endl;
	}

	//������ߵ�������ͨ��ʱ��
	cout << "������ɣ�����������" << iteration << endl;
	cout << "�û����������£����ߵ�������ͨ��ʱ��Ϊ��" << endl;
	cout << "Arc\t" << "Flow\t" << "TravelTime" << endl;
	fout << "������ɣ�����������" << iteration << endl;
	fout << "�û����������£����ߵ�������ͨ��ʱ��Ϊ��" << endl;
	fout << "Arc\t" << "Flow\t" << "TravelTime" << endl;
	for (int NodeNum = 0; NodeNum < NodeNumber; NodeNum++) {
		for (int ArcNum = 0; ArcNum < Network[NodeNum].size(); ArcNum++) {
			cout << "(" << Network[NodeNum][ArcNum]->getInitNode() << "," << Network[NodeNum][ArcNum]->getTermNode() << ")\t"
				   << Network[NodeNum][ArcNum]->getCurrentFlow() << "\t" << Network[NodeNum][ArcNum]->getCurrentTravelTime() << endl;
			fout << "(" << Network[NodeNum][ArcNum]->getInitNode() << "," << Network[NodeNum][ArcNum]->getTermNode() << ")\t"
				<< Network[NodeNum][ArcNum]->getCurrentFlow() << "\t" << Network[NodeNum][ArcNum]->getCurrentTravelTime() << endl;
		}
	}
	cout << "--------------------" << endl;
	fout << "--------------------" << endl;
}