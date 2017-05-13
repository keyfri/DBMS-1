#include "fileoperation.h"
#include <QDir>


/*
 return -1:copy file failed
 return -2��create new dir failed
 */
int copyRecursively(const QString &srcFilePath,
                    const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
            return -2;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            int temp = copyRecursively(newSrcFilePath, newTgtFilePath);
            if (temp!=1)
                return temp;
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return -1;
    }
    return 1;
}

/*
  * return -1: the directory which wanted to be deleted not exist
  *ֻ��ɾ���ļ��� ��������ɾ���ļ�
  * ɾ���ļ���QFile::remove("DBMS/doushuai/back.txt");
  *
 */
int deleteDir(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()){
        return -1;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //���ù���
    QFileInfoList fileList = dir.entryInfoList(); // ��ȡ���е��ļ���Ϣ
    foreach (QFileInfo file, fileList){ //�����ļ���Ϣ
        if (file.isFile()){ // ���ļ���ɾ��
            file.dir().remove(file.fileName());
        }else{ // �ݹ�ɾ��
            deleteDir(file.absoluteFilePath());
        }
    }
    dir.rmpath(dir.absolutePath());
    return 1; // ɾ���ļ���
}
