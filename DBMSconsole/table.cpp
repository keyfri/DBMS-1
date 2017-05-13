#include "table.h"
#include <string>
#include <QStringList>
#include <stdlib.h>
#include <windows.h>
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <conio.h>
#include <fstream>
#include <QDir>
#include <map>
#include "convert.h"

void clearFiles(const QString &folderFullPath)
{
    QDir dir(folderFullPath);
    dir.setFilter(QDir::Files);
    int fileCount = dir.count();
    for (int i = 0; i < fileCount; i++)
    dir.remove(dir[i]);
}

/*返回值错误列表（showtables）
 *-5 该数据库无法查找，不存在该数据库
 *-3 该数据库下无表
 *-2 没有数据库，请先创建数据库
 *-1 文件打开失败
 */
//显示所有表名
int showtables(QString user,QString DBname)//在屏幕上打印当前用户当前数据库下的所有表名
{
    int dbnum,tbnum;
    QString u;
    // 打开指定文件database.txt。其中存储有所有数据库信息和数据库中的表的信息
    QString dataBaseTxtPath;
    dataBaseTxtPath="DBMS/"+user+"/database.txt";//找到文件所在位置
    QFile inputFile1(dataBaseTxtPath);
    // 只读打开：
    if(!inputFile1.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        cout<<"打开database.txt文件失败！"<<endl;
        return -1;
    }

    //读取第一行，获取当前的数据库的数量
    QTextStream in1(&inputFile1);
    dbnum = in1.readLine().toInt();
    if(dbnum<=0){return -2;}//不存在数据库，无法显示表名
    QStringList  v;
    int find=0;
    for(int i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //进行解析，从#分开
        v=u.split('#');
        if(v[0]==DBname)//找到相应的数据库
        {
            find=1;
            break;
        }
    }
    if(find==0)return -5;

    //读取表的信息
    tbnum=v[2].toInt();
    QString p=v[3];//表信息字符串
    //解析表信息
    if(tbnum<=0)return -3;//该数据库下无表
    else{
        QStringList v2;
        v2=p.split('&');                                     //表名通过&分隔开
        for(vector<string>::size_type i = 0; i<v2.size(); ++i)
            cout <<qstringtostring(v2[i])<<endl;
    }
    inputFile1.close();
    return 1;
}

/*返回值错误列表（createTable）
 *-5 不存在该数据库
 *-4 该表已经存在
 *-3 表文件夹创建失败
 *-2 没有数据库，请先创建数据库
 *-1 文件打开失败
 */
//创建表
int createTable(QString user,QString DBname,QString TBname,vector<Field> fields,vector<Constraint> constraints)//创建表
{
    //文件路径定义
    int dbnum,tablenum;//数据库数量，表数量
    QString outFilename;//输出文件路径
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+TBname;
    QString dataBaseTxtPath = onePath+"/database.txt";
    QDir *tempQdirPoint = new QDir;//新建表的文件夹

    if(tempQdirPoint->exists(tbDirPath))//如果已经存在该表文件夹
        return -4;
    else{
        bool isCreate = tempQdirPoint->mkdir(tbDirPath);//创建表文件夹
        if(!isCreate)
            return -3;
        else{
            //往database.txt中写入表的信息
            QFile inputFile1(dataBaseTxtPath);
            if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
            {
                cout<<"打开database.txt文件失败！"<<endl;
                return -1;
            }
            QTextStream in1(&inputFile1);

            outFilename="DBMS/"+user+"/1.txt";
            QFile outFile1(outFilename);

            if(!outFile1.open(QIODevice::WriteOnly|QIODevice::Text))
            {
                cout<<"open file failed"<<endl;
                return -1;
            }
            QTextStream out1(&outFile1);


            //读取database.txt的第一行
            dbnum = in1.readLine().toInt();//数据库的数量
            out1<<dbnum<<endl;//写入文件中
            if(dbnum<=0)return -2;

            //找到对应的DBname，除了被修改行之前的全都被写入
            int i;int find=0;
            QStringList v;//行信息解析容器
            QString u;
            for(i=0;i<dbnum;i++)
            {
                u = in1.readLine();
                //进行解析，从#分开
                v=u.split('#');
                if(v[0]==DBname)//找到相应的数据库
                {
                    find=1;
                    break;
                }

                out1<<u<<endl;//如果不是要找的那行之前的都直接写入
            }

            if(find==0)return -5;

            //更新表数量
            tablenum=v[2].toInt()+1;//表数量
            QString n;//用于修改行表名信息前的信息的编辑
            n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum))+"#";
            //在该行最后加上表名
            if(tablenum==1)
                n=n+TBname;
            else
                n=n+v[3]+"&"+TBname;
            //写入修改后的行信息
            out1<<n<<endl;
            //写入剩下的信息
            for(int h=i+1;h<dbnum;h++)
            {
                u = in1.readLine();
                out1<<u<<endl;                                   //如果不是要找的那行之前的都直接写入
            }
            out1.flush();

            //重写入
            inputFile1.close();
            outFile1.close();
            inputFile1.remove(inputFile1.fileName());
            outFile1.rename(dataBaseTxtPath);

        }
        //往tdf.txt中加入字段信息
        QString tFilename=tbDirPath+"/tdf.txt";
        QFile outFile2(tFilename);
        if(!outFile2.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            cout<<"open file failed"<<endl;
            return -1;
        }
        QTextStream out2(&outFile2);

        //逐个逐行写入字段信息
        //第一行记录当前表中的字段数量
        out2<<fields.size()<<endl;
        //第二行起每行都记录一个字段的信息，不同信息间用#分割，信息包括字段名、类型
        for(vector<Field>::size_type i=0;i<fields.size();i++)
            out2<<fields[i].name<<"#"<<fields[i].type<<endl;
        out2.flush();
        outFile2.close();


        //往tic.txt中加入约束条件
        tFilename=tbDirPath+"/tic.txt";
        QFile outFile3(tFilename);
        if(!outFile3.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            cout<<"open file failed"<<endl;
            return -1;
        }
        QTextStream out3(&outFile3);

        //第一行中记录当前表中的约束条件的数量
        out3<<constraints.size()<<endl;
        //第二行起每行都记录一个约束条件的信息
        //格式：约束条件名#约束条件类型的整型编号#约束条件作用的列（多列用&分隔）#约束作用参考的表名（外键才有）#约束条件参考的列名（外键才有）
        for(vector<Constraint>::size_type i=0;i<constraints.size();i++){
            //约束条件名及约束条件类型的整型编号
            out3<<constraints[i].name<<"#"<<constraints[i].type;
            //约束条件作用行
            vector<QString> c=constraints[i].colnos;
            for(int h=0;h<c.size();h++){
                if(h==0)//第一个作用行
                    out3<<"#"<<c[h];
                else if(h>0){//后面几列用&分隔
                    out3<<"&"<<c[h];
                }
            }

            //一些特殊的约束条件的写法
            //如果有外键的话
            if(constraints[i].type==1)
                out3<<"#"<<constraints[i].outertable<<"#"<<constraints[i].outerprino;
            //如果是check条件的话
            else if(constraints[i].type==4){
                out3<<"#"<<constraints[i].checktype;
                if(constraints[i].checktype==0)//连续型范围
                    out3<<"#"<<constraints[i].min<<" "<<constraints[i].max;
                else if(constraints[i].checktype==1){//离散型范围
                    out3<<"#";
                    //离散型的取值集合
                    set<QString> a=constraints[i].valueset;
                    for(set<QString>::iterator setai=a.begin();setai!=a.end();setai++)
                        out3<<*setai<<" ";
                }
            }
            else if(constraints[i].type==5)
                out3<<"#"<<constraints[i].defaultvalue;
            out3<<endl;
            out3.flush();
        }
        outFile3.close();

    }
    return 1;
}

/*返回值错误列表（dropTable）
 *-6 不存在该表的文件夹
 *-5 不存在该数据库
 *-4 表的数量小于0，即不能进行删除表操作
 *-3 找不到要删除的表
 *-2 数据库数量为0
 *-1 文件打开失败
 */
//删除表
int dropTable(QString user,QString DBname,QString TBname)//删除表
{
    int dbnum,tablenum;
    QString outFilename;
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+TBname;
    QString dataBaseTxtPath = onePath+"/database.txt";

//删除database.txt里面的表名
    QFile inputFile1(dataBaseTxtPath);
    if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        cout<<"打开database.txt文件失败！"<<endl;
        return -1;
    }
    QTextStream in1(&inputFile1);

    outFilename="DBMS/"+user+"/1.txt";
    QFile outFile1(outFilename);
    //
    if(!outFile1.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        cout<<"open file failed"<<endl;
        return -1;
    }
    QTextStream out1(&outFile1);

    //读取database.txt的第一行
    dbnum = in1.readLine().toInt();//数据库的数量
    out1<<dbnum<<endl;//写入文件中
    if(dbnum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -2;
    }

    //找到对应的DBname，除了被修改行之前的全都被写入
    int i;int find=0;
    QStringList v;//行信息解析容器
    QString u;
    for(i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //进行解析，从#分开
        v=u.split('#');
        if(v[0]==DBname)//找到相应的数据库
        {
            find=1;
            break;
        }
        out1<<u<<endl;//如果不是要找的那行之前的都直接写入
    }
    if(find==0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -5;

    }

    //更新表数量
    tablenum=v[2].toInt();//表数量
    if(tablenum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -4;
    }
    QString n;//用于修改行表名信息前的信息的编辑
    n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum-1))+"#";

    //去掉要删除的表名
    QStringList t;
    t=v[3].split('&');
    QString tl;
    find=0;
    int h;//检查第一个是否是我们要找的表名
    for(h=0;h<t.size();h++){
        if(t[h]==TBname)
            find=1;
        else{
            if(t[0]==TBname&&h==1){//如果第一个就是要找到的表名
                tl=t[h];
            }else
                if(h==0)tl=t[h];
                else
                    tl=tl+"&"+t[h];
        }
    }
    if(find==0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -3;
    }
    n=n+tl;
    out1<<n<<endl;

    //写入剩下的信息
    for(int m=i+1;m<dbnum;m++)
    {
        u = in1.readLine();
        out1<<u<<endl;                                   //如果不是要找的那行之前的都直接写入
    }
    out1.flush();

    //重写入
    inputFile1.close();
    outFile1.close();
    inputFile1.remove(inputFile1.fileName());
    outFile1.rename(dataBaseTxtPath);

//删除相应的表的文件夹
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(tbDirPath))//如果不存在该表文件夹
        return -6;
    else{
        QString tabelFilename="DBMS/"+user+"/"+DBname+"/"+TBname;//表文件夹路径;
        deleteDir(tabelFilename);
    }

    return 1;
}

int deleteField(QString user,QString DBname,QString TBname,QString col)//删除表中一列
{
    QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//获得该表描述文件名
    //ifstream fin;
    QFile fileRead(tdffilename);
    if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //打开失败
        return 0;
       QTextStream infile_1(&fileRead);QString line;
       line = infile_1.readLine();//获得第一行的字段数并进行修改
          int colnum = line.toInt();//转为int类型
            colnum--;//字段数自增加1
             QFile fileTemp_4("temp.txt");
             if (!fileTemp_4.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
               return 0;//打开要写入的临时文件失败
                 QTextStream outfile(&fileTemp_4);
                 outfile << QString::number(colnum) <<"\n";  //写入字段个数
        line = infile_1.readLine();//开始读取字段描述行
            while (!line.isNull()) {
                QStringList myList = line.split("#");// 分割获得和这个字段关联的约束条件个数
            if((myList[2]).compare(col) != 0)//不相等
               outfile <<line<<"\n";
                line = infile_1.readLine();
            }

          fileRead.close();
          fileTemp_4.close();
           QFile::remove(tdffilename);//刪除文件
            //删除源文件
           if(!QFile::rename("temp.txt",tdffilename))
               return 0;
            //将temp文件名改为原文件的文件名
           //删除该字段对应的约束条件，包括主键
           QString ticfilename =user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//获得约束条件描述文件名
           int deletenum = 0; QStringList pkList;
            QFile fileRead8(ticfilename);
             QTextStream infile(&fileRead8);
             QString line5;
             line5 = infile.readLine();//获得第一行的约束条件数
                int constraintnum = line.toInt();//转为int类型
                 while (!line5.isNull()) {
                      QStringList myList = line5.split("#");// 分割获得和这个字段关联的约束条件个数
                       if((myList[1].compare("0") != 0) && (myList[2]).compare(col) == 0)//某约束条件不是主键并且该约束条件和要删除的字段相关联
                           {

                            deletenum ++;   //若不是待修改行，写入临时文件

                           }
                       if(myList[1].compare("0") == 0) //某约束条件是主键
                           {
                           pkList = myList[2].split("&");//该约束条件和要删除的字段相关联
                               if(pkList.indexOf(col)!= -1)//这个是主键
                            deletenum ++;   //删除约束条件
                           }
                       line5 = infile.readLine();
                   }
                  constraintnum -= deletenum;
                  fileRead8.close();
                  QFile fileRead5(ticfilename);//重新开始读取文件
                   QTextStream in(&fileRead5);QString line6;
                   line6 = in.readLine();//读取第一行
                   QFile fileTemp_5("temp.txt");//创建临时文件
                   if (!fileTemp_5.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
                     return 0;//打开要写入的临时文件失败
                       QTextStream out(&fileTemp_5);
                       out << QString::number(constraintnum) <<"\n";  //写入约束条件数
              line6 = in.readLine();//开始读取约束条件描述行
                  while (!line6.isNull()) {
                      QStringList myList = line6.split("#");// 分割获得和这个字段关联的约束条件个数
                       if((myList[1].compare("0") != 0) && (myList[2]).compare(col) == 0)//某约束条件不是主键并且该约束条件和要删除的字段相关联
                        continue;
                            if(myList[1].compare("0") == 0) //某约束条件是主键,判断这个field是不是主键的一员
                                pkList = myList[2].split("&");//该约束条件和要删除的字段相关联
                                    if(pkList.indexOf(col)!= -1)//这个是主键
                                continue;
                     out <<line<<"\n";
                      line6 = in.readLine();
                  }

                fileRead5.close();
                fileTemp_5.close();
                 QFile::remove(tdffilename);//刪除文件
                  //删除源文件
                 if(!QFile::rename("temp.txt",tdffilename))
                     return 0;
                 return 1;
}

int addField(QString user,QString DBname,QString TBname,Field col,Constraint constraint)//增加一列，只允许添加列一级约束条件
{
    //打开字段描述文件.tdf
    QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//获得该表描述文件名
    QString tempstring = col.name+"#"+col.type;//字段名#字段数据类型#字段长度#字段约束条件串#scope字符串#

    int count =constraint.colnos.size(); QString colinfo ;
    for (int i = 0; i < count;i++)
    {
        colinfo += constraint.colnos[i];
        if(i!= count-1)
        colinfo += "&";
     } //提取约束条件作用的列
    QFile fileRead(tdffilename);
    if(fileRead.exists())
        std::cout <<"要读的文件存在"<<endl;
    if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //打开失败
        return 0;
    QTextStream in(&fileRead);QString line;
    line = in.readLine();//获得第一行的字段数并进行修改
    int colnum = line.toInt();//转为int类型
    colnum++;//字段数自增加1
    QFile fileTemp_1("temp.txt");
    if (!fileTemp_1.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
        return 0;//打开要写入的临时文件失败
    QTextStream out(&fileTemp_1);
    out << QString::number(colnum) <<"\n";  //写入字段个数
    line = in.readLine();//开始读取字段描述行
    while (!line.isNull()) {
        out <<line<<"\n";
        line = in.readLine();
    }
    out <<tempstring<<"\n";
    fileRead.close();
    fileTemp_1.close();
    QFile::remove(tdffilename);//刪除文件
    //删除源文件
    if(!QFile::rename("temp.txt",tdffilename))
        return 0;
    //将temp文件名改为原文件的文件名，路径也要改
    //将该字段的约束条件更新到tic.txt中
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//获得约束条件描述文件名
    QFile fileWrite(ticfilename);
    if (!fileWrite.open(QIODevice::WriteOnly |QIODevice::Append| QIODevice::Text))
        return 0;//打开要写入的约束条件描述文件失败
    QTextStream out1(&fileWrite);
    out1<<constraint.name<<"#"<<QString::number(constraint.type)<<"#"<<colinfo;
    if(constraint.type == 1)
        out1<<"#"<<constraint.outertable<<"#"<<constraint.outerprino;
    out1<<"\n";
    fileWrite.close();
    return 1;
}//如果添加了非空约束条件
//将新增约束条件写进文件

int modifyCol(QString user,QString DBname,QString TBname,QString oldname,QString newname,QString type,Constraint constraint)//修改一列的名称及类型，只允许添加列一级约束条件
{
    //从头遍历字段描述文件，每用getline（）得到一行字段描述串，得到字符串之后对字符串按#解析得到vector向量，判断name是否符合
       //若符合在原文件中删除这个串并建立新的field结构体实例，调用addField（）函数写入文件尾）。
        QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//获得该表字段描述文件名
        QFile fileRead(tdffilename);
        if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //打开失败
            return 0;
           QTextStream in(&fileRead);QString line3;

           QFile fileTemp_2("temp.txt");
           if (!fileTemp_2.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
             return 0;//打开要写入的临时文件失败
               QTextStream out(&fileTemp_2);
           line3 = in.readLine();
           while (!line3.isNull()) {
               QStringList myList = line3.split("#");
                if((myList[0]).compare(oldname) != 0)
                    {

                      out <<line3<< "\n";   //若不是待修改行，写入临时文件

                    }
               else {//如果某行字段描述串为待改串则进行修改

                    Field tempfield;QString temptype;
                    tempfield.name = newname;
                    tempfield.type = type;
                    QString tempstring;//字段名#字段数据类型
                    tempstring+= tempfield.name;tempstring+= "#";//写字段名到字符串
                    tempstring+= temptype;//写数据类型到字符串
                    out <<tempstring<< "\n";  //将修改好的字符串写入文件
                }

           }

        fileTemp_2.close();
        fileRead.close();
        QFile::remove(tdffilename);//刪除文件
         //删除源文件
        if(!QFile::rename("temp.txt",tdffilename))
            return 0;
       //改约束条件描述文件
            QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//获得约束条件描述文件名
             QFile fileRead3(ticfilename);
               QString tempstring = constraint.name+ "#"+ QString::number(constraint.type)+ + "#"+newname;
               if(constraint.type == 1)//是外键
                   tempstring+=constraint.outertable; tempstring+=constraint.outerprino;
              QTextStream infile(&fileRead3);QString line;
              line = infile.readLine();//获得第一行的字段数并进行修改
                 int constraintnum = line.toInt();//转为int类型
                  constraintnum++;//约束条件数自增加1
                    QFile fileTemp_3("temp.txt");
                    if (!fileTemp_3.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
                      return 0;//打开要写入的临时文件失败
                        QTextStream outfile(&fileTemp_3);
                        outfile << QString::number(constraintnum) <<"\n";  //写入约束条件数
               line = infile.readLine();//开始读取约束条件描述行
                   while (!line.isNull()) {
                      outfile <<line<<"\n";
                       line = infile.readLine();
                   }
                 out <<tempstring<<"\n";
                 fileRead3.close();
                 fileTemp_3.close();
                  QFile::remove(tdffilename);//刪除文件
                   //删除源文件
                  if(!QFile::rename("temp.txt",tdffilename))
                      return 0;

        return 1;
}

/*返回值错误列表（renameTable）
 *-6 不存在该表的文件夹
 *-5 不存在该数据库
 *-4 表的数量小于0，即不能进行重命名表操作
 *-3 找不到要重命名的表
 *-2 数据库数量为0
 *-1 文件打开失败
 */

//重命名表
int renameTable(QString user,QString DBname,QString oldname,QString newname)//重命名表
{
    //修改database.txt里面表名
    int dbnum,tablenum;//数据库数量，表数量
    QString outFilename;//输出文件路径
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+oldname;
    QString dataBaseTxtPath = onePath+"/database.txt";

    QFile inputFile1(dataBaseTxtPath);
    if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        cout<<"打开database.txt文件失败！"<<endl;
        return -1;
    }
    QTextStream in1(&inputFile1);

    outFilename="DBMS/"+user+"/1.txt";
    QFile outFile1(outFilename);
    //
    if(!outFile1.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        cout<<"open file failed"<<endl;
        return -1;
    }
    QTextStream out1(&outFile1);

    //读取database.txt的第一行
    dbnum = in1.readLine().toInt();
    out1<<dbnum<<endl;
    if(dbnum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -2;
    }

    //找到对应的DBname，除了被修改行之前的全都被写入
    int i;int find=0;
    QStringList v;
    QString u;
    for(i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //进行解析，从#分开
        v=u.split('#');
        if(v[0]==DBname)
        {
            find=1;
            break;
        }
        out1<<u<<endl;
    }
    if(find==0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -5;
    }

    tablenum=v[2].toInt();
    if(tablenum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -4;
    }
    QString n;//用于修改行表名信息前的信息的编辑
    n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum))+"#";

    //找到相应的表名，然后进行替换
    QStringList t;
    t=v[3].split('&');
    QString tl;
    find=0;
    int h;//检查第一个是否是我们要找的表名
    for(h=0;h<t.size();h++){
        if(t[h]==oldname){
            find=1;
            if(h==0)
                tl=newname;
            else{
                tl=tl+"&"+newname;
            }
        }
        else{
            if(h==0)
                tl=t[0];
            else
                tl=tl+"&"+t[h];
        }
    }
    if(find==0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -3;
    }
    n=n+tl;
    out1<<n<<endl;

    //写入剩下的信息
    for(int m=i+1;m<dbnum;m++)
    {
        u = in1.readLine();
        out1<<u<<endl;                                   //如果不是要找的那行之前的都直接写入
    }
    out1.flush();

    //重写入
    inputFile1.close();
    outFile1.close();
    inputFile1.remove(inputFile1.fileName());
    outFile1.rename(dataBaseTxtPath);

    //修改目录下文件名
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(tbDirPath))//如果不存在该表文件夹
    {
        cout<<" NO tabLE"<<endl;
        return -6;
    }
    else{
        QString newfilename= onePath+"/"+DBname+"/"+newname;;//表文件夹路径;
        tempQdirPoint->rename(tbDirPath,newfilename);
    }
    return 1;
}

int removePri(QString user,QString DBname,QString TBname)//删除主键
{
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//获得该表描述文件名
    QFile fileRead_7(ticfilename);
    if (!fileRead_7.open(QIODevice::ReadOnly | QIODevice::Text))  //打开失败
        return 0;
    QTextStream in(&fileRead_7);QString line;
    line = in.readLine();//获得第一行的字段数并进行修改
       int constraintnum = line.toInt();//转为int类型
         constraintnum--;//字段数减1
          QFile fileTemp_6("temp.txt");
          if (!fileTemp_6.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
            return 0;//打开要写入的临时文件失败
     QTextStream out(&fileTemp_6);
              out << QString::number(constraintnum) <<"\n";  //写入字段个数
     line = in.readLine();//开始读取字段描述行
         while (!line.isNull()) {
             QStringList myList = line.split("#");// 分割获得和这个字段关联的约束条件个数
         if((myList[1]).compare("0") != 0)//不相等
            out <<line<<"\n";
             line = in.readLine();
         }
       fileRead_7.close();
       fileTemp_6.close();
       QFile::remove(ticfilename);//刪除文件
        //删除源文件
       if(!QFile::rename("temp.txt",ticfilename))
           return 0;
    return 1;
}

int addPri(QString user,QString DBname,QString TBname,Constraint primarykey)//为一到多列添加主键
{
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//获得约束条件描述文件名
    QFile fileRead_8(ticfilename);
    int count =primarykey.colnos.size(); QString pkcol ;
     for (int i = 0; i < count;i++)
     {
         pkcol += primarykey.colnos[i];
         if(i!= count-1)
        pkcol += "&";
     } //将主键作用的列提取出来生成固定格式的字符串

    QString tempString = primarykey.name+ primarykey.type+pkcol;
    if (!fileRead_8.open(QIODevice::ReadOnly | QIODevice::Text))  //打开失败
        return 0;
    QTextStream in(&fileRead_8);QString line;
    line = in.readLine();//获得第一行的字段数并进行修改
       int constraintnum = line.toInt();//转为int类型
         constraintnum++;//字段数减1
          QFile fileTemp_7("temp.txt");
          if (!fileTemp_7.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
            return 0;//打开要写入的临时文件失败
     QTextStream out(&fileTemp_7);
              out << QString::number(constraintnum) <<"\n";  //写入字段个数
     line = in.readLine();//开始读取字段描述行
         while (!line.isNull()) {
            out <<line<<"\n";
             line = in.readLine();
         }
          out <<tempString<<"\n";
       fileRead_8.close();
       fileTemp_7.close();
       QFile::remove(ticfilename);//刪除文件
        //删除源文件
       if(!QFile::rename("temp.txt",ticfilename))
           return 0;
    return 1;
}

string q2s(QString qstr){
    string s = qstr.toStdString();
    return s;
}

QString s2q(string s){
    QString qstr2 = QString::fromStdString(s);
    return qstr2;
}
