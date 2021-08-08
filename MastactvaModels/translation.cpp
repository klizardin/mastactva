#include "translation.h"
#include "../MastactvaBase/defines.h"


Translation::Translation(TranslationModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Translation::Translation()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Translation::Translation()" << this;
#endif

    m_translationModel = parent_;
}

int Translation::id() const
{
    return m_id;
}

void Translation::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int Translation::galleryId() const
{
    return m_galleryId;
}

void Translation::setGalleryId(const int &galleryId_)
{
    m_galleryId = galleryId_;

    emit galleryIdChanged();
}

int Translation::descriptionId() const
{
    return m_descriptionId;
}

void Translation::setDescriptionId(const int &descriptionId_)
{
    m_descriptionId = descriptionId_;

    emit descriptionIdChanged();
}

int Translation::questionId() const
{
    return m_questionId;
}

void Translation::setQuestionId(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionIdChanged();
}

int Translation::answerId() const
{
    return m_answerId;
}

void Translation::setAnswerId(const int &answerId_)
{
    m_answerId = answerId_;

    emit answerIdChanged();
}

QString Translation::text() const
{
    return m_text;
}

void Translation::setText(const QString &text_)
{
    m_text = text_;

    emit textChanged();
}

QDateTime Translation::created() const
{
    return m_created;
}

void Translation::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

bool Translation::autoTranslate() const
{
    return m_autoTranslate;
}

void Translation::setAutoTranslate(const bool &autoTranslate_)
{
    m_autoTranslate = autoTranslate_;

    emit autoTranslateChanged();
}

QString Translation::langFrom() const
{
    return m_langFrom;
}

void Translation::setLangFrom(const QString &langFrom_)
{
    m_langFrom = langFrom_;

    emit langFromChanged();
}

QString Translation::langTo() const
{
    return m_langTo;
}

void Translation::setLangTo(const QString &langTo_)
{
    m_langTo = langTo_;

    emit langToChanged();
}


TranslationModel::TranslationModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "TranslationModel::TranslationModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "TranslationModel::TranslationModel()" << this;
#endif

    init(this);
}
