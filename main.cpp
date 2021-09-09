#include"Network_Structure.h"
#include"O_D.h"
#include<ctime>
using namespace std;

int main() {
	/*
	//Sixous Falls Network
	//��������ļ�
	ofstream fout("F://���罨ģ���Ż�//��ͨ����������//Sixous Falls Network Result (MultiThread).txt", ios_base::out);
	if (!fout.is_open())
	{
		cerr << "���ļ�ʧ��!" << endl;
		return -1;
	}
	
	//��������
	vector<Arc>* ArcList = LoadArcList("F://���罨ģ���Ż�//��ͨ����������//Sixous Falls Network.csv");
	vector<vector<ODpair>>* ODpairs = LoadODpairs("F://���罨ģ���Ż�//��ͨ����������//Sixous Falls Network OD.csv");
	*/
	
	//Chicago sketch network
	//��������ļ�
	ofstream fout("F://���罨ģ���Ż�//��ͨ����������//Chicago Sketch Network Result (SingleThread).txt", ios_base::out);
	if (!fout.is_open())
	{
		cerr << "���ļ�ʧ��!" << endl;
		return -1;
	}
	//��������
	vector<Arc>* ArcList = LoadArcList("F://���罨ģ���Ż�//��ͨ����������//Chicago Sketch Network.csv");
	vector<vector<ODpair>>* ODpairs = LoadODpairs("F://���罨ģ���Ż�//��ͨ����������//Chicago Sketch Network OD.csv");
	
	
	NetworkStructure* Network = new NetworkStructure(*ArcList);
	/*
	for (int j = 0; j < (*ODpairs).back().size(); j++) {
		cout << (*ODpairs).back()[j].getOrigin() << "\t" << (*ODpairs).back()[j].getDestination() << "\t" << (*ODpairs).back()[j].getDemand() << endl;
	}
	*/
	/*
	//���·�㷨����
	
	vector<vector<Path>> All_ShortestPath = Network->MultiThread_Label_Correcting(*ODpairs);
	
	for (int OriNum = 0; OriNum < (*ODpairs).size(); OriNum++) {
		for (int DesNum = 0; DesNum < (*ODpairs)[OriNum].size(); DesNum++) {
			int Origin = (*ODpairs)[OriNum][DesNum].getOrigin();
			int Destination = (*ODpairs)[OriNum][DesNum].getDestination();
			double Demand = (*ODpairs)[OriNum][DesNum].getDemand();
			cout << "��" << Origin << "����" << Destination << "�����·��" << endl;
			cout << All_ShortestPath[OriNum][Destination - 1] << endl;
		}
	}
	*/
	
	clock_t StartTime = clock();
	Network->FW_Algorithm(*ODpairs, &NetworkStructure::SingleThread_Label_Correcting, fout);
	clock_t EndTime = clock();
	cout << "����ʱ�䣺" << ((double)EndTime - (double)StartTime) / CLOCKS_PER_SEC <<"��"<< endl;
	fout << "����ʱ�䣺" << ((double)EndTime - (double)StartTime) / CLOCKS_PER_SEC << "��" << endl;
	
	
	//�ͷ��ڴ�
	delete Network;
	delete ODpairs;
	delete ArcList;
	//�ر��ļ�
	fout.close();
	return 0;
}