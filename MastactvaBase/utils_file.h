#ifndef UTILS_FILE_H
#define UTILS_FILE_H

#include <QString>
#include <QStringList>
#include <QPair>
#include <QVector>
#include <QVector4D>
#include <QJsonDocument>

QVector<QStringList> getBunchOfImages(const QStringList& images_, int bunchSize);
QVector<QPair<QString, QString>> getImagesPairs(const QVector<QStringList>& bunchOfImages_, int bunchSize_);
QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>> getCoordinates(
        const QJsonDocument& configuration_,
        QVector<QPair<QString, QString>>& resultPairs_
        );

#endif // UTILS_FILE_H
