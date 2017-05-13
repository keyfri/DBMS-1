#ifndef DATABASE_H
#define DATABASE_H
#include <QCoreApplication>
#include <string>
#include<cstring>
#include<vector>
#include<ctime>
#include<io.h>
#include<direct.h>
#include<fstream>
#include<iostream>
#include<QFile>
#include<QTextStream>
#include<QString>
#include<QDebug>
#include<QStringList>
#include<iomanip>
#include <QDir>
using namespace std;
/*���ݿ����ģ��*/
void showdatabases(QString user);//����Ļ�ϴ�ӡ��ǰ�û����������ݿ���
int createDBEntity(QString user,QString DBname);//�������ݿ�ʵ��
int renameDBEntity(QString user,QString newName,QString oldName);//���������ݿ�ʵ��
int dropDBEntity(QString user,QString DBname);//ɾ�����ݿ�ʵ��
int useDBEntity(QString user,QString DBname,QString &curDBname);//�򿪲�ʹ�����ݿ�
int initDBEntity(QString user,QString DBname);//��ʼ�����ݿ�
int backupDBEntity(QString user,QString DBname);//�������ݿ�
int recoverDBEntity(QString user,QString DBname);//�ָ����ݿ�
string getZeroNumberString(int number);//print "2015-02-02" not "2015-2-2"
QString getCurrTimeStr();//��õ�ǰʱ����ַ���
int copyRecursively(const QString &srcFilePath,const QString &tgtFilePath);
int deleteDir(const QString &path);

#endif // DATABASE_H
