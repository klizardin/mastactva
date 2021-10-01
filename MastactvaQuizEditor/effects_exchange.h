#ifndef EFFECTS_EXCHANGE_H
#define EFFECTS_EXCHANGE_H


#include <QObject>
#include <QtQuick/QQuickItem>
//#include "../MastactvaBase/dualmodelconfig.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/easingtype.h"


class EffectsExchange : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EffectsExchange(QObject *parent_ = nullptr);
    ~EffectsExchange() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_PROPERTY(QString saveArchive READ saveArchive WRITE setSaveArchive NOTIFY saveArchiveChanged)
    Q_INVOKABLE void download();
    Q_INVOKABLE void upload();
    Q_INVOKABLE void merge();

private:
    QString savePath() const;
    void setSavePath(const QString &url_);
    QString saveArchive() const;
    void setSaveArchive(const QString &url_);
    void initSavePath(const QString &path_);
    void free();
    void create();
    void downloadStep();
    void archiveResults();
    qreal stepProgress();
    void freeInput();
    void createInput();

public:
signals:
    void savePathChanged();
    void saveArchiveChanged();
    void downloaded();
    void progress(qreal p_, const QString &status_);

protected slots:
    void listReloadedSlot();

private:
    QString m_path;
    QString m_archiveName;
    QString m_oldPathServerFiles;

    std::unique_ptr<EffectModel> m_effectModel;
    std::unique_ptr<ArtefactTypeModel> m_artefactTypeModel;
    std::unique_ptr<ArtefactArgTypeModel> m_artefactArgTypeModel;
    std::unique_ptr<ArtefactArgStorageModel> m_artefactArgStorageModel;
    std::unique_ptr<EasingTypeModel> m_easingTypeModel;

    //std::unique_ptr<ChooseModelConfig> m_inputModelConfig;
    std::unique_ptr<EffectModel> m_inputEffectModel;
    std::unique_ptr<ArtefactTypeModel> m_inputArtefactTypeModel;
    std::unique_ptr<ArtefactArgTypeModel> m_inputArtefactArgTypeModel;
    std::unique_ptr<ArtefactArgStorageModel> m_inputArtefactArgStorageModel;
    std::unique_ptr<EasingTypeModel> m_inputEasingTypeModel;

    int m_step = 0;
    int c_downloadStepsCount = 1;
};


#endif // EFFECTS_EXCHANGE_H
