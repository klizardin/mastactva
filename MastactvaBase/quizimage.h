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

    Q_PROPERTY(QVariantList fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QVariantList toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QVariant effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Q_INVOKABLE void updateState();

public:
    QVariantList fromImage() const;
    void setFromImage(const QVariantList &fromImageInfo_);
    QVariantList toImage() const;
    void setToImage(const QVariantList &toImageInfo_);
    QVariant effect() const;
    void setEffect(const QVariant &effect_);
    qreal t() const;
    void setT(qreal t_);

    bool areAllDataAvailable();
    QString getFromImage() const;
    QString getToImage() const;
    Effect *getEffect() const;

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

protected slots:
    void imageDownloadedSlot(const QString &url_);
    void effectChildrenLoadedSlot();

protected:
    void updateStateIfDataIsReady();
    void addShadersToWaitDownload();

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
    QStringList m_shadersUrls;
    bool m_effectLoading = false;
};


#endif // QUIZIMAGE_H