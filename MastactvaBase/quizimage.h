#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <QQuickItem>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaBase/quizimagedata.h"
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
    Q_PROPERTY(QString shadersBuildLog READ shadersBuildLog WRITE setShadersBuildLog NOTIFY shadersBuildLogChanged)
    Q_PROPERTY(int paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
    Q_PROPERTY(int paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    Q_INVOKABLE void updateState();
    Q_INVOKABLE void updateEffects();
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
    QString shadersBuildLog();
    void setShadersBuildLog(const QString &log_);
    void renderBuildError(const QString &log_);

    bool areAllDataAvailable();
    bool isImageDataUpdated() const;
    void retryData();
    Effect *getEffect() const;
    EffectArgSet *getArgumentSet() const;
    bool needToUpdateEffects();
    bool isEffectUpdated() const;
    const QuizImageData &getData() const;

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

protected slots:
    void imageDownloadedSlot(const QString &url_);
    void effectChildrenLoadedSlot();

protected:
    void updateStateIfDataIsReady();
    void addShadersToWaitDownload();
    void formImageData();

signals:
    void fromImageChanged();
    void toImageChanged();
    void effectChanged();
    void tChanged();
    void argumentSetChanged();
    void paintedWidthChanged();
    void paintedHeightChanged();
    void shadersBuildLogChanged();

private:
    QString m_fromImageUrl;
    QString m_toImageUrl;
    qreal m_t = 0.0;
    Effect *m_effect = nullptr;
    EffectArgSet *m_argumentSet = nullptr;
    QString m_renderBuildLog;
    QString m_lastLog;
    QStringList m_artefactsUrls;
    bool m_effectLoading = false;
    QImage *m_image = nullptr;
    QSize m_imageSize = QSize(0,0);
    bool m_updateEffects = false;
    QuizImageData m_data;
    bool m_imageDataUpdated = true;
    bool m_effectUpdated = true;
};


#endif // QUIZIMAGE_H
