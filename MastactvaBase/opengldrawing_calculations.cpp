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
    if(args.size() < 3)
    {
        return false;
    }
    m_vertexAttributeName = args[0].trimmed();
    m_renderImageName = args[1].trimmed();
    m_rectMatrixUniformName = args[2].trimmed();
    setRequiredVariables({
                             g_renderWindowSizeName,
                             m_vertexAttributeName,
                             m_renderImageName,
                         });
    setFilename(QString(g_renderWalkEffectRectMatrixCalculation)
                + QString("_") + m_vertexAttributeName
                + QString("_") + m_renderImageName
                + QString("_") + m_rectMatrixUniformName
                );
    return !m_vertexAttributeName.isEmpty()
            && !m_renderImageName.isEmpty()
            && !m_rectMatrixUniformName.isEmpty()
            ;
}

QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_,
        const QVector<QVector2D> &srcPts_
        )
{
    QMatrix4x4 textureMatrix;
    const float iw = std::max(1.0f, (float)imageSize_.width());
    const float ih = std::max(1.0f, (float)imageSize_.height());
    const float imageRate = iw / ih;
    const float sw = std::max(1.0f, (float)rectSize_.width());
    const float sh = std::max(1.0f, (float)rectSize_.height());
    const float rectRate = sw / sh;

    //qDebug() << "iw" << iw << "ih" << ih << "sw" << sw << "sh" << sh << "imageRate" << imageRate << "rectRate" << rectRate;

    if(rectRate >= 1.0)
    {
        const float scaleX = 1.0f;
        const float scaleY = rectRate;
        textureMatrix.scale( scaleX, scaleY);
    }
    else
    {
        const float scaleX = 1.0f / rectRate;
        const float scaleY = 1.0f;
        textureMatrix.scale( scaleX, scaleY);
    }
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 1.0f;
    float y1 = 1.0f;

    if(rectRate >= imageRate)
    {
        const float shift = (1.0 - imageRate/rectRate) * 0.5f;
        x0 += shift;
        x1 -= shift;
        //qDebug() << "x0" << x0 << "x1" << x1;
        x0 += srcPts_[0].x() * (x1 - x0);
        x1 += (srcPts_[1].x() - 1.0f) * (x1 - x0);
        y0 += srcPts_[0].y();
        y1 += (srcPts_[1].y() - 1.0f);
        //qDebug() << "shift" << shift;
    }
    else
    {
        const float shift = (1.0 - rectRate/imageRate) * 0.5f;
        y0 += shift;
        y1 -= shift;
        y0 += srcPts_[0].y() * (y1 - y0);
        y1 += (srcPts_[1].y() - 1.0f) * (y1 - y0);
        x0 += srcPts_[0].x();
        x1 += (srcPts_[1].x() - 1.0f);
        //qDebug() << "shift" << shift;
    }
    const QVector<QVector2D> srcPts =
    {
        {srcPts_[0].x(), srcPts_[0].y()},
        {srcPts_[1].x(), srcPts_[1].y()},
        {srcPts_[0].x(), srcPts_[1].y()},
        {srcPts_[1].x(), srcPts_[0].y()}
    };
    const QVector<QVector2D> destPts =
    {
        {x0, y0},
        {x1, y1},
        {x0, y1},
        {x1, y0}
    };
    const QMatrix4x4 m1 = opengl_drawing::calculateTransfromMatrixBy4Points(destPts, srcPts);
    //qDebug() << "m1" << m1 << "destPts" << destPts;
    return m1 * textureMatrix;
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

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    const QSize imageSize = objects->getTextureSize(m_renderImageName , windowSize);

    std::vector<GLfloat> vertexDataRow;
    objects->getArgumentValue(m_vertexAttributeName, vertexDataRow);
    std::vector<QVector4D> vertexData;
    drawingdata::utils::vecToAttribute(vertexDataRow, vertexData);

    // qDebug() << "vertexDataRow" << vertexDataRow;
    // qDebug() << "vertexData" << vertexData;

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

    const QVector<QVector2D> srcPts =
    {
        {x0, y0},
        {x1, y1},
    };

    //qDebug() << "srcPts" << srcPts;

    const QMatrix4x4 m = calculatePreserveAspectFitTextureMatrix(
                imageSize,
                windowSize,
                srcPts
                );

    std::vector<QVector4D> vertexDataM;
    vertexDataM.resize(vertexData.size());
    std::transform(
                std::begin(vertexData), std::end(vertexData),
                std::begin(vertexDataM),
                [&m](const QVector4D &pt_)->QVector4D
    {
        return m*pt_;
    });
    qDebug() << "vertexData : " << m_vertexAttributeName << vertexData;
    qDebug() << "vertexDataM : " << m_vertexAttributeName << vertexDataM;

    objects->setUniform(
                m_rectMatrixUniformName,
                m
                );
}
