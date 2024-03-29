/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ARTEFACT_H
#define ARTEFACT_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefact_data.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/artefactarg.h"


class ArtefactModel;


class Artefact : public QObject, protected IListModelInfoObjectImpl, public IListModelItem, private ArtefactData
{
    Q_OBJECT
public:
    explicit Artefact(ArtefactModel *parent_ = nullptr);
    explicit Artefact(ArtefactData &&data_, ArtefactModel *parent_ = nullptr);
    virtual ~Artefact() override;

    Q_PROPERTY(int artefactId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString artefactName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString artefactFilename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString artefactHash READ hash WRITE setHash NOTIFY hashChanged)
    Q_PROPERTY(int artefactTypeId READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString artefactDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime artefactCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant artefactArtefactArg READ artefactArg WRITE setArtefactArg NOTIFY artefactArgChanged)
    Q_PROPERTY(MergeId artefactMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)

    class DefaultLayout : public LayoutBase<Artefact>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Artefact::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Artefact::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Artefact::m_appId);
            addField<int>("id", "artefactId", &Artefact::id, &Artefact::setId);
            addField<QString>("name", "artefactName", &Artefact::name, &Artefact::setName);
            addField<FileSource>("filename", "artefactFilename", &Artefact::getFilename, &Artefact::setFilename);
            addField<QString>("hash", "artefactHash", &Artefact::hash, &Artefact::setHash);
            addField<int>("type", "artefactTypeId", &Artefact::type, &Artefact::setType);
            addField<QString>("description", "artefactDescription", &Artefact::description, &Artefact::setDescription);
            addField<QDateTime>("created", "artefactCreated", &Artefact::created, &Artefact::setCreated);
            addField<MergeId>("mergeid", "artefactMergeId", &Artefact::mergeid, &Artefact::setMergeid);
            addModel<ArtefactArgModel>("artefactArtefactArg", &Artefact::m_artefactArgModel, &Artefact::createArtefactArgModel);
            /* 1:N */
            setIdField("id");
        }
    };

    Q_INVOKABLE void getArgumentsFromArtefactText();
    Q_INVOKABLE bool isShader();
    Q_INVOKABLE bool isTexture();
    Q_INVOKABLE bool isJson();
    Q_INVOKABLE bool isObj3d();
    Q_INVOKABLE bool isLua();
    Q_INVOKABLE QString getFileFilter();
    Q_INVOKABLE void setArtefactFilenameLocalFile(const QString fileName_);
    Q_INVOKABLE void downloadFile();

public:
    int id() const;
    void setId(const int &id_);
    QString name() const;
    void setName(const QString &name_);
    QString filename() const;
    FileSource getFilename() const;
    void setFilename(const QString &filename_);
    void setFilename(const FileSource &filename_);
    QString hash() const;
    void setHash(const QString &hash_);
    int type() const;
    void setType(const int &type_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant artefactArg() const;
    void setArtefactArg(const QVariant &obj_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);

    ArtefactArgModel *getArtefactArg();
    const ArtefactArgModel *getArtefactArg() const;

    bool isObjectLoaded() const;

protected:
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    QString getObjectName() const;

    ArtefactArgModel *createArtefactArgModel();

signals:
    void idChanged();
    void nameChanged();
    void filenameChanged();
    void hashChanged();
    void typeChanged();
    void descriptionChanged();
    void createdChanged();
    void artefactArgChanged();
    void argumentsFromArtefactTextChanged();
    void fileDownloaded();
    void mergeidChanged();

protected slots:
    void artefactFileDownloaded(const QString &url_);
    void argItemAdded();
    void argItemSet();

private:
    void addArgumentsFromComments();

private:
    ArtefactModel *m_artefactModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    ArtefactArgModel *m_artefactArgModel = nullptr;

    QVector<Comment> m_comments;
    int m_commentIndex = 0;
    bool m_insideArtefactDownloding = false;

    friend class ListModelBaseOfData<ArtefactData, ArtefactModel, Artefact>;
};


class ArtefactModel : public ListModelBaseOfData<ArtefactData, ArtefactModel, Artefact>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactData, ArtefactModel, Artefact>;

public:
    explicit ArtefactModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<ArtefactData *>> data_ = std::shared_ptr<QVector<ArtefactData *>>{nullptr},
            IModelConfig *config_ = &DefaultModelConfig::instance()
            );

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString& errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void objectNameDerivedChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // ARTEFACT_H
