#include "utils_file.h"
#include <QDebug>


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
