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
#include <QVector3D>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QRandomGenerator>
#include <QTime>
#include <QVector>
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "drawing_tests.h"
#include "object_tests.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/iquizimageqwindow.h"


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

QString QuizImage::renderingTextureName() const
{
    return m_renderingTextureName;
}

void QuizImage::setRenderingTextureName(const QString &renderingTextureName_)
{
    m_renderingTextureName = renderingTextureName_;

    emit renderingTextureNameChanged();
}

void QuizImage::freeProject()
{
    m_drawingData.reset();
    m_drawingOldData.reset();
}

void QuizImage::loadProject()
{
    freeProject();
    if(!m_project.isEmpty())
    {
        //QUrl url(project());
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
    //QUrl url(fromImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), true);
}

void QuizImage::setProjectToImage()
{
    if(!m_drawingData ||
            toImage().isEmpty()
            ) { return; }
    // do not remove prev image
    //QUrl url(toImage());
    //m_drawingData->addRenderImage(url.toLocalFile(), false);
}

bool QuizImage::isImageDataUpdated() const
{
    return m_drawingData.operator bool();
}

std::shared_ptr<drawing_data::QuizImageObjects> QuizImage::getData()
{
    return std::move(m_drawingData);
}

void QuizImage::setDataToFree(std::shared_ptr<drawing_data::QuizImageObjects> &&old_)
{
    m_drawingOldData = std::move(old_);
}

void QuizImage::renderBuildError(const QString &compilerLog_)
{
    setLog(compilerLog_);
}


template<int index_>
class Initialize : public Initialize<index_ - 1>
{
    using base = Initialize<index_ - 1>;
public:
    template<class TupleType_>
    void initialize(const TupleType_ tuple_, int i_, drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0)
    {
        if(i_ == index_)
        {
            std::get<index_>(tuple_).initialize(data_, argsSetIndex_);
        }
        else
        {
            base::initialize(tuple_, i_, data_, argsSetIndex_);
        }
    }
};

template<>
class Initialize<0>
{
public:
    template<class TupleType_>
    void initialize(const TupleType_ tuple_, int i_, drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0)
    {
        if(i_ == 0)
        {
            std::get<0>(tuple_).initialize(data_, argsSetIndex_);
        }
        else
        {
            Q_ASSERT(false); // index i_ not found
        }
    }
};


void QuizImage::initDefaultDrawingData()
{
    /*
     * QuizImage creates textures
     * 1. should be one QuizImage before initialization
     * how to do it?
     * Setup main QuizImage and ignore all others?
     * How to setup main?
     * First?
     * Who knows about neighborhuds? (interface)
     * So here we add all, but interface will filter calls
    */

    if(IQuizImageQWindow::getInstance()
            && IQuizImageQWindow::getInstance()->getDrawingData()
            )
    {
        //m_drawingData = IQuizImageQWindow::getInstance()->getDrawingData();
    }

    if(m_testIndex < 0)
    {
        std::shared_ptr<drawing_data::QuizImageObjects> data = std::make_shared< drawing_data::QuizImageObjects>();
        TestCaseInitializer defaultData;
        defaultData.initialize(*data.get());
        m_drawingData = std::move(data);
    }
    else
    {
        std::shared_ptr<drawing_data::QuizImageObjects> data = std::make_shared< drawing_data::QuizImageObjects>();
        Tests tests;
        Initialize<std::tuple_size<Tests>::value - 1> init;
        init.initialize(tests, m_testIndex, *data.get());
        m_drawingData = std::move(data);
    }

    if(IQuizImageQWindow::getInstance())
    {
        //IQuizImageQWindow::getInstance()->setDrawingData(m_drawingData);
        IQuizImageQWindow::getInstance()->setTextures(m_drawingData->getTargetTextures());
    }
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

int QuizImage::testIndex() const
{
    return m_testIndex;
}

void QuizImage::setTestIndex(const int &testIndex_)
{
    m_testIndex = testIndex_;
    m_testIndex %= std::tuple_size<Tests>::value;

    initDefaultDrawingData();

    emit testIndexChanged();
}

int QuizImage::renderingWindowsId() const
{
    return m_renderingWindowsId;
}

void QuizImage::setRenderingWindowsId(int id_)
{
    m_renderingWindowsId = id_;

    emit renderingWindowsIdChanged();
}

bool QuizImage::isDefaultTexture() const
{
    return TextureNames::isDefaultTexcture(m_renderingTextureName);
}


QuizImages &QuizImages::getInstance()
{
    static QuizImages _;
    return _;
}

void QuizImages::add(QuizImage *quizImage)
{
    const auto fit = std::find(std::begin(m_quizImages), std::end(m_quizImages), quizImage);
    if(std::end(m_quizImages) != fit)
    {
        return;
    }
    m_quizImages.push_back(quizImage);
}

void QuizImages::remove(QuizImage *quizImage)
{
    const auto fit = std::find(std::begin(m_quizImages), std::end(m_quizImages), quizImage);
    if(std::end(m_quizImages) == fit)
    {
        return;
    }
    m_quizImages.erase(fit);
}
