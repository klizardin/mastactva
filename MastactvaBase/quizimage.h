#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QQuickItem>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/effectargset.h"


class QuizImage : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImage(QQuickItem *parent_ = nullptr);

    Q_PROPERTY(QVariantList fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QVariantList toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QVariant effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QVariant argumentSet READ argumentSet WRITE setArgumentSet NOTIFY argumentSetChanged)
    Q_PROPERTY(int paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
    Q_PROPERTY(int paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Q_INVOKABLE void updateState();
    Q_INVOKABLE void swapImages();

public:
    QVariantList fromImage() const;
    void setFromImage(const QVariantList &fromImageInfo_);
    QString fromImageLocalUrl() const;
    QVariantList toImage() const;
    void setToImage(const QVariantList &toImageInfo_);
    QString toImageLocalUrl() const;
    QVariant effect() const;
    void setEffect(const QVariant &effect_);
    qreal t() const;
    void setT(qreal t_);
    QVariant argumentSet() const;
    void setArgumentSet(const QVariant &argumentSet_);
    int paintedWidth() const;
    int paintedHeight() const;

    bool areAllDataAvailable();
    Effect *getEffect() const;
    EffectArgSet *getArgumentSet() const;

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
    void argumentSetChanged();
    void paintedWidthChanged();
    void paintedHeightChanged();

private:
    QString m_fromImageUrl;
    QString m_toImageUrl;
    qreal m_t = 0.0;
    Effect *m_effect = nullptr;
    EffectArgSet *m_argumentSet = nullptr;
    QStringList m_shadersUrls;
    bool m_effectLoading = false;
    QImage *m_image = nullptr;
    QSize m_imageSize = QSize(0,0);
};


#endif // QUIZIMAGE_H
