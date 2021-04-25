#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


/*#include <QQuickItem>
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
    OpenGLDrawingImageData *getData();

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
};*/


#include <memory>
#include <algorithm>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaBase/quizimagedrawingdata.h"


class QuizImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    struct Image
    {
        QString name;
        QString hash;
        bool loaded = false;
    };

public:
    QuizImage();

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QVariantList fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QVariantList toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QVariant effect READ effect WRITE setEffect NOTIFY effectChanged)
    Q_PROPERTY(QVariant argumentSet READ argumentSet WRITE setArgumentSet NOTIFY argumentSetChanged)
    Q_PROPERTY(QString shadersBuildLog READ log WRITE setLog NOTIFY logChanged)
    Q_PROPERTY(int paintedWidth READ paintedWidth NOTIFY paintedWidthChanged)
    Q_PROPERTY(int paintedHeight READ paintedHeight NOTIFY paintedHeightChanged)

    Q_INVOKABLE void updateState();
    Q_INVOKABLE void updateEffects();
    Q_INVOKABLE void swapImages();

public:
    virtual Renderer *createRenderer() const override;

public:
    qreal t() const;
    bool isImageDataUpdated() const;
    std::unique_ptr<drawing_data::QuizImageObjects> getData();
    void setDataToFree(std::unique_ptr<drawing_data::QuizImageObjects> &&old_);
    void renderBuildError(const QString &compilerLog_);
    bool isFromImageReady() const;
    bool isToImageReady() const;
    const QString &getFromImageUrl() const;
    const QString &getToImageUrl() const;

protected:
    void setT(const qreal &t_);
    QVariantList fromImage() const;
    void setFromImage(const QVariantList &fromImage_);
    QVariantList toImage() const;
    void setToImage(const QVariantList &toImage_);
    QString log() const;
    void setLog(const QString &log_);
    QVariant effect() const;
    void setEffect(const QVariant &effect_);
    QVariant argumentSet() const;
    void setArgumentSet(const QVariant &argumentSet_);
    int paintedWidth() const;
    int paintedHeight() const;

    void freeProject();
    void loadProject();
    void setProjectFromImage();
    void setProjectToImage();

private:
    void initDefaultDrawingData();

private slots:
    void imageDownloadedSlot(const QString &url_);

private:
    void updateStateIfDataIsReady();

signals:
    void tChanged();
    void fromImageChanged();
    void toImageChanged();
    void logChanged();
    void effectChanged();
    void argumentSetChanged();
    void paintedWidthChanged();
    void paintedHeightChanged();

private:
    qreal m_t = 0.0;
    Image m_fromImage;
    Image m_toImage;
    QString m_project;
    std::unique_ptr<drawing_data::QuizImageObjects> m_drawingData;
    std::unique_ptr<drawing_data::QuizImageObjects> m_drawingOldData;
    QString m_compilerLog;
    QVariant m_effect;
    QVariant m_argumentSet;
    QSize m_paintedSize;
};


#endif // QUIZIMAGE_H
