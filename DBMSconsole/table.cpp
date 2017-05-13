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

/*����ֵ�����б�showtables��
 *-5 �����ݿ��޷����ң������ڸ����ݿ�
 *-3 �����ݿ����ޱ�
 *-2 û�����ݿ⣬���ȴ������ݿ�
 *-1 �ļ���ʧ��
 */
//��ʾ���б���
int showtables(QString user,QString DBname)//����Ļ�ϴ�ӡ��ǰ�û���ǰ���ݿ��µ����б���
{
    int dbnum,tbnum;
    QString u;
    // ��ָ���ļ�database.txt�����д洢���������ݿ���Ϣ�����ݿ��еı����Ϣ
    QString dataBaseTxtPath;
    dataBaseTxtPath="DBMS/"+user+"/database.txt";//�ҵ��ļ�����λ��
    QFile inputFile1(dataBaseTxtPath);
    // ֻ���򿪣�
    if(!inputFile1.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        cout<<"��database.txt�ļ�ʧ�ܣ�"<<endl;
        return -1;
    }

    //��ȡ��һ�У���ȡ��ǰ�����ݿ������
    QTextStream in1(&inputFile1);
    dbnum = in1.readLine().toInt();
    if(dbnum<=0){return -2;}//���������ݿ⣬�޷���ʾ����
    QStringList  v;
    int find=0;
    for(int i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //���н�������#�ֿ�
        v=u.split('#');
        if(v[0]==DBname)//�ҵ���Ӧ�����ݿ�
        {
            find=1;
            break;
        }
    }
    if(find==0)return -5;

    //��ȡ�����Ϣ
    tbnum=v[2].toInt();
    QString p=v[3];//����Ϣ�ַ���
    //��������Ϣ
    if(tbnum<=0)return -3;//�����ݿ����ޱ�
    else{
        QStringList v2;
        v2=p.split('&');                                     //����ͨ��&�ָ���
        for(vector<string>::size_type i = 0; i<v2.size(); ++i)
            cout <<qstringtostring(v2[i])<<endl;
    }
    inputFile1.close();
    return 1;
}

/*����ֵ�����б�createTable��
 *-5 �����ڸ����ݿ�
 *-4 �ñ��Ѿ�����
 *-3 ���ļ��д���ʧ��
 *-2 û�����ݿ⣬���ȴ������ݿ�
 *-1 �ļ���ʧ��
 */
//������
int createTable(QString user,QString DBname,QString TBname,vector<Field> fields,vector<Constraint> constraints)//������
{
    //�ļ�·������
    int dbnum,tablenum;//���ݿ�������������
    QString outFilename;//����ļ�·��
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+TBname;
    QString dataBaseTxtPath = onePath+"/database.txt";
    QDir *tempQdirPoint = new QDir;//�½�����ļ���

    if(tempQdirPoint->exists(tbDirPath))//����Ѿ����ڸñ��ļ���
        return -4;
    else{
        bool isCreate = tempQdirPoint->mkdir(tbDirPath);//�������ļ���
        if(!isCreate)
            return -3;
        else{
            //��database.txt��д������Ϣ
            QFile inputFile1(dataBaseTxtPath);
            if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
            {
                cout<<"��database.txt�ļ�ʧ�ܣ�"<<endl;
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


            //��ȡdatabase.txt�ĵ�һ��
            dbnum = in1.readLine().toInt();//���ݿ������
            out1<<dbnum<<endl;//д���ļ���
            if(dbnum<=0)return -2;

            //�ҵ���Ӧ��DBname�����˱��޸���֮ǰ��ȫ����д��
            int i;int find=0;
            QStringList v;//����Ϣ��������
            QString u;
            for(i=0;i<dbnum;i++)
            {
                u = in1.readLine();
                //���н�������#�ֿ�
                v=u.split('#');
                if(v[0]==DBname)//�ҵ���Ӧ�����ݿ�
                {
                    find=1;
                    break;
                }

                out1<<u<<endl;//�������Ҫ�ҵ�����֮ǰ�Ķ�ֱ��д��
            }

            if(find==0)return -5;

            //���±�����
            tablenum=v[2].toInt()+1;//������
            QString n;//�����޸��б�����Ϣǰ����Ϣ�ı༭
            n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum))+"#";
            //�ڸ��������ϱ���
            if(tablenum==1)
                n=n+TBname;
            else
                n=n+v[3]+"&"+TBname;
            //д���޸ĺ������Ϣ
            out1<<n<<endl;
            //д��ʣ�µ���Ϣ
            for(int h=i+1;h<dbnum;h++)
            {
                u = in1.readLine();
                out1<<u<<endl;                                   //�������Ҫ�ҵ�����֮ǰ�Ķ�ֱ��д��
            }
            out1.flush();

            //��д��
            inputFile1.close();
            outFile1.close();
            inputFile1.remove(inputFile1.fileName());
            outFile1.rename(dataBaseTxtPath);

        }
        //��tdf.txt�м����ֶ���Ϣ
        QString tFilename=tbDirPath+"/tdf.txt";
        QFile outFile2(tFilename);
        if(!outFile2.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            cout<<"open file failed"<<endl;
            return -1;
        }
        QTextStream out2(&outFile2);

        //�������д���ֶ���Ϣ
        //��һ�м�¼��ǰ���е��ֶ�����
        out2<<fields.size()<<endl;
        //�ڶ�����ÿ�ж���¼һ���ֶε���Ϣ����ͬ��Ϣ����#�ָ��Ϣ�����ֶ���������
        for(vector<Field>::size_type i=0;i<fields.size();i++)
            out2<<fields[i].name<<"#"<<fields[i].type<<endl;
        out2.flush();
        outFile2.close();


        //��tic.txt�м���Լ������
        tFilename=tbDirPath+"/tic.txt";
        QFile outFile3(tFilename);
        if(!outFile3.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            cout<<"open file failed"<<endl;
            return -1;
        }
        QTextStream out3(&outFile3);

        //��һ���м�¼��ǰ���е�Լ������������
        out3<<constraints.size()<<endl;
        //�ڶ�����ÿ�ж���¼һ��Լ����������Ϣ
        //��ʽ��Լ��������#Լ���������͵����ͱ��#Լ���������õ��У�������&�ָ���#Լ�����òο��ı�����������У�#Լ�������ο���������������У�
        for(vector<Constraint>::size_type i=0;i<constraints.size();i++){
            //Լ����������Լ���������͵����ͱ��
            out3<<constraints[i].name<<"#"<<constraints[i].type;
            //Լ������������
            vector<QString> c=constraints[i].colnos;
            for(int h=0;h<c.size();h++){
                if(h==0)//��һ��������
                    out3<<"#"<<c[h];
                else if(h>0){//���漸����&�ָ�
                    out3<<"&"<<c[h];
                }
            }

            //һЩ�����Լ��������д��
            //���������Ļ�
            if(constraints[i].type==1)
                out3<<"#"<<constraints[i].outertable<<"#"<<constraints[i].outerprino;
            //�����check�����Ļ�
            else if(constraints[i].type==4){
                out3<<"#"<<constraints[i].checktype;
                if(constraints[i].checktype==0)//�����ͷ�Χ
                    out3<<"#"<<constraints[i].min<<" "<<constraints[i].max;
                else if(constraints[i].checktype==1){//��ɢ�ͷ�Χ
                    out3<<"#";
                    //��ɢ�͵�ȡֵ����
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

/*����ֵ�����б�dropTable��
 *-6 �����ڸñ���ļ���
 *-5 �����ڸ����ݿ�
 *-4 �������С��0�������ܽ���ɾ�������
 *-3 �Ҳ���Ҫɾ���ı�
 *-2 ���ݿ�����Ϊ0
 *-1 �ļ���ʧ��
 */
//ɾ����
int dropTable(QString user,QString DBname,QString TBname)//ɾ����
{
    int dbnum,tablenum;
    QString outFilename;
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+TBname;
    QString dataBaseTxtPath = onePath+"/database.txt";

//ɾ��database.txt����ı���
    QFile inputFile1(dataBaseTxtPath);
    if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        cout<<"��database.txt�ļ�ʧ�ܣ�"<<endl;
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

    //��ȡdatabase.txt�ĵ�һ��
    dbnum = in1.readLine().toInt();//���ݿ������
    out1<<dbnum<<endl;//д���ļ���
    if(dbnum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -2;
    }

    //�ҵ���Ӧ��DBname�����˱��޸���֮ǰ��ȫ����д��
    int i;int find=0;
    QStringList v;//����Ϣ��������
    QString u;
    for(i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //���н�������#�ֿ�
        v=u.split('#');
        if(v[0]==DBname)//�ҵ���Ӧ�����ݿ�
        {
            find=1;
            break;
        }
        out1<<u<<endl;//�������Ҫ�ҵ�����֮ǰ�Ķ�ֱ��д��
    }
    if(find==0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -5;

    }

    //���±�����
    tablenum=v[2].toInt();//������
    if(tablenum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -4;
    }
    QString n;//�����޸��б�����Ϣǰ����Ϣ�ı༭
    n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum-1))+"#";

    //ȥ��Ҫɾ���ı���
    QStringList t;
    t=v[3].split('&');
    QString tl;
    find=0;
    int h;//����һ���Ƿ�������Ҫ�ҵı���
    for(h=0;h<t.size();h++){
        if(t[h]==TBname)
            find=1;
        else{
            if(t[0]==TBname&&h==1){//�����һ������Ҫ�ҵ��ı���
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

    //д��ʣ�µ���Ϣ
    for(int m=i+1;m<dbnum;m++)
    {
        u = in1.readLine();
        out1<<u<<endl;                                   //�������Ҫ�ҵ�����֮ǰ�Ķ�ֱ��д��
    }
    out1.flush();

    //��д��
    inputFile1.close();
    outFile1.close();
    inputFile1.remove(inputFile1.fileName());
    outFile1.rename(dataBaseTxtPath);

//ɾ����Ӧ�ı���ļ���
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(tbDirPath))//��������ڸñ��ļ���
        return -6;
    else{
        QString tabelFilename="DBMS/"+user+"/"+DBname+"/"+TBname;//���ļ���·��;
        deleteDir(tabelFilename);
    }

    return 1;
}

int deleteField(QString user,QString DBname,QString TBname,QString col)//ɾ������һ��
{
    QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//��øñ������ļ���
    //ifstream fin;
    QFile fileRead(tdffilename);
    if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //��ʧ��
        return 0;
       QTextStream infile_1(&fileRead);QString line;
       line = infile_1.readLine();//��õ�һ�е��ֶ����������޸�
          int colnum = line.toInt();//תΪint����
            colnum--;//�ֶ���������1
             QFile fileTemp_4("temp.txt");
             if (!fileTemp_4.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
               return 0;//��Ҫд�����ʱ�ļ�ʧ��
                 QTextStream outfile(&fileTemp_4);
                 outfile << QString::number(colnum) <<"\n";  //д���ֶθ���
        line = infile_1.readLine();//��ʼ��ȡ�ֶ�������
            while (!line.isNull()) {
                QStringList myList = line.split("#");// �ָ��ú�����ֶι�����Լ����������
            if((myList[2]).compare(col) != 0)//�����
               outfile <<line<<"\n";
                line = infile_1.readLine();
            }

          fileRead.close();
          fileTemp_4.close();
           QFile::remove(tdffilename);//�h���ļ�
            //ɾ��Դ�ļ�
           if(!QFile::rename("temp.txt",tdffilename))
               return 0;
            //��temp�ļ�����Ϊԭ�ļ����ļ���
           //ɾ�����ֶζ�Ӧ��Լ����������������
           QString ticfilename =user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//���Լ�����������ļ���
           int deletenum = 0; QStringList pkList;
            QFile fileRead8(ticfilename);
             QTextStream infile(&fileRead8);
             QString line5;
             line5 = infile.readLine();//��õ�һ�е�Լ��������
                int constraintnum = line.toInt();//תΪint����
                 while (!line5.isNull()) {
                      QStringList myList = line5.split("#");// �ָ��ú�����ֶι�����Լ����������
                       if((myList[1].compare("0") != 0) && (myList[2]).compare(col) == 0)//ĳԼ�����������������Ҹ�Լ��������Ҫɾ�����ֶ������
                           {

                            deletenum ++;   //�����Ǵ��޸��У�д����ʱ�ļ�

                           }
                       if(myList[1].compare("0") == 0) //ĳԼ������������
                           {
                           pkList = myList[2].split("&");//��Լ��������Ҫɾ�����ֶ������
                               if(pkList.indexOf(col)!= -1)//���������
                            deletenum ++;   //ɾ��Լ������
                           }
                       line5 = infile.readLine();
                   }
                  constraintnum -= deletenum;
                  fileRead8.close();
                  QFile fileRead5(ticfilename);//���¿�ʼ��ȡ�ļ�
                   QTextStream in(&fileRead5);QString line6;
                   line6 = in.readLine();//��ȡ��һ��
                   QFile fileTemp_5("temp.txt");//������ʱ�ļ�
                   if (!fileTemp_5.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
                     return 0;//��Ҫд�����ʱ�ļ�ʧ��
                       QTextStream out(&fileTemp_5);
                       out << QString::number(constraintnum) <<"\n";  //д��Լ��������
              line6 = in.readLine();//��ʼ��ȡԼ������������
                  while (!line6.isNull()) {
                      QStringList myList = line6.split("#");// �ָ��ú�����ֶι�����Լ����������
                       if((myList[1].compare("0") != 0) && (myList[2]).compare(col) == 0)//ĳԼ�����������������Ҹ�Լ��������Ҫɾ�����ֶ������
                        continue;
                            if(myList[1].compare("0") == 0) //ĳԼ������������,�ж����field�ǲ���������һԱ
                                pkList = myList[2].split("&");//��Լ��������Ҫɾ�����ֶ������
                                    if(pkList.indexOf(col)!= -1)//���������
                                continue;
                     out <<line<<"\n";
                      line6 = in.readLine();
                  }

                fileRead5.close();
                fileTemp_5.close();
                 QFile::remove(tdffilename);//�h���ļ�
                  //ɾ��Դ�ļ�
                 if(!QFile::rename("temp.txt",tdffilename))
                     return 0;
                 return 1;
}

int addField(QString user,QString DBname,QString TBname,Field col,Constraint constraint)//����һ�У�ֻ���������һ��Լ������
{
    //���ֶ������ļ�.tdf
    QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//��øñ������ļ���
    QString tempstring = col.name+"#"+col.type;//�ֶ���#�ֶ���������#�ֶγ���#�ֶ�Լ��������#scope�ַ���#

    int count =constraint.colnos.size(); QString colinfo ;
    for (int i = 0; i < count;i++)
    {
        colinfo += constraint.colnos[i];
        if(i!= count-1)
        colinfo += "&";
     } //��ȡԼ���������õ���
    QFile fileRead(tdffilename);
    if(fileRead.exists())
        std::cout <<"Ҫ�����ļ�����"<<endl;
    if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //��ʧ��
        return 0;
    QTextStream in(&fileRead);QString line;
    line = in.readLine();//��õ�һ�е��ֶ����������޸�
    int colnum = line.toInt();//תΪint����
    colnum++;//�ֶ���������1
    QFile fileTemp_1("temp.txt");
    if (!fileTemp_1.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
        return 0;//��Ҫд�����ʱ�ļ�ʧ��
    QTextStream out(&fileTemp_1);
    out << QString::number(colnum) <<"\n";  //д���ֶθ���
    line = in.readLine();//��ʼ��ȡ�ֶ�������
    while (!line.isNull()) {
        out <<line<<"\n";
        line = in.readLine();
    }
    out <<tempstring<<"\n";
    fileRead.close();
    fileTemp_1.close();
    QFile::remove(tdffilename);//�h���ļ�
    //ɾ��Դ�ļ�
    if(!QFile::rename("temp.txt",tdffilename))
        return 0;
    //��temp�ļ�����Ϊԭ�ļ����ļ�����·��ҲҪ��
    //�����ֶε�Լ���������µ�tic.txt��
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//���Լ�����������ļ���
    QFile fileWrite(ticfilename);
    if (!fileWrite.open(QIODevice::WriteOnly |QIODevice::Append| QIODevice::Text))
        return 0;//��Ҫд���Լ�����������ļ�ʧ��
    QTextStream out1(&fileWrite);
    out1<<constraint.name<<"#"<<QString::number(constraint.type)<<"#"<<colinfo;
    if(constraint.type == 1)
        out1<<"#"<<constraint.outertable<<"#"<<constraint.outerprino;
    out1<<"\n";
    fileWrite.close();
    return 1;
}//�������˷ǿ�Լ������
//������Լ������д���ļ�

int modifyCol(QString user,QString DBname,QString TBname,QString oldname,QString newname,QString type,Constraint constraint)//�޸�һ�е����Ƽ����ͣ�ֻ���������һ��Լ������
{
    //��ͷ�����ֶ������ļ���ÿ��getline�����õ�һ���ֶ����������õ��ַ���֮����ַ�����#�����õ�vector�������ж�name�Ƿ����
       //��������ԭ�ļ���ɾ��������������µ�field�ṹ��ʵ��������addField��������д���ļ�β����
        QString tdffilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tdf.txt";//��øñ��ֶ������ļ���
        QFile fileRead(tdffilename);
        if (!fileRead.open(QIODevice::ReadOnly | QIODevice::Text))  //��ʧ��
            return 0;
           QTextStream in(&fileRead);QString line3;

           QFile fileTemp_2("temp.txt");
           if (!fileTemp_2.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
             return 0;//��Ҫд�����ʱ�ļ�ʧ��
               QTextStream out(&fileTemp_2);
           line3 = in.readLine();
           while (!line3.isNull()) {
               QStringList myList = line3.split("#");
                if((myList[0]).compare(oldname) != 0)
                    {

                      out <<line3<< "\n";   //�����Ǵ��޸��У�д����ʱ�ļ�

                    }
               else {//���ĳ���ֶ�������Ϊ���Ĵ�������޸�

                    Field tempfield;QString temptype;
                    tempfield.name = newname;
                    tempfield.type = type;
                    QString tempstring;//�ֶ���#�ֶ���������
                    tempstring+= tempfield.name;tempstring+= "#";//д�ֶ������ַ���
                    tempstring+= temptype;//д�������͵��ַ���
                    out <<tempstring<< "\n";  //���޸ĺõ��ַ���д���ļ�
                }

           }

        fileTemp_2.close();
        fileRead.close();
        QFile::remove(tdffilename);//�h���ļ�
         //ɾ��Դ�ļ�
        if(!QFile::rename("temp.txt",tdffilename))
            return 0;
       //��Լ�����������ļ�
            QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//���Լ�����������ļ���
             QFile fileRead3(ticfilename);
               QString tempstring = constraint.name+ "#"+ QString::number(constraint.type)+ + "#"+newname;
               if(constraint.type == 1)//�����
                   tempstring+=constraint.outertable; tempstring+=constraint.outerprino;
              QTextStream infile(&fileRead3);QString line;
              line = infile.readLine();//��õ�һ�е��ֶ����������޸�
                 int constraintnum = line.toInt();//תΪint����
                  constraintnum++;//Լ��������������1
                    QFile fileTemp_3("temp.txt");
                    if (!fileTemp_3.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
                      return 0;//��Ҫд�����ʱ�ļ�ʧ��
                        QTextStream outfile(&fileTemp_3);
                        outfile << QString::number(constraintnum) <<"\n";  //д��Լ��������
               line = infile.readLine();//��ʼ��ȡԼ������������
                   while (!line.isNull()) {
                      outfile <<line<<"\n";
                       line = infile.readLine();
                   }
                 out <<tempstring<<"\n";
                 fileRead3.close();
                 fileTemp_3.close();
                  QFile::remove(tdffilename);//�h���ļ�
                   //ɾ��Դ�ļ�
                  if(!QFile::rename("temp.txt",tdffilename))
                      return 0;

        return 1;
}

/*����ֵ�����б�renameTable��
 *-6 �����ڸñ���ļ���
 *-5 �����ڸ����ݿ�
 *-4 �������С��0�������ܽ��������������
 *-3 �Ҳ���Ҫ�������ı�
 *-2 ���ݿ�����Ϊ0
 *-1 �ļ���ʧ��
 */

//��������
int renameTable(QString user,QString DBname,QString oldname,QString newname)//��������
{
    //�޸�database.txt�������
    int dbnum,tablenum;//���ݿ�������������
    QString outFilename;//����ļ�·��
    QString onePath = "DBMS/"+user;
    QString tbDirPath = onePath+"/"+DBname+"/"+oldname;
    QString dataBaseTxtPath = onePath+"/database.txt";

    QFile inputFile1(dataBaseTxtPath);
    if(!inputFile1.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        cout<<"��database.txt�ļ�ʧ�ܣ�"<<endl;
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

    //��ȡdatabase.txt�ĵ�һ��
    dbnum = in1.readLine().toInt();
    out1<<dbnum<<endl;
    if(dbnum<=0){
        outFile1.close();
        outFile1.remove(outFile1.fileName());
        return -2;
    }

    //�ҵ���Ӧ��DBname�����˱��޸���֮ǰ��ȫ����д��
    int i;int find=0;
    QStringList v;
    QString u;
    for(i=0;i<dbnum;i++)
    {
        u = in1.readLine();
        //���н�������#�ֿ�
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
    QString n;//�����޸��б�����Ϣǰ����Ϣ�ı༭
    n=v[0]+"#"+v[1]+"#"+stringtoqstring(inttostring(tablenum))+"#";

    //�ҵ���Ӧ�ı�����Ȼ������滻
    QStringList t;
    t=v[3].split('&');
    QString tl;
    find=0;
    int h;//����һ���Ƿ�������Ҫ�ҵı���
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

    //д��ʣ�µ���Ϣ
    for(int m=i+1;m<dbnum;m++)
    {
        u = in1.readLine();
        out1<<u<<endl;                                   //�������Ҫ�ҵ�����֮ǰ�Ķ�ֱ��д��
    }
    out1.flush();

    //��д��
    inputFile1.close();
    outFile1.close();
    inputFile1.remove(inputFile1.fileName());
    outFile1.rename(dataBaseTxtPath);

    //�޸�Ŀ¼���ļ���
    QDir *tempQdirPoint = new QDir;
    if(!tempQdirPoint->exists(tbDirPath))//��������ڸñ��ļ���
    {
        cout<<" NO tabLE"<<endl;
        return -6;
    }
    else{
        QString newfilename= onePath+"/"+DBname+"/"+newname;;//���ļ���·��;
        tempQdirPoint->rename(tbDirPath,newfilename);
    }
    return 1;
}

int removePri(QString user,QString DBname,QString TBname)//ɾ������
{
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//��øñ������ļ���
    QFile fileRead_7(ticfilename);
    if (!fileRead_7.open(QIODevice::ReadOnly | QIODevice::Text))  //��ʧ��
        return 0;
    QTextStream in(&fileRead_7);QString line;
    line = in.readLine();//��õ�һ�е��ֶ����������޸�
       int constraintnum = line.toInt();//תΪint����
         constraintnum--;//�ֶ�����1
          QFile fileTemp_6("temp.txt");
          if (!fileTemp_6.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
            return 0;//��Ҫд�����ʱ�ļ�ʧ��
     QTextStream out(&fileTemp_6);
              out << QString::number(constraintnum) <<"\n";  //д���ֶθ���
     line = in.readLine();//��ʼ��ȡ�ֶ�������
         while (!line.isNull()) {
             QStringList myList = line.split("#");// �ָ��ú�����ֶι�����Լ����������
         if((myList[1]).compare("0") != 0)//�����
            out <<line<<"\n";
             line = in.readLine();
         }
       fileRead_7.close();
       fileTemp_6.close();
       QFile::remove(ticfilename);//�h���ļ�
        //ɾ��Դ�ļ�
       if(!QFile::rename("temp.txt",ticfilename))
           return 0;
    return 1;
}

int addPri(QString user,QString DBname,QString TBname,Constraint primarykey)//Ϊһ�������������
{
    QString ticfilename ="DBMS/"+user+ "/"+DBname+ "/"+ TBname + "/tic.txt";//���Լ�����������ļ���
    QFile fileRead_8(ticfilename);
    int count =primarykey.colnos.size(); QString pkcol ;
     for (int i = 0; i < count;i++)
     {
         pkcol += primarykey.colnos[i];
         if(i!= count-1)
        pkcol += "&";
     } //���������õ�����ȡ�������ɹ̶���ʽ���ַ���

    QString tempString = primarykey.name+ primarykey.type+pkcol;
    if (!fileRead_8.open(QIODevice::ReadOnly | QIODevice::Text))  //��ʧ��
        return 0;
    QTextStream in(&fileRead_8);QString line;
    line = in.readLine();//��õ�һ�е��ֶ����������޸�
       int constraintnum = line.toInt();//תΪint����
         constraintnum++;//�ֶ�����1
          QFile fileTemp_7("temp.txt");
          if (!fileTemp_7.open(QIODevice::WriteOnly | QIODevice::Append| QIODevice::Text))
            return 0;//��Ҫд�����ʱ�ļ�ʧ��
     QTextStream out(&fileTemp_7);
              out << QString::number(constraintnum) <<"\n";  //д���ֶθ���
     line = in.readLine();//��ʼ��ȡ�ֶ�������
         while (!line.isNull()) {
            out <<line<<"\n";
             line = in.readLine();
         }
          out <<tempString<<"\n";
       fileRead_8.close();
       fileTemp_7.close();
       QFile::remove(ticfilename);//�h���ļ�
        //ɾ��Դ�ļ�
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
