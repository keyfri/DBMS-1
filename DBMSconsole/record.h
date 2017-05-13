#ifndef RECORD_H
#define RECORD_H
#include <vector>
#include <QCoreApplication>
#include <map>
using namespace std;
/*��¼����ģ��*/
int insertRecord(QString user,QString DBname,QString TBname,vector<map<QString,QString>> records);//�����¼
int updateRecord(QString user,QString DBname,QString TBname,vector<map<QString,QString>> colnames);//���¼�¼
int selectRecord(QString user,QString DBname,vector<QString> TBnames,vector<QString> colname,vector<int> rdno);//��ѯ��¼���������Ϊ�����ͼ�¼�����
int deleteRecord(QString user,QString DBname,QString TBname,int rdno);//ɾ����¼���������Ϊ��¼�����
vector<int> analyzeWhere(QString condition);//�������Ϊwhereɸѡ����������ֵΪ��¼��ŵļ���
vector<int> analyzeGroupby(QString condition);//�������Ϊgroupbyɸѡ����������ֵΪ��¼��ŵļ���
bool checkUnique(QString colname,QString content);//���Ψһ��
bool checkExist(QString value,QString TBname,QString colname);//�������ο������Ƿ��д�ֵ
#endif // RECORD_H
