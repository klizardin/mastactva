#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <memory>
#include <QtQuick/QQuickFramebufferObject>
#include "../MastactvaBase/quizimagedrawingdata.h"


class IDefaultData
{
public:
    virtual ~IDefaultData() = default;
    virtual void initialize() = 0;
};


namespace drawing_data
{
    class DefaultQuizImageDrawingData : public IDefaultData, public QuizImageDrawingData
    {
    public:
        virtual void initialize() override;
    };
}

class QuizImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(QuizImageDemo)
public:
    QuizImage();

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QString projectFilename READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QString compilerLog READ log WRITE setLog NOTIFY logChanged)

    Q_INVOKABLE void updateState();
    Q_INVOKABLE void updateProject();

public:
    virtual Renderer *createRenderer() const override;

public:
    qreal t() const;
    bool isImageDataUpdated() const;
    std::unique_ptr<drawing_data::QuizImageDrawingData> getData();
    void setDataToFree(std::unique_ptr<drawing_data::QuizImageDrawingData> &&old_);
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

private:
    void initDefaultDrawingData();

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
    std::unique_ptr<drawing_data::QuizImageDrawingData> m_drawingData;
    std::unique_ptr<drawing_data::QuizImageDrawingData> m_drawingOldData;
    QString m_compilerLog;
};

#endif // QUIZIMAGE_H
