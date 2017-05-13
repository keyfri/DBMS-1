#include "fileoperation.h"
#include <QDir>


/*
 return -1:copy file failed
 return -2：create new dir failed
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
  *只能删除文件夹 不能用来删除文件
  * 删除文件用QFile::remove("DBMS/doushuai/back.txt");
  *
 */
int deleteDir(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()){
        return -1;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            deleteDir(file.absoluteFilePath());
        }
    }
    dir.rmpath(dir.absolutePath());
    return 1; // 删除文件夹
}
