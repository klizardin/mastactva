#ifndef LOCALDATASET_H
#define LOCALDATASET_H


#include <QObject>
#include <QtQuick/QQuickItem>


class GalleryModel;
class ImageModel;
class ShaderTypeModel;
class ShaderArgTypeModel;
class EasingTypeModel;
class UserStepModel;


class LocalDataSet : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit LocalDataSet(QObject * parent_ = nullptr);
    virtual ~LocalDataSet() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)

    Q_INVOKABLE void download();

protected:
    void create();
    void free();
    void downloadStep();
    qreal stepProgress();
    QString savePath() const;
    void setSavePath(const QString &url_);

signals:
    void savePathChanged();
    void downloaded();
    void progress(qreal p_);

protected slots:
    void listReloadedSlot();

private:
    QString m_savePath;
    QString m_serverFilesOldRootDir;
    GalleryModel *m_galleryModel = nullptr;
    ImageModel *m_imageModel = nullptr;
    ShaderTypeModel *m_shaderTypeModel = nullptr;
    ShaderArgTypeModel *m_shaderArgTypeModel = nullptr;
    EasingTypeModel *m_easingTypeModel = nullptr;
    UserStepModel *m_userStepModel = nullptr;
    int m_step = 0;
    int m_galleryIndex = -1;
    int c_downloadStepsCount = 1;
};

#endif // LOCALDATASET_H
