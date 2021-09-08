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

#ifndef QUESTIONANSWERMODELVIEW_H
#define QUESTIONANSWERMODELVIEW_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaModels/questionanswer.h"
#include "../MastactvaModels/gallerystatistics.h"
#include "../MastactvaModels/imagepoint.h"
#include "../MastactvaModels/image.h"


class QuestionAnswerModelView : public QObject, public ILocalDataUpdateAPI
{
    Q_OBJECT
public:
    QuestionAnswerModelView(QObject * parent_ = nullptr);
    virtual ~QuestionAnswerModelView() override;

    virtual bool canProcess(const DBRequestBase *r_) const override;
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool delItemImpl(const QVariant &id_,
                             DBRequestBase *r_) override;

protected:
    void addUserQuestionAnswer();
    void addUserQuestionAnswerItem();
    void error();

protected slots:
    void galleryStatisticsModelListReloaded();
    void userQuestionAnswerModelListReloaded();
    void galleryStatisticsModelItemAdded();
    void galleryStatisticsModelItemSet();

private:
    UserQuestionAnswerModel *m_userQuestionAnswerModel = nullptr;
    GalleryStatisticsModel *m_galleryStatisticsModel = nullptr;
    QList<QPair<DBRequestBase *, UserQuestionAnswer *>> m_requests;
    GalleryStatistics *m_galleryStatistics = nullptr;
};

#endif // QUESTIONANSWERMODELVIEW_H
