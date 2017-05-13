#ifndef FILEOPERATION_H
#define FILEOPERATION_H
#include <QCoreApplication>

int copyRecursively(const QString &srcFilePath,const QString &tgtFilePath);
int deleteDir(const QString &path);

#endif // FILEOPERATION_H
