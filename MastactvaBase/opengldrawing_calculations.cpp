#include "opengldrawing_calculations.h"
#include "../MastactvaBase/quizimageopengldrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


opengl_drawing::WalkEffectRectMatrixCalculation::WalkEffectRectMatrixCalculation()
{
}

opengl_drawing::WalkEffectRectMatrixCalculation::~WalkEffectRectMatrixCalculation()
{
}

bool opengl_drawing::WalkEffectRectMatrixCalculation::init(const QString &args_)
{
    const QStringList args = args_.split(g_renderCalculationsSpliter);
    if(args.size() < 3)
    {
        return false;
    }
    m_vertexAttributeName = args[0].trimmed();
    m_renderImageName = args[1].trimmed();
    m_rectMatrixUniformName = args[2].trimmed();
    return !m_vertexAttributeName.isEmpty()
            && !m_renderImageName.isEmpty()
            && !m_rectMatrixUniformName.isEmpty()
            ;
}

QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_,
        const QRectF &srcRect_,
        const QRectF &destRect_
        )
{
    QMatrix4x4 textureMatrix;
    const qreal imageRate = (qreal)std::max(1, imageSize_.width())
            / (qreal)std::max(1, imageSize_.height())
            ;
    const qreal rectRate = (qreal)std::max(1, rectSize_.width())
            / (qreal)std::max(1, rectSize_.height())
            ;
    if(rectRate >= imageRate)
    {
        textureMatrix.scale(rectRate/imageRate, 1.0);
        textureMatrix.translate(-(rectRate - imageRate)/rectRate*0.5, 0.0);
    }
    else
    {
        textureMatrix.scale(1.0, imageRate/rectRate);
        textureMatrix.translate(0.0, -(imageRate - rectRate)/imageRate*0.5);
    }
    return textureMatrix;
}

void opengl_drawing::WalkEffectRectMatrixCalculation::calculate(opengl_drawing::IVariables *variables_) const
{
    opengl_drawing::Objects *objects = dynamic_cast<opengl_drawing::Objects *>(variables_);
    if(!objects)
    {
        return; // work only with quiz image opengl drawing data
    }

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    const QSize imageSize = objects->getTextureSize(m_renderImageName , windowSize);

    std::vector<GLfloat> vertexDataRow;
    objects->getArgumentValue(m_vertexAttributeName, vertexDataRow);
    std::vector<QVector4D> vertexData;
    drawingdata::utils::vecToAttribute(vertexDataRow, vertexData);

    const auto minmax_x = std::minmax_element(
                std::begin(vertexData),
                std::end(vertexData),
                [](const QVector4D &v1_, const QVector4D &v2_)
    {
        return v1_.x() < v2_.x();
    });
    const auto minmax_y = std::minmax_element(
                std::begin(vertexData),
                std::end(vertexData),
                [](const QVector4D &v1_, const QVector4D &v2_)
    {
        return v1_.y() < v2_.y();
    });
    const float x0 = minmax_x.first->x();
    const float x1 = minmax_x.second->x();
    const float y0 = minmax_y.first->y();
    const float y1 = minmax_y.second->y();

    const float dx0 = x0 * imageSize.width();
    const float dx1 = x1 * imageSize.width();
    const float dy0 = y0 * imageSize.height();
    const float dy1 = y1 * imageSize.height();

    objects->setUniform(
                m_rectMatrixUniformName,
                calculatePreserveAspectFitTextureMatrix(
                    imageSize,
                    windowSize,
                    QRectF(x0, y0 , x1 - x0, y1 - y0),
                    QRectF(dx0, dy0 , dx1 - dx0, dy1 - dy0)
                    )
                );
}
