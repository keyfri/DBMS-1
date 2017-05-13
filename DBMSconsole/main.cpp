//�����ļ�
#include <QCoreApplication>
#include <iostream>
#include <vector>
#include "convert.h"
#include "database.h"
#include "index.h"
#include "record.h"
#include "table.h"
#include "user.h"
#include "sqlanalysis.h"
//�����ռ�
using namespace  std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    User user;
    User reg;
    reg.username="root";//ע���õ��û���
    reg.password="root";//ע���õ�����

    switch(regUser(reg))//TODO:������
    {
    case 0:
        cout<<"Registion success!"<<endl;
        break;
    case 1:
        cout<<"Registion failed because this user exists!"<<endl;
        break;
    case 2:
        cout<<"Registion failed because account or password is not valid!"<<endl;//�˺����5λ����1λ���������10λ����1λ��ֻ������ĸ�����ִ�Сд�����������
        break;
    case 3:
        cout<<"Registion failed because of system error!"<<endl;//һ�����ļ�������
        break;
    default:
        break;
    }

    while(1)
    {
        cout<<"Please input account:";//Ĭ���˺���root
        string u;
        cin>>u;
        user.username = QString::fromStdString(u);
        getchar();
        cout<<"Please input password:";//Ĭ��������root
        user.password = EnterPassword();
        if(JudgeUser(user))
        {
            cout<<"Login success!"<<endl;

            QString curuser = user.username;
            QString curDBname = "";
			while(1)
			{
                cout<<"sql>>";
				int sqlType;//sql��������ͣ������������ݿ⡢����������ֶε���ȷ������֮�����չ����﷨������
                string sql;//�����sql����
                vector<QString> sqlkey;
                int result;
				
                getline(cin,sql);//TODO:��ʱû���ǻ��е����⣬��һ��SQL�������һ����д��ģ��ԷֺŽ�β
                QString qsql = stringtoqstring(sql);
                sqlType = sqlAnalysis(qsql,sqlkey);
                switch(sqlType)
				{
                case 0://�������ݿ�
                    result = createDBEntity(curuser,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Create database successfully!"<<endl;
                        break;
                    case -1:
                        cout<<"The database has existed!"<<endl;
                        break;
                    case -2:
                        cout<<"Create dbDir failed!"<<endl;
                        break;
                    case -3:
                        cout<<"Open database.txt firsttime failed!"<<endl;
                        break;
                    case -4:
                        cout<<"Rewrite database.txt failed!"<<endl;
                        break;
                    }
                    break;
                case 1://���������ݿ�
                    result=renameDBEntity(curuser,sqlkey.at(1),sqlkey.at(0));
                    if(curDBname==sqlkey.at(0))
                    {
                        cout<<"The database is being used!"<<endl;
                    }
                    else if(curDBname == sqlkey.at(1))
                    {
                        cout<<"The database's name have already existed!"<<endl;
                    }
                    else
                    {
                        switch(result)
                        {
                        case 1:
                            cout<<"Rename database successfully!"<<endl;
                            break;
                        case -1:
                            cout<<"The olddatabase not existed!"<<endl;
                            break;
                        case -2:
                            cout<<"The new database name has existed!"<<endl;
                            break;
                        case -3:
                            cout<<"Open the database.txt fail!"<<endl;
                            break;
                        case -4:
                            cout<<"Open the database.txt again fail"<<endl;
                            break;
                        }
                    }
                    break;
                case 2://ɾ�����ݿ�
                    result=dropDBEntity(curuser,sqlkey.at(0));
                    if(curDBname == sqlkey.at(0))
                    {
                        cout<<"The database is being used!"<<endl;
                    }
                    else
                    {
                        switch(result)
                        {
                        case 1:
                            cout<<"Drop database successfully!"<<endl;
                            break;
                        case -1:
                            cout<<"The db which to be dropped is not exist!"<<endl;
                            break;
                        case -2:
                            cout<<"Delete the dir fail!"<<endl;
                            break;
                        case -3:
                            cout<<"Open the database.txt fail!"<<endl;
                            break;
                        case -4:
                            cout<<"Open the database.txt again fail!"<<endl;
                            break;
                        }
                    }
                    break;
                case 3://ʹ�����ݿ�
                    result=useDBEntity(curuser,sqlkey.at(0),curDBname);
                    //cout<<qstringtostring(curDBname)<<endl;
                    switch(result)
                    {
                    case 1:
                        cout<<"Database changed."<<endl;
                        break;
                    case -1:
                        cout<<"The dbname not exist"<<endl;
                        break;
                    }
                    break;
                case 4://��ʾ�������ݿ�
                    if(curuser == "") cout<<"Please use a database!"<<endl;
                    else showdatabases(curuser);
                    break;
                case 5://��ʼ�����ݿ�
                    result = initDBEntity(curuser,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Initiallize database successfully!"<<endl;
                        break;
                    case -1:
                        cout<<"Drop database failed!"<<endl;
                        break;
                    case -2:
                        cout<<"Create database failed!"<<endl;
                        break;
                    }
                    break;
                case 6://�������ݿ�
                    result = backupDBEntity(curuser,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Backup database successfully!"<<endl;
                        break;
                    case -1:
                        cout<<"Copy file failed!"<<endl;
                        break;
                    case -2:
                        cout<<"Create new dir failed!"<<endl;
                        break;
                    case -3:
                        cout<<"The database doesn't exist"<<endl;
                        break;
                    }
                    break;
                case 7://�ָ����ݿ�
                    result = recoverDBEntity(curuser,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Recover database successfully!"<<endl;
                        break;
                    case -1:
                        cout<<"Copy file failed!"<<endl;
                        break;
                    case -2:
                        cout<<"Create new dir failed!"<<endl;
                        break;
                    case -3:
                        cout<<"The database haven't been backuped!"<<endl;
                        break;
                    }
                    break;
                case 8://��ʾ���б�
                    result = showtables(curuser,curDBname);
                    switch(result)
                    {
                    case -1:
                        cout<<"Open file failed!"<<endl;
                        break;
                    case -2:
                        cout<<"Please choose a database!"<<endl;
                        break;
                    case -3:
                        cout<<"The database is empty!"<<endl;
                        break;
                    }
                    break;
                case 9://������
                {
                    vector<Field> fields;
                    vector<Constraint> constraints;
                    for(int i=0;i<qstringtoint(sqlkey.at(0));i++)
                    {
                        Field onefield;
                        onefield.name=sqlkey.at(3+3*i);
                        onefield.type=sqlkey.at(4+3*i);
                        fields.push_back(onefield);
                    }
                    result = createTable(curuser,curDBname,sqlkey.at(2),fields,constraints);
                    switch(result)
                    {
                    case 1:
                        cout<<"Create table successfully!"<<endl;
                        break;
                    case -1:
                        break;
                    }
                }
                    break;
                case 10://ɾ����
                    result = dropTable(curuser,curDBname,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Drop table successfully!"<<endl;
                        break;
                    case -1:
                        break;
                    case -2:
                        break;
                    }
                    break;
                case 11://ɾ��һ��
                    result = deleteField(curuser,curDBname,sqlkey.at(0),sqlkey.at(1));
                    switch(result)
                    {
                    case 1:
                        cout<<"Delte a field successfully!"<<endl;
                        break;
                    case -1:
                        break;
                    case -2:
                        break;
                    }
                    break;
                case 12://����һ��
                    //TODO:�������������ݷŵ�Field��Constraint�ṹ������
                {
                    Field onefield;
                    //result = addField(curuser,curDBname,sqlkey.at(0));
                    switch(result)
                    {
                    case 1:
                        cout<<"Add a field successfully!"<<endl;
                        break;
                    }
                }
                    break;
                case -1:
                    cout<<"syntax error!"<<endl;
                    break;
				default:
					break;
				}
			}
        }
        else
        {
            cout<<"Acount or passeord is not valid,please reenter..."<<endl;
        }
    }

    return a.exec();
}
