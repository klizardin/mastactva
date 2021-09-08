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

#ifndef SHADERANALYZE_H
#define SHADERANALYZE_H


#include <QObject>
#include <QString>
#include <QStringList>
#include <QQueue>
#include <QByteArray>


class ShaderComments : public QObject
{
    Q_OBJECT
public:
    explicit ShaderComments(QObject *parent_ = nullptr);
    void append(QStringList &files_);

private slots:
    void startNextProcess();

signals:
    void finished();

private:
    QQueue<QString> m_filesQueue;
    int m_totalCount = 0;
    int m_processedCount = 0;
};


#endif // SHADERANALYZE_H
