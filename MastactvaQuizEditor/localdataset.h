#ifndef LOCALDATASET_H
#define LOCALDATASET_H


#include <QObject>
#include "../MastactvaModels/gallery.h"
#include "../MastactvaModels/shadertype.h"
#include "../MastactvaModels/shaderargtype.h"
#include "../MastactvaModels/easingtype.h"


class LocalDataSet : public QObject
{
    Q_OBJECT
public:
    LocalDataSet(QObject * parent_ = nullptr);
    virtual ~LocalDataSet() override;

    void download();

protected:
    void create();
    void free();
    void downloadStep();
    qreal stepProgress();

signals:
    void downloaded();
    void progress(qreal p_);

protected slots:
    void listReloadedSlot();

private:
    GalleryModel *m_galleryModel = nullptr;
    ShaderTypeModel *m_shaderTypeModel = nullptr;
    ShaderArgTypeModel *m_shaderArgTypeModel = nullptr;
    EasingTypeModel *m_easingTypeModel = nullptr;
    int m_step = 0;

    const int c_downloadedStepsCount = 4;
};

#endif // LOCALDATASET_H
