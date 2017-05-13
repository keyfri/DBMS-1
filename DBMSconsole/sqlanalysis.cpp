#include "sqlanalysis.h"
#include <typeinfo>
#include <iostream>
//sql语句均不区分大小写，此处接受的sql语句已经转换为小写
#define CREATE_TABLE_EXP 9              //建表语句的id
#define RESERVE_SPACE 5                 //保留位的大小

/*
 * 解析建表语句
 * @param 放置sql关键字的vector
 */
int createTableAna(vector<QString> &sqlkey)
{
    int fieldNum = 0;                   //字段个数
    int extraExp = 0;                   //是否有额外的约束条件，如加在最后的主键等
    int pos = 0;
    int oldPos = 0;
    QString content = sqlkey.back();    //取出建表的字段等内容
    sqlkey.pop_back();                  //从vector中删除

    QRegExp field1Reg(QString("(?:\\s*)(\\b[a-z0-9_]+\\b)"          //字段名
                              "(?:\\s*)(integer|double|float|date|bool|varchar)"    //数据类型
                              "([a-z 0-9]*),"));            //约束条件  以","结尾
    QRegExp field2Reg(QString("(?:\\s*)(\\b[a-z0-9_]+\\b)"          //字段名
                              "(?:\\s*)(integer|double|float|date|bool|varchar)"    //数据类型
                              "([a-z 0-9]*)"));             //约束条件 无结尾
    QRegExp extraReg(QString("(?:\\s*)primary(?:\\s*)key"
                             "(?:\\s*)\\("
                             "([a-z 0-9,]*)"));                //额外主键的字段名

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

    return field2Reg.indexIn(content);                      //部分校验sql语句
}
int sqlAnalysis(QString sql, vector<QString> &sqlkey)
{
    vector<QString> regVector;          //存放正则表达式

    //create创建数据库的正则 create database 数据库名（允许数字，字母，下划线）                          0
    regVector.push_back(QString("(?:\\s*)create(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //rename重命名数据库的正则 rename database 旧名 to 新名                                         1
    regVector.push_back(QString("(?:\\s*)rename(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)to(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));     //匹配重命名数据库的正则

    //drop删除数据库的正则 drop database 数据库名                                                 2
    regVector.push_back(QString("(?:\\s*)drop(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //use使用数据库的正则 use database 数据库名                                                  3
    regVector.push_back(QString("(?:\\s*)use(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //show显示数据库的正则 show databases                                                       4
    regVector.push_back(QString("(?:\\s*)show(?:\\s*)databases(?:\\s*);"));

    //initialize初始化数据库的正则 initialize database 数据库名                                      5
    regVector.push_back(QString("(?:\\s*)initialize(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //regVector.push_back(QString("(?:\\s*)"));

    //backup备份数据库的正则 backup database 数据库名                                           6
    regVector.push_back(QString("(?:\\s*)backup(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //restore恢复数据库的正则 restore database 数据库名                                         7
    regVector.push_back(QString("(?:\\s*)restore(?:\\s*)database(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //显示当前数据库下所有表名：show tables;                                                     8
    regVector.push_back(QString("(?:\\s*)show(?:\\s*)tables(?:\\s*);"));

    //创建表 create table lll(rr int 666,                                                     9
    regVector.push_back(QString(
        "(?:\\s*)create(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)"     // create table test
        "\\("                                                               // (
        "(.+)"                                                     //将括号中内容提取出来准备进一步处理
        "\\);"));

    //删除表10
    regVector.push_back(QString("(?:\\s*)drop(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //删除表中一列11
    regVector.push_back(QString("(?:\\s*)alter(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)drop(?:\\s*)column(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*);"));

    //增加一列12
    regVector.push_back(QString("(?:\\s*)alter(?:\\s*)table(?:\\s*)(\\b[a-z0-9_]+\\b)(?:\\s*)add(?:\\s*)column(?:\\s*)(\\b[a-z0-9_]+\\b)(.*);"));


    //开始解析sql语句
    for (unsigned int i = 0; i < regVector.size(); i++)
    {
        int result = 0;
        QRegExp reg(regVector[i]);
        reg.setMinimal(false);
        //reg.setPatternSyntax(QRegExp::RegExp2);
        reg.setCaseSensitivity(Qt::CaseInsensitive);    //设置不区分大小写
        //bool match = reg.exactMatch(sql);               //是否和正则完全匹配
        int pos = reg.indexIn(sql);       //匹配的第一个字符的位置，从0开始，若为-1则不匹配

        if (pos >= 0)                      //若匹配
        {
            for (int j = 0; j < reg.captureCount(); j++)
            {
                sqlkey.push_back(reg.cap(j + 1));   //将对应的值放入sqlkey
            }
            if (i == CREATE_TABLE_EXP)              //如果是建表语句
            {
                result = createTableAna(sqlkey);
            }
            if (result != -1)
                return i;                    //返回数据库操作对应的编号，从0开始
            else
                return -1;
        }
    }

    return -1;                              //没有对应的操作，返回-1，表示该类型操作暂未支持，或者sql语句错误
}
