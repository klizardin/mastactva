#ifndef QUIZIMAGEDEMO_H
#define QUIZIMAGEDEMO_H


#include <QtQuick/QQuickFramebufferObject>
#include <QQuickItem>
#include <QColor>
#include "../MastactvaBase/quizimagedata.h"


class QuizImageDemo : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    QuizImageDemo(QQuickItem *parent_ = nullptr);
    virtual ~QuizImageDemo() override;

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QString projectFilename READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QString compilerLog READ log WRITE setLog NOTIFY logChanged)

    Q_INVOKABLE void updateState();

public:
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;

public:
    qreal t() const;
    bool isImageDataUpdated() const;
    OpenGLDrawingImageData *getData();
    void retryData();
    void renderBuildError(const QString &compilerLog_);

protected:
    void setT(const qreal &t_);
    QString fromImage() const;
    void setFromImage(const QString &fromImage_);
    QString toImage() const;
    void setToImage(const QString &toImage_);
    QString project() const;
    void setProject(const QString &project_);
    QString log() const;
    void setLog(const QString &log_);

    void freeProject();
    void loadProject();
    void setProjectFromImage();
    void setProjectToImage();

signals:
    void tChanged();
    void fromImageChanged();
    void toImageChanged();
    void projectChanged();
    void logChanged();

private:
    qreal m_t = 0.0;
    QString m_fromImage;
    QString m_toImage;
    QString m_project;
    OpenGLDrawingImageData *m_drawingData = nullptr;
    QString m_compilerLog;
};


using QuizImage = QuizImageDemo;


#endif // QUIZIMAGEDEMO_H
