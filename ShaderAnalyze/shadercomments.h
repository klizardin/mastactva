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
