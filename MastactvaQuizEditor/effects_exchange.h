#ifndef EFFECTS_EXCHANGE_H
#define EFFECTS_EXCHANGE_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/dualmodelconfig.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaModels/effectobjects.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/objectinfo.h"
#include "../MastactvaModels/easingtype.h"


class EffectsExchange;


class MergeData
{
public:
    void clear();
    void clearIds(int newSize_, int oldSize_);
    void pushNewId(int id_, const QString &mergeid_);
    void pushOldId(int id_, const QString &mergeid_);
    void pushDifferentId(int aId_, int bId_);
    void sort();
    int count() const;
    bool hasDifferent() const;
    std::pair<int,int> popDifferentId();
    bool hasNewId() const;
    int popNewId();
    void setIdMapping(const QString &layoutName_, int oldId_, int newId_);
    void addIdMapping(const QString &layoutName_, int oldId_);
    bool hasMapping(const QString &layoutName_, int oldId_) const;
    QVariant getMapping(const QString &layoutName_, int oldId_) const;
    void trace(const QString &qmlLayoutName_) const;

private:
    QVector<std::pair<int, QString>> m_onlyInNew;   // add items (with filter idOld -> idNew mark)
    QVector<std::pair<int, QString>> m_onlyInOld;   // do not remove items
    QVector<std::pair<int, int>> m_differents;  // update items
    QHash<QString, QHash<int, int>> m_idsMap;
};


template<typename ModelType_>
class MergeItem
{
    using DataObjectType = typename ModelType_::DataObjectType;
public:
    static int countSteps(
            ModelType_ *model_,
            ModelType_ *inputModel_
            );
    void idMapping(
            MergeData &data_,
            ModelType_ *model_,
            ModelType_ *inputModel_
            );
    void clear();
    bool mergeStepImpl(
            MergeData &data_,
            EffectsExchange *effectExchange_,
            ModelType_ *model_,
            ModelType_ *inputModel_
            );
private:
    bool m_modelTypePrepered = false;
    bool m_modelTypeMerged = false;
    DataObjectType *m_modelItemNew = nullptr;
    int m_oldInsertItemId = -1;
};

template<typename ModelType1_, typename ModelType2_, typename ... ModelTypes_>
class Merge : public MergeItem<ModelType1_>, public Merge<ModelTypes_...>
{
public:
    static int countSteps(ModelType1_ *model_, ModelType2_ *inputModel_, ModelTypes_ *...models_)
    {
        static_assert (std::is_same<ModelType1_, ModelType2_>::value, "you should use same types");
        return MergeItem<ModelType1_>::countSteps(model_, inputModel_) + Merge<ModelTypes_...>::countSteps(models_...);
    }

    void clear()
    {
        MergeItem<ModelType1_>::clear();
        Merge<ModelTypes_...>::clear();
    }

    void idMapping(MergeData &data_, ModelType1_ *model_, ModelType2_ *inputModel_, ModelTypes_ *...models_)
    {
        MergeItem<ModelType1_>::idMapping(data_, model_, inputModel_);
        Merge<ModelTypes_...>::idMapping(data_, models_...);
    }

    bool mergeStep(MergeData &data_, EffectsExchange *effectExchange_, ModelType1_ *model_, ModelType2_ *inputModel_, ModelTypes_ *...models_)
    {
        static_assert (std::is_same<ModelType1_, ModelType2_>::value, "you should use same types");
        if(MergeItem<ModelType1_>::mergeStepImpl(
                    data_,
                    effectExchange_,
                    model_,
                    inputModel_
                    )
                )
        {
            return true;
        }
        return Merge<ModelTypes_...>::mergeStep(data_, effectExchange_, models_...);
    }
};

template<typename ModelType1_, typename ModelType2_>
class Merge<ModelType1_, ModelType2_> : public MergeItem<ModelType1_>
{
public:
    static int countSteps(ModelType1_ *model_, ModelType2_ *inputModel_)
    {
        static_assert (std::is_same<ModelType1_, ModelType2_>::value, "you should use same types");
        return MergeItem<ModelType1_>::countSteps(model_, inputModel_);
    }

    void clear()
    {
        MergeItem<ModelType1_>::clear();
    }

    void idMapping(MergeData &data_, ModelType1_ *model_, ModelType2_ *inputModel_)
    {
        MergeItem<ModelType1_>::idMapping(data_, model_, inputModel_);
    }

    bool mergeStep(MergeData &data_, EffectsExchange *effectExchange_, ModelType1_ *model_, ModelType2_ *inputModel_)
    {
        static_assert (std::is_same<ModelType1_, ModelType2_>::value, "you should use same types");
        return MergeItem<ModelType1_>::mergeStepImpl(
                    data_,
                    effectExchange_,
                    model_,
                    inputModel_
                    );
    }
};



class EffectsExchange : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit EffectsExchange(QObject *parent_ = nullptr);
    ~EffectsExchange() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)
    Q_PROPERTY(QString saveArchive READ saveArchive WRITE setSaveArchive NOTIFY saveArchiveChanged)
    Q_INVOKABLE bool download();
    Q_INVOKABLE bool upload();
    Q_INVOKABLE bool mergeDownload();
    Q_INVOKABLE bool merge();
    Q_INVOKABLE void cancel();

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
    void extractArchive(const QString &path_);
    void uploadStep();
    void setLocalFiles();
    void disconnectDownload();
    void disconnectUpload();
    bool downloadImpl();
    bool uploadImpl();
    bool mergeImpl();
    void mergeStep();

public:
signals:
    void savePathChanged();
    void saveArchiveChanged();
    void downloaded();
    void progress(qreal p_, const QString &status_);
    void uploaded();
    void merged();

protected slots:
    void listReloadedSlot();
    void listReloadedSlotForImport();
public slots:
    void itemSetSlotForImport();
    void itemAddedSlotForImport();

private:
    QString m_path;
    QString m_archiveName;
    QString m_oldPathServerFiles;

    // export
    // models
    std::unique_ptr<EffectModel> m_effectModel;
    std::unique_ptr<ArtefactModel> m_artefactModel;
    std::unique_ptr<EffectObjectsModel> m_effectObjectsModel;
    std::unique_ptr<ObjectArtefactModel> m_objectArtefactModel;
    std::unique_ptr<ObjectInfoModel> m_objectInfoModel;
    std::unique_ptr<EffectArgModel> m_effectArgModel;
    std::unique_ptr<EffectArgSetModel> m_effectArgSetModel;
    std::unique_ptr<EffectArgValueModel> m_effectArgValueModel;
    std::unique_ptr<ArtefactTypeModel> m_artefactTypeModel;
    std::unique_ptr<ArtefactArgTypeModel> m_artefactArgTypeModel;
    std::unique_ptr<ArtefactArgStorageModel> m_artefactArgStorageModel;
    std::unique_ptr<EasingTypeModel> m_easingTypeModel;

    // import
    std::unique_ptr<ChooseModelConfig> m_inputModelConfig;
    // models
    std::unique_ptr<EffectModel> m_inputEffectModel;
    std::unique_ptr<ArtefactModel> m_inputArtefactModel;
    std::unique_ptr<EffectObjectsModel> m_inputEffectObjectsModel;
    std::unique_ptr<ObjectArtefactModel> m_inputObjectArtefactModel;
    std::unique_ptr<ObjectInfoModel> m_inputObjectInfoModel;
    std::unique_ptr<EffectArgModel> m_inputEffectArgModel;
    std::unique_ptr<EffectArgSetModel> m_inputEffectArgSetModel;
    std::unique_ptr<EffectArgValueModel> m_inputEffectArgValueModel;
    std::unique_ptr<ArtefactTypeModel> m_inputArtefactTypeModel;
    std::unique_ptr<ArtefactArgTypeModel> m_inputArtefactArgTypeModel;
    std::unique_ptr<ArtefactArgStorageModel> m_inputArtefactArgStorageModel;
    std::unique_ptr<EasingTypeModel> m_inputEasingTypeModel;

    // merge
    using MergeType = Merge<
        EasingTypeModel, EasingTypeModel,
        ArtefactArgStorageModel, ArtefactArgStorageModel,
        ArtefactArgTypeModel, ArtefactArgTypeModel,
        ArtefactTypeModel, ArtefactTypeModel,
        ArtefactModel, ArtefactModel,
        EffectModel, EffectModel,
        ObjectInfoModel, ObjectInfoModel,
        EffectObjectsModel, EffectObjectsModel,
        ObjectArtefactModel, ObjectArtefactModel,
        EffectArgModel, EffectArgModel,
        EffectArgSetModel, EffectArgSetModel,
        EffectArgValueModel, EffectArgValueModel
        >;

    MergeData m_mergeData;
    MergeType m_merge;

    int m_step = 0;
    int c_downloadStepsCount = 1;
    bool m_downloading = false;
    bool m_uploading = false;
};


#endif // EFFECTS_EXCHANGE_H
