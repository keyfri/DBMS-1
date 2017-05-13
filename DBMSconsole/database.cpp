#include "convert.h"
#include "database.h"

typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
}Date;

string getZeroNumberString(int number)
{
    string returnString = to_string(number);
    if(number>=0&&number<=9)
        return "0"+returnString;
    else
        return returnString;
}

QString getCurrTimeStr()
{
    time_t now = time(0);
    tm *ptm = localtime(&now);
    Date currDate;
    currDate.year = ptm->tm_year+1900;
    currDate.month = ptm->tm_mon+1;
    currDate.day = ptm->tm_mday;
    currDate.hour = ptm->tm_hour;
    currDate.minute = ptm->tm_min;
    currDate.second = ptm->tm_sec;
    string currDateStr = to_string(currDate.year) +"-"+ getZeroNumberString(currDate.month)+"-"+getZeroNumberString(currDate.day)+" "+getZeroNumberString(currDate.hour)+":"+getZeroNumberString(currDate.minute)+":"+getZeroNumberString(currDate.second);
    return stringtoqstring(currDateStr);
}

void showdatabases(QString user)//在屏幕上打印当前用户的所有数据库
{
    /*
     * return -1:the dir of user is not exist
     * return -3:open database.txt first time failed
     */
    QString onePath = "DBMS/"+user;
    QString dataBaseTxtPath = onePath+"/database.txt";
    QFile f1(dataBaseTxtPath);
    f1.open(QIODevice::ReadWrite|QIODevice::Text);
    QTextStream in (&f1);
    QString content = in.readAll();
    QStringList dbFileList = content.split("\n", QString::SkipEmptyParts);
    QString dbNumber = dbFileList[0];
    cout<<qstringtostring(user)<<" have "<<qstringtostring(dbNumber)<<" databases"<<endl;
    QString toFileQstring;
    for(int i=1;i<dbFileList.length();i++)
    {
        QStringList name;
        name = dbFileList[i].split("#", QString::SkipEmptyParts);
        qDebug()<<name[0];
    }

    f1.close();
}

/*
return 1:success
return -1:the database has existed
return -2:create dbDir failed
return -3:open database.txt firsttime failed
return -4:rewrite database.txt failed
*/
int createDBEntity(QString user,QString DBname)//创建数据库实体
{
    QString onePath = "DBMS/"+user;
    QString dbDirPath = onePath+"/"+DBname;
    QString dataBaseTxtPath = onePath+"/database.txt";
    QDir *tempQdirPoint = new QDir;
    if(tempQdirPoint->exists(dbDirPath))
        return -1;
    else
    {
        bool isCreate = tempQdirPoint->mkdir(dbDirPath);
        if(!isCreate)
            return -2;
        else
        {
            QFile f1(dataBaseTxtPath);
            if(!f1.open(QIODevice::ReadWrite|QIODevice::Text))
                return -3;
            QTextStream in (&f1);
            QString content = in.readAll();
            QStringList dbFileList = content.split("\n", QString::SkipEmptyParts);
            QString dbNumber = dbFileList[0];
            QString newdbNumber = inttoqstring((qstringtoint(dbNumber)+1));
            QString addDbInfo = DBname+"#"+getCurrTimeStr()+"#0#\n";
            QString toFileQstring = newdbNumber+"\n";
            for(int i=1;i<dbFileList.length();i++)
                toFileQstring = toFileQstring+dbFileList[i]+"\n";
            toFileQstring = toFileQstring + addDbInfo;
            f1.close();
            QFile f2(dataBaseTxtPath);
            if(!f2.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                return-4;
            QTextStream out (&f2);
            out<<toFileQstring;
            f2.close();
            return 1;
        }
    }
}

/*
 * return -1: the olddatabase not existed
 * return -2:the new database name has existed
 * return -3: open the database.txt fail
 * return -4: open the database.txt again fail
 */
int renameDBEntity(QString user,QString newName,QString oldName)//重命名数据库实体
{
    QString dbOldPath ="DBMS/"+user+"/"+oldName;
    QString dbNewPath ="DBMS/"+user+"/"+newName;
    QString dataBaseTxtPath ="DBMS/"+user+"/database.txt";
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(dbOldPath))
        return -1;
    else
    {
        if(tempQdirPoint->exists(dbNewPath))
            return -2;
        else
        {
            copyRecursively(dbOldPath,dbNewPath);
            deleteDir(dbOldPath);
            QString dataBaseTxtPath ="DBMS/"+user+"/database.txt";
            QFile f1(dataBaseTxtPath);
            if(!f1.open(QIODevice::ReadWrite|QIODevice::Text))
                return -3;
            QTextStream in (&f1);
            QString content = in.readAll();
            QStringList dbFileList = content.split("\n", QString::SkipEmptyParts);
            QStringList temp;
            QString tempFirst;
            QString dbNumber = dbFileList[0];
            QString toFileQstring = dbNumber+"\n";
            for(int i=1;i<dbFileList.length();i++)
            {
                temp = dbFileList[i].split("#",QString::SkipEmptyParts);
                tempFirst = temp[0];
                if(tempFirst==oldName)
                {
                    QString modifyRow = newName+"#";
                    if(temp[temp.length()-1]=="0"){

                        for(int i=1;i<temp.length();i++)
                            modifyRow = modifyRow + temp[i]+"#";
                    }
                    else
                    {
                        for(int i=1;i<temp.length()-1;i++)
                            modifyRow = modifyRow + temp[i]+"#";
                        modifyRow = modifyRow + temp[temp.length()-1];
                    }
                    modifyRow  = modifyRow +"\n";
                    toFileQstring = toFileQstring + modifyRow;
                }
                else
                    toFileQstring = toFileQstring+dbFileList[i]+"\n";
            }

            f1.close();
            QFile f2(dataBaseTxtPath);
            if(!f2.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                return -4;
            QTextStream out (&f2);
            out<<toFileQstring;
            f2.close();


        }
    }
    return 1;
}

/*
 * return -1:the db which to be dropped is not exist
 * return -2:delete the dir fail
 * return -3: open the database.txt fail
 * return -4: open the database.txt again fail
 *
 *
 */
int dropDBEntity(QString user,QString DBname)//删除数据库实体
{
    QString onePath = "DBMS/"+user;
    QString dbDirPath = onePath+"/"+DBname;
    QString dataBaseTxtPath = onePath+"/database.txt";
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(dbDirPath))
        return -1;
    else
    {
        int temp = deleteDir(dbDirPath);
        if(temp!=1)  return -2;
        else
        {
            QFile f1(dataBaseTxtPath);
            if(!f1.open(QIODevice::ReadWrite|QIODevice::Text))
                return -3;
            QTextStream in (&f1);
            QString content = in.readAll();
            QStringList dbFileList = content.split("\n", QString::SkipEmptyParts);
            QStringList temp;
            QString tempFirst;
            QString dbNumber = dbFileList[0];
            QString newDbNumber = inttoqstring(qstringtoint(dbNumber)-1);
            QString toFileQstring = newDbNumber+"\n";
            for(int i=1;i<dbFileList.length();i++)
            {
                temp = dbFileList[i].split("#",QString::SkipEmptyParts);
                tempFirst = temp[0];
                if(tempFirst==DBname)
                    continue;
                else
                    toFileQstring = toFileQstring+dbFileList[i]+"\n";
            }
            f1.close();
            QFile f2(dataBaseTxtPath);
            if(!f2.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
                return -4;
            QTextStream out (&f2);
            out<<toFileQstring;
            f2.close();

        }
    }
    return 1;
}

/*
 *return -1: the dbname not exist
 *return 1:  the dbname exist and let the curDBname = dbname
 */
int useDBEntity(QString user,QString DBname,QString &curDBname)//打开并使用数据库
{
    //如果存在，那么curDBname=DBname;
    QString onePath = "DBMS/"+user;
    QString dbDirPath = onePath+"/"+DBname;
    QDir *tempQdirPoint = new QDir;
    if(tempQdirPoint->exists(dbDirPath))
    {
        curDBname = DBname;
        return 1;
    }
    else
        return -1;
}

int initDBEntity(QString user,QString DBname)//初始化数据库
{
    if(dropDBEntity(user,DBname)!=1) return -1;//删除失败
    if(createDBEntity(user,DBname)!=1) return -2;//创建失败
    return 1;
}

/*
 * return -1:copy file failed
 * return -2：create new dir failed
 * return -3: the file which want to be backed up not exist
 *
 *
 */
int backupDBEntity(QString user,QString DBname)//备份数据库
{
    QString onePath = "DBMS/"+user;
    QString dbDirPath = onePath+"/"+DBname;
    QString dbNewDirPath = dbDirPath +"_B";
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(dbDirPath))
        return -3;
    else
    {
        int temp= copyRecursively(dbDirPath,dbNewDirPath);
        return temp;
    }
}

int recoverDBEntity(QString user,QString DBname)
{
    QString onePath = "DBMS/"+user;
    QString dbDirPath = onePath+"/"+DBname;
    QString dbNewDirPath = dbDirPath +"_B";
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(dbNewDirPath))
        return -3;
    else
    {
        int temp= copyRecursively(dbNewDirPath,dbDirPath);
        return temp;
    }
}
