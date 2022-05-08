#include <QCoreApplication>
#include <QSize>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QRandomGenerator>
#include <math.h>


bool calculateTransfromMatrixBy4Points(
        const QVector<QVector2D> &srcPts_,
        const QVector<QVector2D> &destPts_,
        QMatrix4x4 &res_
        );

QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_,
        bool inverse_ = false
        );

QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_,
        bool inverse_ /*= false*/
        )
{
    QMatrix4x4 textureMatrix;
    const float imageWidgth = std::max(1.0f, (float)imageSize_.width());
    const float imageHeight = std::max(1.0f, (float)imageSize_.height());
    const float imageRate = imageWidgth / imageHeight;
    const float screenWidth = std::max(1.0f, (float)rectSize_.width());
    const float screenHeight = std::max(1.0f, (float)rectSize_.height());
    const float screanRate = screenWidth / screenHeight;

    //qDebug() << "iw" << iw << "ih" << ih << "sw" << sw << "sh" << sh << "imageRate" << imageRate << "rectRate" << rectRate;

    if(screanRate >= 1.0)
    {
        const float scaleX = 1.0f;
        const float scaleY = screanRate;
        textureMatrix.scale( scaleX, scaleY);
    }
    else
    {
        const float scaleX = 1.0f / screanRate;
        const float scaleY = 1.0f;
        textureMatrix.scale( scaleX, scaleY);
    }
    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 1.0f;
    float y1 = 1.0f;

    if(screanRate >= imageRate)
    {
        const float shift = (1.0 - imageRate/screanRate) * 0.5f;
        //qDebug() << "shift" << shift;
        x0 += shift;
        x1 -= shift;
    }
    else
    {
        const float shift = (1.0 - screanRate/imageRate) * 0.5f;
        //qDebug() << "shift" << shift;
        y0 += shift;
        y1 -= shift;
    }
    const QVector<QVector2D> srcPts =
    {
        {0.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f}
    };
    const QVector<QVector2D> destPts =
    {
        {x0, y0},
        {x1, y1},
        {x0, y1},
        {x1, y0}
    };
    QMatrix4x4 m1;
    if(!(inverse_ ?
                calculateTransfromMatrixBy4Points(srcPts, destPts, m1)
              : calculateTransfromMatrixBy4Points(destPts, srcPts, m1)
            ))
    {
        return {};
    }
              ;
    if(inverse_)
    {
        qDebug() << "m1" << m1 << "destPts" << destPts;
    }
    return m1; // * textureMatrix;
}

bool calculateTransfromMatrixBy4Points(
        const QVector<QVector2D> &srcPts_,
        const QVector<QVector2D> &destPts_,
        QMatrix4x4 &res_
        )
{
    if(srcPts_.size() < 4 || destPts_.size() < 4)
    {
        return false;
    }
    if(fabs(srcPts_[0].y() - srcPts_[1].y()) < 1e-4
            || fabs(srcPts_[2].y() - srcPts_[3].y()) < 1e-4)
    {
        return false;  // reoder pts data, to make cross of lines (p0,p1) and (p2,p3)
    }
    const float sx1 = (destPts_[0].x() - destPts_[1].x())/(srcPts_[0].y() - srcPts_[1].y());
    const float sx2 = (destPts_[2].x() - destPts_[3].x())/(srcPts_[2].y() - srcPts_[3].y());
    const float sx3 = (srcPts_[0].x() - srcPts_[1].x())/(srcPts_[0].y() - srcPts_[1].y());
    const float sx4 = (srcPts_[2].x() - srcPts_[3].x())/(srcPts_[2].y() - srcPts_[3].y());
    const float sx = fabs(sx3 - sx4) < 1e-4 ? 1.0f : (sx1 - sx2)/(sx3 - sx4);
    const float kx = sx1 - sx3*sx;
    const float tx = destPts_[0].x() - srcPts_[0].x()*sx - srcPts_[0].y()*kx;

    if(fabs(srcPts_[0].x() - srcPts_[1].x()) < 1e-4
            || fabs(srcPts_[2].x() - srcPts_[3].x()) < 1e-4)
    {
        return false;  // reoder pts data, to make cross of lines (p0,p1) and (p2,p3)
    }
    const float sy1 = (destPts_[0].y() - destPts_[1].y())/(srcPts_[0].x() - srcPts_[1].x());
    const float sy2 = (destPts_[2].y() - destPts_[3].y())/(srcPts_[2].x() - srcPts_[3].x());
    const float sy3 = (srcPts_[0].y() - srcPts_[1].y())/(srcPts_[0].x() - srcPts_[1].x());
    const float sy4 = (srcPts_[2].y() - srcPts_[3].y())/(srcPts_[2].x() - srcPts_[3].x());
    const float sy = fabs(sy3 - sy4) < 1e-4 ? 1.0f : (sy1 - sy2)/(sy3 - sy4);
    const float ky = sy1 - sy3*sy;
    const float ty = destPts_[0].y() - srcPts_[0].x()*ky - srcPts_[0].y()*sy;

    res_ = QMatrix4x4(sx,   kx,     0.0f,   tx,
                      ky,   sy,     0.0f,   ty,
                      0.0f, 0.0f,   1.0f,   0.0f,
                      0.0f, 0.0f,   0.0f,   1.0f);
    return true;
}

void testScaleSizeShiftTopLeft()
{
    for(int i = 0; i < 100; i++)
    {
        QVector<QVector2D> src = {
            {0.0, 0.0},
            {1.0, 1.0},
            {0.0, 1.0},
            {1.0, 0.0}
        };
        const float x0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        QVector<QVector2D> dst = {
            {x0, y0},
            {x1, y1},
            {x0, y1},
            {x1, y0}
        };
        QMatrix4x4 m;
        if(!calculateTransfromMatrixBy4Points(src, dst, m))
        {
            continue;
        }
        const QVector<QVector4D> tst = {
            {src[0].x(), src[0].y(), 0.0, 1.0},
            {src[1].x(), src[1].y(), 0.0, 1.0},
            {src[2].x(), src[2].y(), 0.0, 1.0},
            {src[3].x(), src[3].y(), 0.0, 1.0},
        };
        const QVector<QVector4D> res = {
            {dst[0].x(), dst[0].y(), 0.0, 1.0},
            {dst[1].x(), dst[1].y(), 0.0, 1.0},
            {dst[2].x(), dst[2].y(), 0.0, 1.0},
            {dst[3].x(), dst[3].y(), 0.0, 1.0},
        };
        for(int j = 0; j < tst.size() && j < res.size(); ++j)
        {
            const QVector4D r = m * tst[j];
            assert((res[j] - r).length() < 1e-4);
        }
    }
}

void testScaleSizeShiftTopLeftTestInsideCoordinates()
{
    for(int i = 0; i < 100; i++)
    {
        QVector<QVector2D> src = {
            {0.0, 0.0},
            {1.0, 1.0},
            {0.0, 1.0},
            {1.0, 0.0}
        };
        const float x0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        QVector<QVector2D> dst = {
            {x0, y0},
            {x1, y1},
            {x0, y1},
            {x1, y0}
        };
        QMatrix4x4 m;
        if(!calculateTransfromMatrixBy4Points(src, dst, m))
        {
            continue;
        }
        QVector<QVector4D> tst;
        QVector<QVector4D> res;
        for(int i = 0; i < 20; ++i)
        {
            const float x = QRandomGenerator64::global()->generateDouble();
            const float y = QRandomGenerator64::global()->generateDouble();
            tst.push_back(QVector4D{x, y, 0.0, 1.0});
            res.push_back(QVector4D{x * (x1 - x0) + x0, y * (y1 - y0) + y0, 0.0, 1.0});
        }
        for(int j = 0; j < tst.size() && j < res.size(); ++j)
        {
            const QVector4D r = m * tst[j];
            assert((res[j] - r).length() < 1e-4);
        }
    }
}

void testScaleSizeShiftTopLeftSkew()
{
    for(int i = 0; i < 100; i++)
    {
        QVector<QVector2D> src = {
            {0.0, 0.0},
            {1.0, 1.0},
            {0.0, 1.0},
            {1.0, 0.0}
        };
        const float x0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float y2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        QVector<QVector2D> dst = {
            {x0, y0},
            {x1 + x2, y1 + y2},
            {x0 + x2, y1 + y2},
            {x1, y0}
        };
        QMatrix4x4 m;
        if(!calculateTransfromMatrixBy4Points(src, dst, m))
        {
            continue;
        }
        const QVector<QVector4D> tst = {
            {src[0].x(), src[0].y(), 0.0, 1.0},
            {src[1].x(), src[1].y(), 0.0, 1.0},
            {src[2].x(), src[2].y(), 0.0, 1.0},
            {src[3].x(), src[3].y(), 0.0, 1.0},
        };
        const QVector<QVector4D> res = {
            {dst[0].x(), dst[0].y(), 0.0, 1.0},
            {dst[1].x(), dst[1].y(), 0.0, 1.0},
            {dst[2].x(), dst[2].y(), 0.0, 1.0},
            {dst[3].x(), dst[3].y(), 0.0, 1.0},
        };
        for(int j = 0; j < tst.size() && j < res.size(); ++j)
        {
            const QVector4D r = m * tst[j];
            assert((res[j] - r).length() < 1e-4);
        }
    }
}

void testScaleSizeShiftTopLeftBothSkew()
{
    for(int i = 0; i < 100; i++)
    {
        QVector<QVector2D> src = {
            {0.0, 0.0},
            {1.0, 1.0},
            {0.0, 1.0},
            {1.0, 0.0}
        };
        const float x0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float y2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float x3 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float y3 = QRandomGenerator64::global()->generateDouble() - 0.5;
        QVector<QVector2D> dst = {
            {x0, y0},
            {x1 + x2 + x3, y1 + y2 + y3},
            {x0 + x2, y1 + y2},
            {x1 + x3, y0 + y3}
        };
        QMatrix4x4 m;
        if(!calculateTransfromMatrixBy4Points(src, dst, m))
        {
            continue;
        }
        const QVector<QVector4D> tst = {
            {src[0].x(), src[0].y(), 0.0, 1.0},
            {src[1].x(), src[1].y(), 0.0, 1.0},
            {src[2].x(), src[2].y(), 0.0, 1.0},
            {src[3].x(), src[3].y(), 0.0, 1.0},
        };
        const QVector<QVector4D> res = {
            {dst[0].x(), dst[0].y(), 0.0, 1.0},
            {dst[1].x(), dst[1].y(), 0.0, 1.0},
            {dst[2].x(), dst[2].y(), 0.0, 1.0},
            {dst[3].x(), dst[3].y(), 0.0, 1.0},
        };
        for(int j = 0; j < tst.size() && j < res.size(); ++j)
        {
            const QVector4D r = m * tst[j];
            assert((res[j] - r).length() < 1e-4);
        }
    }
}

void testImpossibleTransform()
{
    bool result = true;
    for(int i = 0; i < 100; i++)
    {
        QVector<QVector2D> src = {
            {0.0, 0.0},
            {1.0, 1.0},
            {0.0, 1.0},
            {1.0, 0.0}
        };
        const float x0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y0 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float y1 = QRandomGenerator64::global()->generateDouble() * 1e2;
        const float x2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float y2 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float x3 = QRandomGenerator64::global()->generateDouble() - 0.5;
        const float y3 = QRandomGenerator64::global()->generateDouble() - 0.5;
        QVector<QVector2D> dst = {
            {x0, y0},
            {x1, y1},
            {x2, y2},
            {x3, y3}
        };
        QMatrix4x4 m;
        if(!calculateTransfromMatrixBy4Points(src, dst, m))
        {
            continue;
        }
        const QVector<QVector4D> tst = {
            {src[0].x(), src[0].y(), 0.0, 1.0},
            {src[1].x(), src[1].y(), 0.0, 1.0},
            {src[2].x(), src[2].y(), 0.0, 1.0},
            {src[3].x(), src[3].y(), 0.0, 1.0},
        };
        const QVector<QVector4D> res = {
            {dst[0].x(), dst[0].y(), 0.0, 1.0},
            {dst[1].x(), dst[1].y(), 0.0, 1.0},
            {dst[2].x(), dst[2].y(), 0.0, 1.0},
            {dst[3].x(), dst[3].y(), 0.0, 1.0},
        };
        for(int j = 0; j < tst.size() && j < res.size(); ++j)
        {
            const QVector4D r = m * tst[j];
            result &= (res[j] - r).length() < 1e-4;
        }
    }
    assert(!result);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    testScaleSizeShiftTopLeft();
    testScaleSizeShiftTopLeftTestInsideCoordinates();
    testScaleSizeShiftTopLeftSkew();
    testScaleSizeShiftTopLeftBothSkew();
    testImpossibleTransform();

    return 0;
}
