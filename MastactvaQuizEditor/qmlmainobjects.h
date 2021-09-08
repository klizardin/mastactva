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

#ifndef QMLMAINOBJECTS_H
#define QMLMAINOBJECTS_H


#include <QObject>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"


class GalleryImagesModel;
class GalleryEditViewModel;
class DescriptionModel;
class QuestionAnswersModel;
class MastactvaAPI;
class QGuiApplication;


class QMLMainObjects : public QObject
{
    Q_OBJECT
public:
    explicit QMLMainObjects(QObject *parent_ = nullptr);
    virtual ~QMLMainObjects();

    void setRootQMLObject(QObject* root_);
    GalleryEditViewModel *getGalleryViewModel();
    GalleryImagesModel *getGalleryAllImagesModel();
    DescriptionModel *getImageOfGalleryDescriptionModel();
    QuestionAnswersModel *getQuestionAnswersModel();
    MastactvaAPI *getMastactvaAPI();
    QGuiApplication *getApp();
    void setApp(QGuiApplication *app_);

    static void createSingelton(QObject *parent_ = nullptr);
    static QMLMainObjects *getSingelton();

private:
    void searchQMLObjects();

signals:

private:
    static QMLMainObjects *g_singelton;

    QObject* m_root = nullptr;
    GalleryEditViewModel *m_galleryViewModel = nullptr;
    GalleryImagesModel *m_galleryAllImagesModel = nullptr;
    DescriptionModel *m_imageOfGalleryDescriptionModel = nullptr;
    QuestionAnswersModel *m_questionAnswersModel = nullptr;
    MastactvaAPI *m_mastactvaAPI = nullptr;
    QGuiApplication *m_app = nullptr;
};


class NetAPI;


class QMLObjects : public QMLObjectsBase
{
public:
    QMLObjects() = default;

    virtual void setInitialized() override;
    virtual void modelError(int errorCode_, const QString &errorCodeStr_, const QJsonDocument &reply_) override;

protected:
    virtual void searchObjects() override;

private:
    MastactvaAPI *m_mastactvaAPI = nullptr;
};


#endif // QMLMAINOBJECTS_H
