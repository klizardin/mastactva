#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QQuickItem>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaModels/effect.h"


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QVariant effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Q_INVOKABLE void updateState();

public:
    QString fromImage() const;
    void setFromImage(const QString &fromImageUrl_);
    QString toImage() const;
    void setToImage(const QString &toImageUrl_);
    QVariant effect() const;
    void setEffect(const QVariant &effect_);
    qreal t() const;
    void setT(qreal t_);

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

signals:
    void fromImageChanged();
    void toImageChanged();
    void effectChanged();
    void tChanged();

private:
    QString m_fromImageUrl;
    QString m_toImageUrl;
    qreal m_t = 0.0;
    Effect *m_effect = nullptr;
};


#endif // QUIZIMAGE_H
