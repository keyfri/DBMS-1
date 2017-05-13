#include "sqlanalysis.h"
#include <typeinfo>
#include <iostream>
//sql���������ִ�Сд���˴����ܵ�sql����Ѿ�ת��ΪСд
#define CREATE_TABLE_EXP 9              //��������id
#define RESERVE_SPACE 5                 //����λ�Ĵ�С

/*
 * �����������
 * @param ����sql�ؼ��ֵ�vector
 */
int createTableAna(vector<QString> &sqlkey)
{
    int fieldNum = 0;                   //�ֶθ���
    int extraExp = 0;                   //�Ƿ��ж����Լ�����������������������
    int pos = 0;
    int oldPos = 0;
    QString content = sqlkey.back();    //ȡ��������ֶε�����
    sqlkey.pop_back();                  //��vector��ɾ��

    QRegExp field1Reg(QString("(?:\\s*)(\\b[a-z0-9_]+\\b)"          //�ֶ���
                              "(?:\\s*)(integer|double|float|date|bool|varchar)"    //��������
                              "([a-z 0-9]*),"));            //Լ������  ��","��β
    QRegExp field2Reg(QString("(?:\\s*)(\\b[a-z0-9_]+\\b)"          //�ֶ���
                              "(?:\\s*)(integer|double|float|date|bool|varchar)"    //��������
                              "([a-z 0-9]*)"));             //Լ������ �޽�β
    QRegExp extraReg(QString("(?:\\s*)primary(?:\\s*)key"
                             "(?:\\s*)\\("
                             "([a-z 0-9,]*)"));                //�����������ֶ���

    while ((pos = field1Reg.indexIn(content, pos)) != -1)
    {
        fieldNum ++;
        for (int i = 0; i < field1Reg.captureCount(); i++)
        {
            sqlkey.push_back(field1Reg.cap(i + 1));
        }
        pos += field1Reg.matchedLength();
        oldPos = pos;
    }
    if (field2Reg.indexIn(content, oldPos) != -1)
    {
        fieldNum++;
        for (int i = 0; i < field2Reg.captureCount(); i++)
        {
            sqlkey.push_back(field2Reg.cap(i + 1));
        }
    }
    else if (extraReg.indexIn(content, oldPos) != -1)
    {
        extraExp = 1;
        for (int i = 0; i < extraReg.captureCount(); i++)
        {
            sqlkey.push_back(extraReg.cap(i + 1));
        }
    }
    sqlkey.insert(sqlkey.begin(), QString::number(extraExp));
    sqlkey.insert(sqlkey.begin(), QString::number(fieldNum));

    return field2Reg.indexIn(content);                      //����У��sql���
}
int sqlAnalysis(QString sql, vector<QString> &sqlkey)
{
    vector<QString> regVector;          //���������ʽ

    //create�������ݿ������ create database ���ݿ������������֣���ĸ���»��ߣ�                          0
    regVector.push_back(QString("(?:\\s*)create(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //rename���������ݿ������ rename database ���� to ����                                         1
    regVector.push_back(QString("(?:\\s*)rename(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)to(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));     //ƥ�����������ݿ������

    //dropɾ�����ݿ������ drop database ���ݿ���                                                 2
    regVector.push_back(QString("(?:\\s*)drop(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //useʹ�����ݿ������ use database ���ݿ���                                                  3
    regVector.push_back(QString("(?:\\s*)use(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //show��ʾ���ݿ������ show databases                                                       4
    regVector.push_back(QString("(?:\\s*)show(?:\\s*)databases(?:\\s*);"));

    //initialize��ʼ�����ݿ������ initialize database ���ݿ���                                      5
    regVector.push_back(QString("(?:\\s*)initialize(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //regVector.push_back(QString("(?:\\s*)"));

    //backup�������ݿ������ backup database ���ݿ���                                           6
    regVector.push_back(QString("(?:\\s*)backup(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //restore�ָ����ݿ������ restore database ���ݿ���                                         7
    regVector.push_back(QString("(?:\\s*)restore(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //��ʾ��ǰ���ݿ������б�����show tables;                                                     8
    regVector.push_back(QString("(?:\\s*)show(?:\\s*)tables(?:\\s*);"));

    //������ create table lll(rr int 666,                                                     9
    regVector.push_back(QString(
        "(?:\\s*)create(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)"     // create table test
        "\\("                                                               // (
        "(.+)"                                                     //��������������ȡ����׼����һ������
        "\\);"));

    //ɾ����10
    regVector.push_back(QString("(?:\\s*)drop(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //ɾ������һ��11
    regVector.push_back(QString("(?:\\s*)alter(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)drop(?:\\s*)column(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //����һ��12
    regVector.push_back(QString("(?:\\s*)alter(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)add(?:\\s*)column(?:\\s*)(\\b[a-z0-9_]+\\b)(.*);"));


    //��ʼ����sql���
    for (unsigned int i = 0; i < regVector.size(); i++)
    {
        int result = 0;
        QRegExp reg(regVector[i]);
        reg.setMinimal(false);
        //reg.setPatternSyntax(QRegExp::RegExp2);
        reg.setCaseSensitivity(Qt::CaseInsensitive);    //���ò����ִ�Сд
        //bool match = reg.exactMatch(sql);               //�Ƿ��������ȫƥ��
        int pos = reg.indexIn(sql);       //ƥ��ĵ�һ���ַ���λ�ã���0��ʼ����Ϊ-1��ƥ��

        if (pos >= 0)                      //��ƥ��
        {
            for (int j = 0; j < reg.captureCount(); j++)
            {
                sqlkey.push_back(reg.cap(j + 1));   //����Ӧ��ֵ����sqlkey
            }
            if (i == CREATE_TABLE_EXP)              //����ǽ������
            {
                result = createTableAna(sqlkey);
            }
            if (result != -1)
                return i;                    //�������ݿ������Ӧ�ı�ţ���0��ʼ
            else
                return -1;
        }
    }

    return -1;                              //û�ж�Ӧ�Ĳ���������-1����ʾ�����Ͳ�����δ֧�֣�����sql������
}
