#ifndef USER_H
#define USER_H
#include <QCoreApplication>
#include <string>
#include <QFile>
#include <QTextStream>
#include <conio.h>
#include <QDir>
using namespace std;
//�궨��
#define UNLEN 5//�û�����󳤶�
#define PWDLEN 10//������󳤶�
typedef struct
{
    QString username;
    QString password;
}User;
/*�û�����ģ��*/
QString EnterPassword();//�������룬����ֵΪ���������
bool JudgeUser(User user);//�ж��û����������Ƿ���ڡ��Ƿ�ƥ��
void encryption(string& c, int a[]);//��д���ļ���������м��ܲ���
void decode(string& c,int a[]);//����
int regUser(User user);//ע���û�
#endif // USER_H
