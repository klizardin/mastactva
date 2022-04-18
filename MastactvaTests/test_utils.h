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

#ifndef TEST_UTILS_H
#define TEST_UTILS_H


#include <QString>
#include <QDir>
#include <QFileInfo>


inline
bool equal(
        const QString &result_,
        const QString &stringsSum_
        )
{
    return result_ == stringsSum_;
}

template<typename Arg_> inline
QString sum(Arg_ arg_)
{
    return QString(arg_);
}

template<typename Arg_, typename ... Args_> inline
QString sum(Arg_ arg_, Args_ ... args_)
{
    return QString(arg_) + sum(args_ ...);
}


bool findDynLibs(const QDir &dir_, QDir &result_)
{
    QFileInfoList files = dir_.entryInfoList(QStringList{} << "*.so", QDir::Files);
    if(!files.isEmpty())
    {
        result_ = dir_;
        return true;
    }
    QFileInfoList dirs = dir_.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
    for(const QFileInfo &fi_ : dirs)
    {
        if(!fi_.isDir())
        {
            continue;
        }
        if(findDynLibs(QDir(fi_.absoluteFilePath()), result_))
        {
            return true;
        }
    }
    return false;
}


#endif // TEST_UTILS_H
