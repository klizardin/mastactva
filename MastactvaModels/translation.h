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

#ifndef TRANSLATION_H
#define TRANSLATION_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class TranslationModel;


class Translation : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit Translation(TranslationModel *parent_ = nullptr);

    Q_PROPERTY(int translationId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int translationGalleryId READ galleryId WRITE setGalleryId NOTIFY galleryIdChanged)
    Q_PROPERTY(int translationDescriptionId READ descriptionId WRITE setDescriptionId NOTIFY descriptionIdChanged)
    Q_PROPERTY(int translationQuestionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(int translationAnswerId READ answerId WRITE setAnswerId NOTIFY answerIdChanged)
    Q_PROPERTY(QString translationText READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool translationAutoTranslate READ autoTranslate WRITE setAutoTranslate NOTIFY autoTranslateChanged)
    Q_PROPERTY(QString translationLanguageFrom READ langFrom WRITE setLangFrom NOTIFY langFromChanged)
    Q_PROPERTY(QString translationLanguageTo READ langTo WRITE setLangTo NOTIFY langToChanged)
    Q_PROPERTY(QDateTime translationCreated READ created WRITE setCreated NOTIFY createdChanged)


    class DefaultLayout : public LayoutBase<Translation>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("translation");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Translation::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &Translation::m_objectModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &Translation::m_appId);
            addField<int>("id", "translationId", &Translation::id, &Translation::setId);
            addField<int>("gallery", "translationGalleryId", &Translation::galleryId, &Translation::setGalleryId);
            addField<int>("description", "translationDescriptionId", &Translation::descriptionId, &Translation::setDescriptionId);
            addField<int>("question", "translationQuestionId", &Translation::questionId, &Translation::setQuestionId);
            addField<int>("answer", "translationAnswerId", &Translation::answerId, &Translation::setAnswerId);
            addField<QString>("text", "translationText", &Translation::text, &Translation::setText);
            addField<bool>("auto_translate", "translationAutoTranslate", &Translation::autoTranslate, &Translation::setAutoTranslate);
            addField<QString>("language_from", "translationLanguageFrom", &Translation::langFrom, &Translation::setLangFrom);
            addField<QString>("language_to", "translationLanguageTo", &Translation::langTo, &Translation::setLangTo);
            addField<QDateTime>("created", "translationCreated", &Translation::created, &Translation::setCreated);
            setIdField("id");
        }
    };


public:
    int id() const;
    void setId(const int &id_);
    int galleryId() const;
    void setGalleryId(const int &galleryId_);
    int descriptionId() const;
    void setDescriptionId(const int &descriptionId_);
    int questionId() const;
    void setQuestionId(const int &questionId_);
    int answerId() const;
    void setAnswerId(const int &answerId_);
    QString text() const;
    void setText(const QString &text_);
    bool autoTranslate() const;
    void setAutoTranslate(const bool &autoTranslate_);
    QString langFrom() const;
    void setLangFrom(const QString &langFrom_);
    QString langTo() const;
    void setLangTo(const QString &langTo_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);

signals:
    void idChanged();
    void galleryIdChanged();
    void descriptionIdChanged();
    void questionIdChanged();
    void answerIdChanged();
    void textChanged();
    void autoTranslateChanged();
    void langFromChanged();
    void langToChanged();
    void createdChanged();

private:
    TranslationModel *m_translationModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_galleryId = -1;
    int m_descriptionId = -1;
    int m_questionId = -1;
    int m_answerId = -1;
    QString m_text;
    bool m_autoTranslate = false;
    QString m_langFrom;
    QString m_langTo;
    QDateTime m_created;
};


class TranslationModel : public ListModelBaseOfData<Translation, TranslationModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Translation, TranslationModel>;

public:
    explicit TranslationModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
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


#endif // TRANSLATION_H
