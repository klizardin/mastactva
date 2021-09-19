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

#ifndef LOCALDATASET_H
#define LOCALDATASET_H


#include <QObject>
#include <QtQuick/QQuickItem>


class GalleryModel;
class ImageModel;
class ArtefactTypeModel;
class ArtefactArgTypeModel;
class EasingTypeModel;
class UserStepModel;
class QuizUserModel;


class LocalDataSet : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit LocalDataSet(QObject * parent_ = nullptr);
    virtual ~LocalDataSet() override;

    Q_PROPERTY(QString savePath READ savePath WRITE setSavePath NOTIFY savePathChanged)

    Q_INVOKABLE void download();

protected:
    void create();
    void free();
    void downloadStep();
    qreal stepProgress();
    QString savePath() const;
    void setSavePath(const QString &url_);
    void archiveResults();

signals:
    void savePathChanged();
    void downloaded();
    void progress(qreal p_);

protected slots:
    void listReloadedSlot();

private:
    QString m_savePath;
    QString m_saveName;
    QString m_serverFilesOldRootDir;
    GalleryModel *m_galleryModel = nullptr;
    ImageModel *m_imageModel = nullptr;
    ArtefactTypeModel *m_artefactTypeModel = nullptr;
    ArtefactArgTypeModel *m_artefactArgTypeModel = nullptr;
    EasingTypeModel *m_easingTypeModel = nullptr;
    UserStepModel *m_userStepModel = nullptr;
    QuizUserModel *m_quizUserModel = nullptr;
    int m_step = 0;
    int m_galleryIndex = -1;
    int c_downloadStepsCount = 1;
};

#endif // LOCALDATASET_H
