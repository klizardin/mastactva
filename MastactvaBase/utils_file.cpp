#include "utils_file.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include "../MastactvaBase/names.h"


QVector<QStringList> getBunchOfImages(const QStringList& images_, int bunchSize_)
{
    QVector<QStringList> bunchOfImages;
    bunchOfImages.push_back(QStringList());
    int i = 0;
    for(const QString& imageName : images_)
    {
        if(++i>bunchSize_)
        {
            i = 1;
            bunchOfImages.push_back(QStringList());
        }
        bunchOfImages.back().push_back(imageName);
    }
    //qDebug() << "Bunch of images: " << bunchOfImages;
    return bunchOfImages;
}

QVector<QPair<QString, QString>> getImagesPairs(const QVector<QStringList>& bunchOfImages_, int bunchSize_)
{
    QVector<QPair<QString, QString>> pairs;
    for(int i = 0; i < bunchSize_; i++)
    {
        for(int j = 1; j < bunchOfImages_.size(); ++j)
        {
            //qDebug() << "pair (" << (j-1)*bunchSize_ + i << "," << j*bunchSize_ + i << "):" << bunchOfImages_[j-1].at(i) << "-" << bunchOfImages_[j].at(i);
            if(i >= 0 && i < bunchOfImages_[j-1].size() && i < bunchOfImages_[j].size())
            {
                pairs.push_back(qMakePair(bunchOfImages_[j-1].at(i),bunchOfImages_[j].at(i)));
            }
        }
    }
    for(int j = 0; j < bunchOfImages_.size(); ++j)
    {
        for(int i = 1; i < bunchSize_; i++)
        {
            //qDebug() << "pair (" << j*bunchSize_ + i - 1 << "," << j*bunchSize_ + i << "):" << bunchOfImages_[j].at(i - 1) << "-" << bunchOfImages_[j].at(i);
            if(i>=1 && i < bunchOfImages_[j].size())
            {
                pairs.push_back(qMakePair(bunchOfImages_[j].at(i-1),bunchOfImages_[j].at(i)));
            }
        }
    }

    //qInfo() << "All pairs : " << pairs.size();
    return pairs;
}

void convertJsonResultToCoordinates(const QJsonValue &value_, std::vector<QVector4D> &values_)
{
    if(!value_.isArray())
    {
        return;
    }
    const QJsonArray arr = value_.toArray();
    for(const QJsonValue& val: arr)
    {
        if(!val.isObject())
        {
            continue;
        }
        QVector4D vec;
        const QJsonObject obj = val.toObject();
        if(!obj.contains("x") || !obj.contains("y") ||  !obj.contains("z") || !obj.contains("w"))
        {
            continue;
        }
        vec.setX(obj.value("x").toDouble());
        vec.setY(obj.value("y").toDouble());
        vec.setZ(obj.value("z").toDouble());
        vec.setW(obj.value("w").toDouble());
        values_.push_back(vec);
    }
}

QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>> getCoordinates(
        const QJsonDocument& configuration_,
        QVector<QPair<QString, QString>>& resultPairs_
        )
{
    QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>> result;
    if(configuration_.isEmpty() || configuration_.isNull())
    {
        return result;
    }
    if(!configuration_.isObject())
    {
        return result;
    }
    const QJsonObject rootObj = configuration_.object();
    const QJsonValue rootObjVal = rootObj.value(g_demoJsonPairs);
    if(!rootObjVal.isArray())
    {
        return result;
    }
    const QJsonArray arr = rootObjVal.toArray();
    for(const QJsonValue& val: arr)
    {
        if(!val.isObject())
        {
            continue;
        }
        const QJsonObject obj = val.toObject();
        QString fromName = obj.value(g_demoJsonFieldFromFile).toString();
        QString toFile = obj.value(g_demoJsonFieldToFile).toString();
        const QJsonValue fromValuesArray = obj.value(g_demoJsonFieldFromValues);
        const QJsonValue toValuesArray = obj.value(g_demoJsonFieldToValues);
        std::vector<QVector4D> fromValues, toValues;
        convertJsonResultToCoordinates(fromValuesArray, fromValues);
        convertJsonResultToCoordinates(toValuesArray, toValues);
        resultPairs_.emplace_back(qMakePair(std::move(fromName), std::move(toFile)));
        result.emplace_back(qMakePair(std::move(fromValues), std::move(toValues)));
    }
    return result;
}
