/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef IMAGESOURCE_H
#define IMAGESOURCE_H


#include <QString>
#include <QFile>
#include <QVariant>
#include <QUrl>
#include <QJsonValue>


class FileSourceReference;


class FileSource : public QString
{
public:
    using QString::QString;
    FileSource(const QString &str_)
    {
        operator=(str_);
    }

    FileSource(const FileSource &fileSource_)
        :QString(static_cast<const QString&>(fileSource_))
    {
    }

    FileSource &operator = (const QString& str_)
    {
        *static_cast<QString *>(this) = str_;
        return *this;
    }

    FileSource &operator = (const FileSource &fileSource_)
    {
        return operator=(static_cast<const QString&>(fileSource_));
    }

    bool hasFile() const
    {
        return m_hasFile;
    }

private:
    bool m_hasFile = true;

    friend class FileSourceReference;
};

class FileSourceReference
{
public:
    FileSourceReference(const QString &str_ = QString())
    {
        m_fileSource.operator=(str_);
    }

    FileSourceReference(const FileSource &fileSource_)
    {
        m_fileSource = fileSource_;
    }

    FileSourceReference(const FileSourceReference &fileSourceReference_)
    {
        m_fileSource = static_cast<const FileSource &>(fileSourceReference_);
    }

    FileSourceReference &operator = (const QString& str_)
    {
        m_fileSource.operator=(str_);
        return *this;
    }

    FileSourceReference &operator = (const FileSource &fileSource_)
    {
        return operator=(static_cast<const QString&>(fileSource_));
    }

    FileSourceReference &operator = (const FileSourceReference &fileSourceReference_)
    {
        return operator=(static_cast<const FileSource &>(fileSourceReference_));
    }

    bool isInsideObject() const
    {
        return m_hasFile
                && m_hasFileReference
                && m_value
                ;
    }

    void initObjectDetails(bool *hasFile_, bool *hasFileReference_, QString *value_)
    {
        m_hasFile = hasFile_;
        m_hasFileReference = hasFileReference_;
        m_value = value_;
    }

    const QString & value() const
    {
        if(isInsideObject()
                && !*m_hasFile
                && *m_hasFileReference
                )
        {
            return *m_value;
        }
        else
        {
            return static_cast<const QString &>(m_fileSource);
        }
    }

    bool hasFile() const
    {
        return isInsideObject() && *m_hasFile;
    }

    operator FileSource () const
    {
        FileSource result(value());
        result.m_hasFile = hasFile();
        return result;
    }

    void clear()
    {
        m_fileSource.clear();
    }

private:
    bool *m_hasFile = nullptr;
    bool *m_hasFileReference = nullptr;
    QString *m_value = nullptr;
    FileSource m_fileSource;
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
            if(val_.hasFile())
            {
                QUrl url(val_);
                QString filename = url.toLocalFile();
                QFile *f = new QFile(filename);
                return QVariant::fromValue(static_cast<QObject *>(f));
            }
            else
            {
                return QVariant::fromValue(nullptr);
            }
        }
    }

    inline void setValue(const QVariant &var_, FileSource &dta_)
    {
        dta_ = qvariant_cast<QString>(var_);
    }

    inline void setValue(const QJsonValue &var_, FileSource &dta_)
    {
        if(var_.isNull())
        {
            dta_.clear();
        }
        else
        {
            dta_ = var_.toString();
        }
    }

    inline void setValue(const QVariant &var_, FileSourceReference &dta_)
    {
        dta_ = qvariant_cast<QString>(var_);
    }

    inline void setValue(const QJsonValue &var_, FileSourceReference &dta_)
    {
        if(var_.isNull())
        {
            dta_.clear();
        }
        else
        {
            dta_ = var_.toString();
        }
    }
}


#endif // IMAGESOURCE_H
