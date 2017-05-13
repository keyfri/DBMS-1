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
/*数据库管理模块*/
void showdatabases(QString user);//在屏幕上打印当前用户的所有数据库名
int createDBEntity(QString user,QString DBname);//创建数据库实体
int renameDBEntity(QString user,QString newName,QString oldName);//重命名数据库实体
int dropDBEntity(QString user,QString DBname);//删除数据库实体
int useDBEntity(QString user,QString DBname,QString &curDBname);//打开并使用数据库
int initDBEntity(QString user,QString DBname);//初始化数据库
int backupDBEntity(QString user,QString DBname);//备份数据库
int recoverDBEntity(QString user,QString DBname);//恢复数据库
string getZeroNumberString(int number);//print "2015-02-02" not "2015-2-2"
QString getCurrTimeStr();//获得当前时间的字符串
int copyRecursively(const QString &srcFilePath,const QString &tgtFilePath);
int deleteDir(const QString &path);

#endif // DATABASE_H
