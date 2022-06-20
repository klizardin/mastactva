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


opengl_drawing::ImagesGeometryMatrixCalculation::ImagesGeometryMatrixCalculation()
{
    setFilename(g_renderImageGeometryMatrixMultipleCalculation);
}

opengl_drawing::ImagesGeometryMatrixCalculation::~ImagesGeometryMatrixCalculation()
{
}

bool opengl_drawing::ImagesGeometryMatrixCalculation::init(const QString &args_)
{
    QStringList args = removeEmpty(trimmed(args_.split(g_argumentsSplitter)));
    if(args.size() < 1)
    {
        return false;
    }
    m_renderImages = std::move(args);
    setRequiredVariables(QStringList{
                             g_renderWindowSizeName
                         } << m_renderImages);
    return !m_renderImages.isEmpty();
}

void opengl_drawing::ImagesGeometryMatrixCalculation::calculate(IVariables *variables_) const
{
    opengl_drawing::Objects *objects = variables_ ? dynamic_cast<opengl_drawing::Objects *>(variables_->getRoot()) : nullptr;
    if(!objects)
    {
        return;
    }

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    const QVector<QRectF> imagesRects = getImageRects(objects, m_renderImages, windowSize);

    objects->setUniform( g_renderMatrixName, getGeometryMatrix(objects, windowSize, imagesRects));
}

QVector<QRectF> opengl_drawing::ImagesGeometryMatrixCalculation::getImageRects(
        opengl_drawing::Objects *objects_,
        const QStringList &imageNames_,
        const QSize &windowSize_
        ) const
{
    QVector<QRectF> rects;
    for(const QString &imageName_ : qAsConst(imageNames_))
    {
        const QSize imageSize = objects_->getTextureSize(imageName_ , windowSize_);
        rects.push_back(QRectF{0.0, 0.0, float(imageSize.width()), float(imageSize.height())});
    }
    return rects;
}

QMatrix4x4 opengl_drawing::ImagesGeometryMatrixCalculation::getGeometryMatrix(
        opengl_drawing::Objects *objects_,
        const QSize &windowSize_,
        const QVector<QRectF> &imagesRects_
        ) const
{
    Q_UNUSED(objects_);
    const auto maxImageWidthRect = std::max_element(
                std::begin(imagesRects_),
                std::end(imagesRects_),[](const QRectF &rc1_, const QRectF &rc2_) -> bool
    {
        return rc1_.width() > rc2_.width();
    });
    const auto maxImageHeightRect = std::max_element(
                std::begin(imagesRects_),
                std::end(imagesRects_),[](const QRectF &rc1_, const QRectF &rc2_) -> bool
    {
        return rc1_.height() > rc2_.height();
    });
    const QSizeF maxImageRect{maxImageWidthRect->width(), maxImageHeightRect->height()};
    const float imageRel = maxImageRect.width()/maxImageRect.height();
    const float windowRel = (float)windowSize_.width()/(float)windowSize_.height();
    const float imageRelHeight = 1.0 / imageRel;
    const float windowRelHeight = 1.0 / windowRel;

    float x0 = -1.0;
    float x1 = 1.0;
    float y0 = -1.0;
    float y1 = 1.0;

    if(imageRel < windowRel)
    {
        const float shift = (windowRel - imageRel) / imageRel * 0.5 * (x1 - x0);
        x0 -= shift;
        x1 += shift;
    }
    else if(imageRelHeight < windowRelHeight)
    {
        const float shift = (windowRelHeight - imageRelHeight) / imageRelHeight * 0.5 * (x1 - x0);
        y0 -= shift;
        y1 += shift;
    }

    QMatrix4x4 renderMatrix;
    renderMatrix.ortho(x0, x1, -y0, -y1, -1.0, 1.0);
    return renderMatrix;
}
