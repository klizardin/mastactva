#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H


#include <QString>
#include <QFile>
#include <QVariant>
#include <QUrl>
#include <QJsonValue>


class FileSource : public QString
{
public:
    using QString::QString;
    FileSource(const QString &str_)
    {
        operator=(str_);
    }
    FileSource &operator = (const QString& str_)
    {
        *static_cast<QString *>(this) = str_;
        return *this;
    }
};


namespace layout
{
    inline QVariant getValue(const FileSource &val_, bool toQML_ = true)
    {
        if(toQML_)
        {
            return QVariant::fromValue(static_cast<const QString &>(val_));
        }
        else
        {
            QUrl url(val_);
            QString filename = url.toLocalFile();
            QFile *f = new QFile(filename);
            return QVariant::fromValue(static_cast<QObject *>(f));
        }
    }

    inline void setValue(const QVariant &var_, FileSource &dta_)
    {
        dta_ = qvariant_cast<QString>(var_);
    }

    inline void setValue(const QJsonValue &var_, FileSource &dta_)
    {
        dta_ = var_.toString();
    }
}


#endif // IMAGESOURCE_H
