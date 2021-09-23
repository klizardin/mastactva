#include "effects_exchange.h"


EffectsExchange::EffectsExchange(QObject *parent_ /*= nullptr*/)
    : QObject(parent_)
{
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
    m_path = path_;
}
