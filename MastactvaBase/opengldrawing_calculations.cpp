#include "opengldrawing_calculations.h"
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


opengl_drawing::WalkEffectRectMatrixCalculation::WalkEffectRectMatrixCalculation()
{
    setFilename(g_renderWalkEffectRectMatrixCalculation);
}

opengl_drawing::WalkEffectRectMatrixCalculation::~WalkEffectRectMatrixCalculation()
{
}

bool opengl_drawing::WalkEffectRectMatrixCalculation::init(const QString &args_)
{
    const QStringList args = removeEmpty(trimmed(args_.split(g_argumentsSplitter)));
    if(args.size() < 2)
    {
        return false;
    }
    m_renderImageName = args[0].trimmed();
    m_rectMatrixUniformName = args[1].trimmed();

    setRequiredVariables({
                             g_renderWindowSizeName,
                             m_renderImageName,
                         });
    setFilename(QString(g_renderWalkEffectRectMatrixCalculation)
                + QString("_") + m_renderImageName
                + QString("_") + m_rectMatrixUniformName
                );
    return !m_renderImageName.isEmpty()
            && !m_rectMatrixUniformName.isEmpty()
            ;
}

void opengl_drawing::WalkEffectRectMatrixCalculation::calculate(opengl_drawing::IVariables *variables_) const
{
    if(!variables_)
    {
        return;
    }

    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_->getRoot());
    if(!objects)
    {
        return; // work only with quiz image opengl drawing data
    }

    //const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    //const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    //const QSize imageSize = objects->getTextureSize(m_renderImageName , windowSize);

    //const QMatrix4x4 m = opengl_drawing::calculatePreserveAspectFitTextureMatrix(
    //            imageSize,
    //            windowSize
    //            );

    objects->setUniform(
                m_rectMatrixUniformName,
                QMatrix4x4{} // m
                );
}

opengl_drawing::WalkEffectClipRectCalculation::WalkEffectClipRectCalculation()
{
    setFilename(g_renderWalkEffectClipRectCalculation);
}

opengl_drawing::WalkEffectClipRectCalculation::~WalkEffectClipRectCalculation()
{
}

bool opengl_drawing::WalkEffectClipRectCalculation::init(const QString &args_)
{
    const QStringList args = removeEmpty(trimmed(args_.split(g_argumentsSplitter)));
    if(args.size() < 2)
    {
        return false;
    }
    m_renderFromImageName = args[0].trimmed();
    m_renderToImageName = args[1].trimmed();

    setRequiredVariables({
                             g_renderWindowSizeName,
                             m_renderFromImageName,
                             m_renderToImageName
                         });
    return !m_renderFromImageName.isEmpty()
            && !m_renderToImageName.isEmpty()
            ;
}

void opengl_drawing::WalkEffectClipRectCalculation::calculate(IVariables *variables_) const
{
    opengl_drawing::Objects *objects = variables_ ? dynamic_cast<opengl_drawing::Objects *>(variables_->getRoot()) : nullptr;
    if(!objects)
    {
        return;
    }

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    const QVector4D clipRectFromImage = getImageClipRect(objects, m_renderFromImageName, windowSize );
    const QVector4D clipRectToImage = getImageClipRect(objects, m_renderToImageName, windowSize );
    const QVector4D clipRect = minClipRect(clipRectFromImage, clipRectToImage);

    qDebug() << clipRect;
    objects->setUniform(
                QString(g_renderObjectsStateStartName) + QString(g_renderClipRectStateName),
                clipRect
                );
}

QVector4D opengl_drawing::WalkEffectClipRectCalculation::getImageClipRect(
        opengl_drawing::Objects *objects_,
        const QString &imageName_,
        const QSize &windowSize_
        ) const
{
    const QSize imageSize = objects_->getTextureSize(imageName_ , windowSize_);
    const QMatrix4x4 m = ::opengl_drawing::calculatePreserveAspectFitTextureMatrix(imageSize, windowSize_, true);
    const QVector4D topLeftPt{0.0, 0.0, 0.0, 1.0};
    const QVector4D bottomRightPt{(float)windowSize_.width(), (float)windowSize_.height(), 0.0, 1.0};
    const QVector4D topLeftPtT = m * topLeftPt;
    const QVector4D bottomRightPtT = m * bottomRightPt;
    return QVector4D{topLeftPtT.x(), topLeftPtT.y(), bottomRightPtT.x(), bottomRightPtT.y()};
}

QVector4D opengl_drawing::WalkEffectClipRectCalculation::minClipRect(const QVector4D &cr1_, const QVector4D &cr2_) const
{
    return QVector4D{
        std::min(cr1_.x(), cr2_.x()), std::min(cr1_.y(), cr2_.y()),
        std::max(cr1_.z(), cr2_.z()), std::max(cr1_.w(), cr2_.w())
        };
}


