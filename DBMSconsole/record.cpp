#include "record.h"
#include <QFile>
#include <QTextStream>
#include "convert.h"
int insertRecord(QString user,QString DBname,QString TBname,vector<map<QString,QString>> records)//�����¼
{
    QString dir = "DBMS/"+user+"/"+DBname+"/"+TBname+"/";
    QString dir1 = "DBMS/"+user+"/database.txt";
    QFile file1(dir+"tdf.txt");
    QFile file2(dir+"tic.txt");
    QFile file3(dir+"trd.txt");
    QFile file4(dir1);
    file1.open(QIODevice::ReadOnly|QIODevice::Text);
    file2.open(QIODevice::ReadOnly|QIODevice::Text);
    file3.open(QIODevice::ReadOnly|QIODevice::Text);
    file4.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in1(&file1);
    QTextStream in2(&file2);
    QTextStream in3(&file3);
    QTextStream in4(&file4);
    QString content1 = in1.readAll();
    QString content2 = in2.readAll();
    QString content3 = in3.readAll();
    QString content4 = in4.readAll();
    file1.close();
    file2.close();
    file3.close();
    file4.close();

    //��ȡ�ֶ���Ϣ������¼д����Ӧλ��
    QStringList tdfFileList = content1.split("\n");
    QStringList ticFileList = content2.split("\n");
    QStringList trdFileList = content3.split("\n");
    QStringList dbFileList = content4.split("\n");
    int fieldNum = qstringtoint(tdfFileList[0]);
    QString insertContent = "";
    QStringList field;

    for(int i=1;i<tdfFileList.length();i++)
    {
        field = tdfFileList[i].split("#");
        QString fieldName = field[0];
        QString fieldType = field[1];
        for(int j=0;j<records.size();j++)
        {
            map<QString,QString>::iterator key = records.at(j).find(fieldName);
            if(key!=records.at(j).end())//���в���ֵ
            {
                for(int k=0;k<ticFileList.length();k++)
                {
                    QStringList l = ticFileList.at(k).split("#");
                    if((l.at(1)=="0"||l.at(1)=="2")&&l.at(2).contains(fieldName)==1)//���Ψһ��Ҳ������������
                    {
                        if(!checkUnique(fieldName,content1))//����2��Υ��ΨһԼ��
                            return -2;
                    }
                    else if(l.at(1)=="1"&&l.at(2)==fieldName)//����3���ο��������޴�ֵ
                    {
                        if(!checkExist((records.at(j))[fieldName],l.at(3),l.at(4))) return -3;
                    }
                    else if(l.at(1)=="4"&&l.at(2)==fieldName)//
                    {

                    }
                }
            }
            else//���в�����ֵ
            {
                for(int k=0;k<ticFileList.length();k++)
                {
                    QStringList l = ticFileList.at(k).split("#");
                    if((l.at(1)=="0"||l.at(1)=="3")&&l.at(2).contains(fieldName)==1)//����1.�ǿգ�Ҳ������������
                    {
                        return -1;
                    }
                    else if(l.at(1)=="5"&&l.at(2)==fieldName)//Ĭ��ȡֵ
                    {
                        records.at(j).insert(pair<QString,QString>(fieldName,l.at(3)));
                    }
                    else
                    {
                        records.at(j).insert(pair<QString,QString>(fieldName,""));
                    }
                }
            }
        }
    }

    insertContent+=(inttoqstring(fieldNum+1)+"\n");
    for(int i=1;i<trdFileList.length();i++)
    {
        insertContent+=(trdFileList[i]+"\n");
    }
    for(int j=0;j<records.size();j++)
    {

        insertContent+=((records.at(j))[field[0]]+"#");
    }
    insertContent+="\n";

    file1.close();
    file2.close();
    file3.close();
    file4.close();

    return 1;
}

int updateRecord(QString user,QString DBname,QString TBname,vector<map<QString,QString>> colnames)//���¼�¼
{
    QString dir = "DBMS/"+user+"/"+DBname+"/"+TBname+"/";
    QString dir1 = "DBMS/"+user+"/database.txt";
    QFile file1(dir+"tdf.txt");
    QFile file2(dir+"tic.txt");
    QFile file3(dir+"trd.txt");
    QFile file4(dir1);
    file1.open(QIODevice::ReadOnly|QIODevice::Text);
    file2.open(QIODevice::ReadOnly|QIODevice::Text);
    file3.open(QIODevice::ReadOnly|QIODevice::Text);
    file4.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in1(&file1);
    QTextStream in2(&file2);
    QTextStream in3(&file3);
    QTextStream in4(&file4);
    QString content1 = in1.readAll();
    QString content2 = in2.readAll();
    QString content3 = in3.readAll();
    QString content4 = in4.readAll();

    //��ȡ�ֶ���Ϣ������¼д����Ӧλ��
    QStringList tdfFileList = content1.split("\n", QString::SkipEmptyParts);
    QStringList ticFileList = content2.split("\n", QString::SkipEmptyParts);
    QStringList trdFileList = content3.split("\n", QString::SkipEmptyParts);
    QStringList dbFileList = content4.split("\n", QString::SkipEmptyParts);
    int fieldNum = qstringtoint(tdfFileList[0]);
    QString insertContent = "";

    insertContent+=(trdFileList[0]+"\n");
    for(int i=1;i<trdFileList.length();i++)
    {
        QStringList record = trdFileList[i].split("#", QString::SkipEmptyParts);

        insertContent+=(trdFileList[i]+"\n");
    }
    //TODO:��һ���ּ��ϴ��������ж�
    //for(int i=1;i<dbFileList1.length();i++)
    //{
        //QStringList fielddf = tdfFileList[i].split("#", QString::SkipEmptyParts);
//        for(int j=0;j<records.size();j++)
//        {
//            insertContent+=((records.at(j))[fielddf[0]]+"#");
//        }
//        insertContent+="\n";

    //}


    file1.close();
    file2.close();
    file3.close();
    file4.close();
    return 1;
}

int selectRecord(QString user,QString DBname,vector<QString> TBnames,vector<QString> colname,vector<int> rdno)//��ѯ��¼���������Ϊ�����ͼ�¼�����
{
    return 1;
}

int deleteRecord(QString user,QString DBname,QString TBname,int rdno)//ɾ����¼���������Ϊ��¼�����
{

    return 1;
}

vector<int> analyzeWhere(QString condition)//�������Ϊwhereɸѡ����������ֵΪ��¼��ŵļ���
{

}

vector<int> analyzeGroupby(QString condition)//�������Ϊgroupbyɸѡ����������ֵΪ��¼��ŵļ���
{

}

bool checkUnique(QString colname,QString content)
{
    QStringList fields = content.split("\n");
    int i;
    for(i=1;i<fields.length();i++)
    {
        QStringList field = fields[i].split("#");
        if(colname == field[0])
        {
            return false;
        }
    }
    if(i == fields.length()) return true;
    return true;
}

bool checkExist(QString value,QString TBname,QString colname)
{

}
