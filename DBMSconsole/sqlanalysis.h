#ifndef SQLANALYSIS_H
#define SQLANALYSIS_H
#include <QCoreApplication>
#include <vector>
using namespace std;
/*SQL������*/
int sqlAnalysis(QString sql,vector<QString> &sqlkey);//����sql�﷨�����ظ���������
int createTableAna(vector<QString> &sqlkey);//��������
#endif // SQLANALYSIS_H
