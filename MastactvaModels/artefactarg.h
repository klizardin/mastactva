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

#ifndef ARTEFACTARG_H
#define ARTEFACTARG_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/artefactarg_data.h"


class ArtefactArgModel;


class ArtefactArg : public QObject, public IListModelItem, private ArtefactArgData
{
    Q_OBJECT
public:
    explicit ArtefactArg(ArtefactArgModel *parent_ = nullptr);
    explicit ArtefactArg(ArtefactArgData &&data_, ArtefactArgModel *parent_ = nullptr);

    Q_PROPERTY(int artefactArgId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int artefactArgArtefactId READ artefactId WRITE setArtefactId NOTIFY artefactIdChanged)
    Q_PROPERTY(int artefactArgArgTypeId READ argTypeId WRITE setArgTypeId NOTIFY argTypeChanged)
    Q_PROPERTY(int artefactArgArgStorageId READ argStorageId WRITE setArgStorageId NOTIFY argStorageChanged)
    Q_PROPERTY(QString artefactArgName READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString artefactArgDefaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(QString artefactArgDescription READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime artefactArgCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(MergeId artefactArgMergeId READ mergeid WRITE setMergeid NOTIFY mergeidChanged)
    Q_PROPERTY(bool artefactArgHasFileReference READ hasFileReference WRITE setHasFileReference NOTIFY hasFileReferenceChanged)
    Q_PROPERTY(bool artefactArgHasFile READ hasFile WRITE setHasFile NOTIFY hasFileChanged)
    Q_PROPERTY(QString artefactArgFilename READ getFilename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString artefactArgHash READ hash WRITE setHash NOTIFY hashChanged)

    class DefaultLayout : public LayoutBase<ArtefactArg>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact-arg");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ArtefactArg::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &ArtefactArg::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ArtefactArg::m_appId);
            addField<int>("id", "artefactArgId", &ArtefactArg::id, &ArtefactArg::setId);
            addField<int>("artefact", "artefactArgArtefactId", &ArtefactArg::artefactId, &ArtefactArg::setArtefactId);
            addField<int>("arg_type", "artefactArgArgTypeId", &ArtefactArg::argTypeId, &ArtefactArg::setArgTypeId);
            addField<int>("arg_storage", "artefactArgArgStorageId", &ArtefactArg::argStorageId, &ArtefactArg::setArgStorageId);
            addField<QString>("name", "artefactArgName", &ArtefactArg::name, &ArtefactArg::setName);
            addField<QString>("default_value", "artefactArgDefaultValue", &ArtefactArg::defaultValue, &ArtefactArg::setDefaultValue);
            addField<QString>("description", "artefactArgDescription", &ArtefactArg::description, &ArtefactArg::setDescription);
            addField<QDateTime>("created", "artefactArgCreated", &ArtefactArg::created, &ArtefactArg::setCreated);
            addField<MergeId>("mergeid", "artefactArgMergeId", &ArtefactArg::mergeid, &ArtefactArg::setMergeid);
            addField<bool>("has_file_reference", "artefactArgHasFileReference", &ArtefactArg::hasFileReference, &ArtefactArg::setHasFileReference);
            addField<bool>("has_file", "artefactArgHasFile", &ArtefactArg::hasFile, &ArtefactArg::setHasFile);
            addField<FileSource>("filename", "artefactArgFilename", &ArtefactArg::getFilename, &ArtefactArg::setFilename);
            addField<QString>("hash", "artefactArgHash", &ArtefactArg::hash, &ArtefactArg::setHash);
            setIdField("id");
        }
    };

    Q_INVOKABLE void updateArtefactId(int effectArtefactId_);

public:
    int id() const;
    void setId(const int &id_);
    int artefactId() const;
    void setArtefactId(const int &effectArtefactId_);
    int argTypeId() const;
    void setArgTypeId(const int &argTypeId_);
    int argStorageId() const;
    void setArgStorageId(const int &argStorageId_);
    QString name() const;
    void setName(const QString &name_);
    QString defaultValue() const;
    void setDefaultValue(const QString &defaultValue_);
    QString description() const;
    void setDescription(const QString &description_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    MergeId mergeid() const;
    void setMergeid(const MergeId &mergeid_);
    bool hasFileReference() const;
    void setHasFileReference(const bool &value_);
    bool hasFile() const;
    void setHasFile(const bool &value_);
    FileSource getFilename() const;
    void setFilename(const QString &filename_);
    void setFilename(const FileSource &filename_);
    QString hash() const;
    void setHash(const QString &hash_);

    bool createFrom(const int &effectArtefactId_, const Comment &comment_);
    void copyFrom(const ArtefactArg *arg_);

signals:
    void idChanged();
    void artefactIdChanged();
    void argTypeChanged();
    void argStorageChanged();
    void nameChanged();
    void defaultValueChanged();
    void descriptionChanged();
    void createdChanged();
    void mergeidChanged();
    void hasFileReferenceChanged();
    void hasFileChanged();
    void filenameChanged();
    void hashChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    ArtefactArgModel *m_artefactArgModel = nullptr;


    friend class ListModelBaseOfData<ArtefactArgData, ArtefactArgModel, ArtefactArg>;
};


class ArtefactArgModel : public ListModelBaseOfData<ArtefactArgData, ArtefactArgModel, ArtefactArg>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactArgData, ArtefactArgModel, ArtefactArg>;

public:
    explicit ArtefactArgModel(
            QObject *parent_ = nullptr,
            std::shared_ptr<QVector<ArtefactArgData *>> data_ = std::shared_ptr<QVector<ArtefactArgData *>>{nullptr}
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


#endif // ARTEFACTARG_H
