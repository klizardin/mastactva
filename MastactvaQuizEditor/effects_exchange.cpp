#include "effects_exchange.h"


#include <QFileInfo>


static const char *g_defaultSavePath = "./LocalData/export.tar";


EffectsExchange::EffectsExchange(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    initSavePath(g_defaultSavePath);
}

EffectsExchange::~EffectsExchange()
{
}

void EffectsExchange::download()
{
}

void EffectsExchange::upload()
{
}

QString EffectsExchange::savePath() const
{
    return m_path;
}

void EffectsExchange::setSavePath(const QString &path_)
{
    initSavePath(path_);

    emit savePathChanged();
}

void EffectsExchange::initSavePath(const QString &path_)
{
    QFileInfo fi(path_);
    m_path = fi.absolutePath();
    m_archiveName = fi.absoluteFilePath();
    if(!m_archiveName.endsWith(".tar"))
    {
        m_archiveName += ".tar";
    }
}
