/*
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

#include "imagepointeffect.h"
#include "../MastactvaBase/defines.h"


ImagePointEffect::ImagePointEffect(ImagePointEffectModel *parent_ /*=nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointEffect::ImagePointEffect()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointEffect::ImagePointEffect()" << this;
#endif

    m_imagePointEffectModel = parent_;
}

ImagePointEffect::~ImagePointEffect()
{
    if(m_effectModel)
    {
        m_effectModel->deleteLater();
    }
    m_effectModel = nullptr;
    if(m_effectArgSetModel)
    {
        m_effectArgSetModel->deleteLater();
    }
    m_effectArgSetModel = nullptr;
}

int ImagePointEffect::id() const
{
    return m_id;
}

void ImagePointEffect::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePointEffect::imagePointId() const
{
    return m_imagePointId;
}

void ImagePointEffect::setImagePointId(const int &imagePointId_)
{
    m_imagePointId = imagePointId_;

    emit imagePointIdChanged();
}

int ImagePointEffect::effectId() const
{
    return m_effectId;
}

void ImagePointEffect::setEffectId(const int &effectId_)
{
    if(effectId_ == m_effectId) { return; }

    m_effectId = effectId_;
    if(m_effectModel)
    {
        m_effectModel->parentItemChanged();
    }

    emit effectChanged();
}

QVariant ImagePointEffect::effect() const
{
    if(!m_effectModel)
    {
        const_cast<ImagePointEffect *>(this)->m_effectModel = const_cast<ImagePointEffect *>(this)
                ->createEffectModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectModel *>(
                                       m_effectModel)
                                   )
                               );
}

void ImagePointEffect::setEffect(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectModel)
    {
        delete m_effectModel;
        m_effectModel = nullptr;

        emit effectChanged();
    }
}

int ImagePointEffect::argSetId() const
{
    return m_effectArgSetId;
}

void ImagePointEffect::setArgSetId(const int &argSetId_)
{
    if(argSetId_ == m_effectArgSetId) { return; }

    m_effectArgSetId = argSetId_;

    if(m_effectArgSetModel)
    {
        m_effectArgSetModel->parentItemChanged();
    }

    emit argSetChanged();
}

QVariant ImagePointEffect::argSet() const
{
    if(!m_effectArgSetModel)
    {
        const_cast<ImagePointEffect *>(this)->m_effectArgSetModel = const_cast<ImagePointEffect *>(this)
                ->createEffectArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<EffectArgSetModel *>(
                                       m_effectArgSetModel)
                                   )
                               );
}

void ImagePointEffect::setArgSet(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectArgSetModel)
    {
        delete m_effectArgSetModel;
        m_effectArgSetModel = nullptr;

        emit argSetChanged();
    }
}

int ImagePointEffect::duration() const
{
    return m_duration;
}

void ImagePointEffect::setDuration(const int &duration_)
{
    m_duration = duration_;

    emit durationChanged();
}

EffectModel *ImagePointEffect::createEffectModel()
{
    EffectModel *m = new EffectModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_imagePointEffectModel->getQMLLayoutName(), "effect");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointEffectModel->getQMLLayoutName() + QString("_ImagePointEffect_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgSetModel *ImagePointEffect::createEffectArgSetModel()
{
    EffectArgSetModel *m = new EffectArgSetModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_imagePointEffectModel->getQMLLayoutName(), "arg_set");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointEffectModel->getQMLLayoutName() + QString("_ImagePointEffect_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgSetModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ImagePointEffectModel::ImagePointEffectModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointEffectModel::ImagePointEffectModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointEffectModel::ImagePointEffectModel()" << this;
#endif

    init(this);
}
