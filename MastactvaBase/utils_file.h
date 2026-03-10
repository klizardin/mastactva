#ifndef UTILS_FILE_H
#define UTILS_FILE_H

#include <QString>
#include <QStringList>
#include <QPair>
#include <QVector>

QVector<QStringList> getBunchOfImages(const QStringList& images_, int bunchSize);
QVector<QPair<QString, QString>> getImagesPairs(const QVector<QStringList>& bunchOfImages_, int bunchSize_);

#endif // UTILS_FILE_H
