/*
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

#ifndef IMODEL_H
#define IMODEL_H


#include <QString>
#include <QVariant>
#include <QHash>


class IListModelItem
{
public:
    virtual ~IListModelItem() = default;
};


// TODO: review methods use, remove extra methods
class IListModel
{
public:
    virtual ~IListModel() = default;
    virtual QObject* getModel() const = 0;
    virtual void initResponse() = 0;
    virtual const QString &getQMLLayoutName() const = 0;
    virtual const QString &getJsonLayoutName() const = 0;
    virtual QString getLayoutIdFiledName() const = 0;
    virtual bool containsAppId(const QVariant &appId_) const = 0;
    virtual QVariant getCurrentIndexAppId() const = 0;
    virtual bool getValuesForAppId(const QVariant &appId_,
                                   QHash<QString, QVariant> &values_) const = 0;
    virtual QVariant getIdFieldValueForAppId(const QVariant &appId_) const = 0;
    virtual QVariant getFieldValueForAppId(const QVariant &appId_,
                                           const QString &jsonFieldName) const = 0;
};


class IListModelChangeNotify
{
public:
    virtual ~IListModelChangeNotify() = default;
    virtual void listLoadedVF() = 0;
    virtual void itemAddedVF() = 0;
    virtual void itemSetVF() = 0;
    virtual void itemDeletedVF() = 0;
    virtual void errorVF(int errorCode_,
                         const QString &errorCodeStr_,
                         const QJsonDocument &reply_) = 0;
    virtual void objectLoadedVF() = 0;
};
Q_DECLARE_METATYPE(IListModelChangeNotify*)


class IListModelInfo : public IListModelChangeNotify
{
public:
    virtual void startLoadChildModel() = 0;
    virtual void endLoadChildModel() = 0;
    virtual bool isListLoadedImpl() const = 0;
    virtual void loadChildrenVF() = 0;
};
Q_DECLARE_METATYPE(IListModelInfo*)


class IListModelInfoObjectImpl : public IListModelInfo
{
public:
    virtual void startLoadChildModel() override;
    virtual void endLoadChildModel() override;
    virtual bool isListLoadedImpl() const override;
    virtual void listLoadedVF() override;
    virtual void itemAddedVF() override;
    virtual void itemSetVF() override;
    virtual void itemDeletedVF() override;
    virtual void errorVF(int errorCode_,
                         const QString &errorCodeStr_,
                         const QJsonDocument &reply_) override;
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

protected:
    void setParentModelInfo(IListModelInfo *parentListModelInfo_);
    void setObjectName(const QString &objName_);
    void trace();

private:
    int m_loadingChildenModels = 0;
    IListModelInfo *m_parentListModelInfo = nullptr;
    bool m_childrenLoaded = false;
    bool m_childrenLoading = false;
    QString m_objectName;
};


#endif // IMODEL_H
