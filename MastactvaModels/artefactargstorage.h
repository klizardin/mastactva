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

#ifndef ARTEFACTARGSTORAGE_H
#define ARTEFACTARGSTORAGE_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class ArtefactArgStorage : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit ArtefactArgStorage(QObject *parent_ = nullptr);

    Q_PROPERTY(int artefactArgStorageId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString artefactArgStorageStorage READ storage WRITE setStorage NOTIFY storageChanged)

    class DefaultLayout : public LayoutBase<ArtefactArgStorage>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("artefact-arg-storage");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ArtefactArgStorage::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ArtefactArgStorage::m_appId);
            addField<int>("id", "artefactArgStorageId", &ArtefactArgStorage::id, &ArtefactArgStorage::setId);
            addField<QString>("storage", "artefactArgStorageStorage", &ArtefactArgStorage::storage, &ArtefactArgStorage::setStorage);
            setIdField("id");
        }
    };

public:
    int id() const;
    void setId(const int &id_);
    QString storage() const;
    void setStorage(const QString &storage_);

signals:
    void idChanged();
    void storageChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_storage;
};


class ArtefactArgStorageModel : public ListModelBaseOfData<ArtefactArgStorage, ArtefactArgStorageModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ArtefactArgStorage, ArtefactArgStorageModel>;

public:
    explicit ArtefactArgStorageModel(QObject *parent_ = nullptr);

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


#endif // ARTEFACTARGSTORAGE_H
