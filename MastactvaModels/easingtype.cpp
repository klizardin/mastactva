#include "easingtype.h"
#include "../MastactvaBase/defines.h"


EasingType::EasingType(EasingTypeModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_easingTypeModel = parent_;
#if defined(TRACE_THREADS)
    qDebug() << "EasingType::EasingType()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EasingType::EasingType()" << this;
#endif
}

int EasingType::id() const
{
    return m_id;
}

void EasingType::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString EasingType::type() const
{
    return m_type;
}

void EasingType::setType(const QString &type_)
{
    m_type = type_;

    emit typeChanged();
}


EasingTypeModel::EasingTypeModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EasingTypeModel::EasingTypeModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EasingTypeModel::EasingTypeModel()" << this;
#endif
    init(this);
}
