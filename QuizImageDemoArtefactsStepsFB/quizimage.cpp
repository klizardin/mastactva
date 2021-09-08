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

#include "quizimage.h"
#include <math.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtQuick/QQuickWindow>
#include <qsgsimpletexturenode.h>
#include <QRandomGenerator>
#include <QTime>
#include <QVector>
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "drawing_tests.h"
#include "object_tests.h"
#include "../MastactvaBase/utils.h"


QuizImage::QuizImage()
{
    initDefaultDrawingData();
}

QQuickFramebufferObject::Renderer *QuizImage::createRenderer() const
{
    return new QuizImageFboRenderer<QuizImage>();
}

void QuizImage::updateState()
{
    update();
}

void QuizImage::updateProject()
{
    loadProject();
}

qreal QuizImage::t() const
{
    return m_t;
}

void QuizImage::setT(const qreal &t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

QString QuizImage::fromImage() const
{
    return m_fromImage;
}

void QuizImage::setFromImage(const QString &fromImage_)
{
    m_fromImage = fromImage_;
    loadProject();

    updateState();
    emit fromImageChanged();
}

QString QuizImage::toImage() const
{
    return m_toImage;
}

void QuizImage::setToImage(const QString &toImage_)
{
    m_toImage = toImage_;
    loadProject();

    updateState();
    emit toImageChanged();
}

QString QuizImage::project() const
{
    return m_project;
}

void QuizImage::setProject(const QString &project_)
{
    m_project = project_;

    loadProject();
    updateState();
    emit projectChanged();
}

QString QuizImage::log() const
{
    return m_compilerLog;
}

void QuizImage::setLog(const QString &log_)
{
    m_compilerLog = log_;

    emit logChanged();
}

void QuizImage::freeProject()
{
    m_drawingData.reset(nullptr);
    m_drawingOldData.reset(nullptr);
}

void QuizImage::loadProject()
{
    freeProject();
    if(!m_project.isEmpty())
    {
        QUrl url(project());
        initDefaultDrawingData();
        //m_drawingData = OpenGLDrawingImageData::fromJson(
        //            QJsonDocument::fromJson(
        //                loadBinaryFile(url.toLocalFile())
        //                )
        //            );
    }
    setProjectFromImage();
    setProjectToImage();
}

void QuizImage::setProjectFromImage()
{
    if(!m_drawingData ||
            fromImage().isEmpty()
            ) { return; }
    // do not remove prev image
    QUrl url(fromImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), true);
}

void QuizImage::setProjectToImage()
{
    if(!m_drawingData ||
            toImage().isEmpty()
            ) { return; }
    // do not remove prev image
    QUrl url(toImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), false);
}

bool QuizImage::isImageDataUpdated() const
{
    return m_drawingData.operator bool();
}

std::unique_ptr<drawing_data::QuizImageObjects> QuizImage::getData()
{
    return std::move(m_drawingData);
}

void QuizImage::setDataToFree(std::unique_ptr<drawing_data::QuizImageObjects> &&old_)
{
    m_drawingOldData = std::move(old_);
}

void QuizImage::renderBuildError(const QString &compilerLog_)
{
    setLog(compilerLog_);
}

void QuizImage::initDefaultDrawingData()
{
    std::unique_ptr<drawing_data::QuizImageObjects> data(new drawing_data::QuizImageObjects());
    TestCaseInitializer defaultData;
    defaultData.initialize(*data.get());
    m_drawingData = std::move(data);
}

bool QuizImage::isFromImageReady() const
{
    return true;
}

bool QuizImage::isToImageReady() const
{
    return true;
}

const QString &QuizImage::getFromImageUrl() const
{
    return m_fromImage;
}

const QString &QuizImage::getToImageUrl() const
{
    return m_toImage;
}
