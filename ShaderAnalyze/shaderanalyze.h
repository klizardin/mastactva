#ifndef SHADERANALYZE_H
#define SHADERANALYZE_H


#include <QObject>
#include <QString>
#include <QByteArray>


class ShaderAnalyze : public QObject
{
    Q_OBJECT
public:
    explicit ShaderAnalyze(QObject *parent = nullptr);
    void process(const QByteArray &data_);

signals:

};

#endif // SHADERANALYZE_H
