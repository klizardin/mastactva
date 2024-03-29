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
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtCore/QDebug>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSGRenderNode>
#include <QTextCodec>
#include "openglquizimage.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"


QuizImage::QuizImage(QQuickItem *parent_ /*= nullptr*/)
    : QQuickItem(parent_)
{
    setFlag(ItemHasContents);
}

void QuizImage::updateState()
{
    update();
}

void QuizImage::updateShaders()
{
    m_shadersUpdated = true;
    updateState();
}

void QuizImage::updateArguments()
{
    m_argumentsUpdated = true;
    updateState();
}

QString QuizImage::fromImage() const
{
    return m_fromImage;
}

void QuizImage::setFromImage(const QString &fromImage_)
{
    QUrl url(fromImage_);
    const QString fromImageFile = g_noImage != fromImage_ ? url.toLocalFile() : g_noImage;

    if(fromImageFile == m_fromImage) { return; }

    m_fromImage = fromImageFile;

    delete m_image;
    m_image = nullptr;

    // update image size
    m_image = new QImage(m_fromImage);
    const QSize imageOrigSize = m_image->size();
    const qreal imageRate = (qreal)std::max(1,imageOrigSize.width())/(qreal)std::max(1,imageOrigSize.height());
    const qreal rectRate = std::max(1.0, width())/std::max(1.0, height());
    if(rectRate > imageRate)
    {
        m_imageSize = QSize(height()*imageRate, height());
    }
    else
    {
        m_imageSize = QSize(width(), width()/imageRate);
    }

    updateState();
    emit fromImageChanged();
    emit paintedWidthChanged();
    emit paintedHeightChanged();
}

QString QuizImage::toImage() const
{
    return m_toImage;
}

void QuizImage::setToImage(const QString &toImage_)
{
    QUrl url(toImage_);
    const QString toImageFile = g_noImage != toImage_ ? url.toLocalFile() : g_noImage;

    if(toImageFile == m_toImage) { return; }

    m_toImage = toImageFile;

    updateState();
    emit toImageChanged();
}

QString QuizImage::fragmentShaderFilename()
{
    return m_fragmentShaderFilename;
}

void QuizImage::setFragmentShaderFilename(const QString &fragmentShaderFilename_)
{
    QUrl url(fragmentShaderFilename_);

    m_fragmentShaderFilename = url.toLocalFile();

    const QString fragmentShaderNew = loadFile(m_fragmentShaderFilename);
    const QString newHash = calculateHash(fragmentShaderNew);

    if(newHash == m_fragmentShaderHash) { return; }

    m_fragmentShader = fragmentShaderNew;
    m_fragmentShaderHash = newHash;

    emit fragmentShaderFilenameChanged();
    emit fragmentShaderChanged();
}

QString QuizImage::vertexShaderFilename()
{
    return m_vertexShaderFilename;
}

void QuizImage::setVertexShaderFilename(const QString &vertexShaderFilename_)
{
    QUrl url(vertexShaderFilename_);

    m_vertexShaderFilename = url.toLocalFile();

    const QString vertexShaderNew = loadFile(m_vertexShaderFilename);
    const QString newHash = calculateHash(vertexShaderNew);

    if(newHash == m_vertexShaderHash) { return; }

    m_vertexShader = vertexShaderNew;
    m_vertexShaderHash = newHash;

    emit vertexShaderFilenameChanged();
    emit vertexShaderChanged();
}

QString QuizImage::fragmentShader()
{
    return m_fragmentShader;
}

void QuizImage::setFragmentShader(const QString &fragmentShader_)
{
    const QString newHash = calculateHash(fragmentShader_);
    if(newHash == m_fragmentShaderHash) { return; }

    m_fragmentShader = fragmentShader_;
    m_fragmentShaderHash = newHash;

    if(!m_fragmentShaderFilename.isEmpty())
    {
        saveFile(m_fragmentShaderFilename, m_fragmentShader);
    }

    emit fragmentShaderChanged();
}

QString QuizImage::vertexShader()
{
    return m_vertexShader;
}

void QuizImage::setVertexShader(const QString &vertexShader_)
{
    const QString newHash = calculateHash(vertexShader_);
    if(newHash == m_vertexShaderHash) { return; }

    m_vertexShader = vertexShader_;
    m_vertexShaderHash = newHash;

    if(!m_vertexShaderFilename.isEmpty())
    {
        saveFile(m_vertexShaderFilename, m_vertexShader);
    }

    emit vertexShaderChanged();
}

qreal QuizImage::t() const
{
    return m_t;
}

void QuizImage::setT(qreal t_)
{
    m_t = t_;

    updateState();
    emit tChanged();
}

int QuizImage::paintedWidth() const
{
    return std::max(1, m_imageSize.width());
}

int QuizImage::paintedHeight() const
{
    return std::max(1, m_imageSize.height());
}

bool QuizImage::shadersUpdated() const
{
    return m_shadersUpdated;
}

void QuizImage::clearShadersUpdated()
{
    m_shadersUpdated = false;
}

bool QuizImage::argumentsUpdated() const
{
    return m_argumentsUpdated;
}

void QuizImage::clearArgumentsUpdated()
{
    m_argumentsUpdated = false;
}

QString QuizImage::shadersBuildLog()
{
    return m_renderBuildLog;
}

void QuizImage::setShadersBuildLog(const QString &log_)
{
    m_lastLog = log_;
    m_renderBuildLog = m_lastLog;

    emit shadersBuildLogChanged();
}

void QuizImage::renderBuildError(const QString &log_)
{
    if(log_ == m_lastLog) { return; }

    m_lastLog = log_;
    m_renderBuildLog += m_lastLog;

    emit shadersBuildLogChanged();
}

QSGNode *QuizImage::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGRenderNode *n = static_cast<QSGRenderNode *>(node);

    QSGRendererInterface *ri = window()->rendererInterface();
    if (!ri)
        return nullptr;

    switch (ri->graphicsApi()) {
    case QSGRendererInterface::OpenGL:
        Q_FALLTHROUGH();
    case QSGRendererInterface::OpenGLRhi:
#if QT_CONFIG(opengl)
        if (!n)
        {
            n = new OpenGlQuizImage(this);
        }
        static_cast<OpenGlQuizImage *>(n)->sync(this);
#endif
        break;

    case QSGRendererInterface::MetalRhi:
// Restore when QTBUG-78580 is done and the .pro is updated accordingly
//#ifdef Q_OS_DARWIN
        break;

    case QSGRendererInterface::Direct3D12: // ### Qt 6: remove
        break;

    case QSGRendererInterface::Software:
        break;

    default:
        break;
    }

    if (!n)
        qWarning("QSGRendererInterface reports unknown graphics API %d", ri->graphicsApi());

    return n;
}

QString QuizImage::loadFile(const QString &filename_)
{
    QFile file(filename_);
    if(!file.open(QIODevice::ReadOnly)) { return QString(); }
    QByteArray fd = file.readAll();
    QTextCodec *codec = QTextCodec::codecForUtfText(fd);
    return codec->toUnicode(fd);
}

void QuizImage::saveFile(const QString &filename_, const QString& data_)
{
    QFile file(filename_);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) { return; }
    file.write(data_.toUtf8());
}
