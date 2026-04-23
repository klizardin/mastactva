#ifndef DRAWINGDATAINITIALIZER_UTILS_H
#define DRAWINGDATAINITIALIZER_UTILS_H


#include <QDir>
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/format.h"
#include <GL/gl.h>

/*
 * simple IFileSource implementation
 * (for test purpose)
*/
class MapFileSource : public drawingdata::IFileSource
{
public:
    MapFileSource() = default;
    ~MapFileSource() override = default;

    void loadImage(const QString &filename_, const QDir& sourceImageDir_);
    void add(const QString &filename_, const QString &text_);
    void add(const QString &filename_, const QImage &image_);

    QString getText(const FileSource &filename_) const override;
    QImage getImage(const FileSource &filename_) const override;

private:
    QHash<QString, QString> m_texts;
    QHash<QString, QImage> m_images;
};


void createGeometry(
        int geomertyPointsWidth_, int geometryPointsHeight_,
        const std::vector<QVector4D> &coords_,
        std::vector<GLfloat> &vertexData_
        );
std::shared_ptr<MapFileSource> createMapFileSource();

inline
QString toString(const std::vector<QVector2D> &data_)
{
    QString result;
    for(const QVector2D &vec_ : qAsConst(data_))
    {
        result += QString("%1 %2 ").arg(
                    fmt::toString(vec_.x()),
                    fmt::toString(vec_.y())
                    );
    }
    return result;
}

inline
QString toString(const std::vector<QVector3D> &data_)
{
    QString result;
    for(const QVector3D &vec_ : qAsConst(data_))
    {
        result += QString("%1 %2 %3 ").arg(
                    fmt::toString(vec_.x()),
                    fmt::toString(vec_.y()),
                    fmt::toString(vec_.z())
                    );
    }
    return result;
}

inline
QString toString(const std::vector<QVector4D> &data_)
{
    QString result;
    for(const QVector4D &vec_ : qAsConst(data_))
    {
        result += QString("%1 %2 %3 %4 ").arg(
                    fmt::toString(vec_.x()),
                    fmt::toString(vec_.y()),
                    fmt::toString(vec_.z()),
                    fmt::toString(vec_.w())
                    );
    }
    return result;
}

inline
QString toString(const std::vector<float> &data_)
{
    QString result;
    for(const float &val_ : qAsConst(data_))
    {
        result += QString("%1 ").arg(val_);
    }
    return result;
}

inline
QString toString(const int &data_)
{
    QString result = QString("%1")
            .arg(
                fmt::toString(data_)
                );
    return result;
}

inline
QString toString(const float &data_)
{
    QString result = QString("%1")
            .arg(
                fmt::toString(data_)
                );
    return result;
}

inline
QString toString(const QVector2D &vec_)
{
    QString result = QString("%1 %2")
            .arg(
                fmt::toString(vec_.x()),
                fmt::toString(vec_.y())
                );
    return result;
}

inline
QString toString(const QVector3D &vec_)
{
    QString result = QString("%1 %2 %3")
            .arg(
                fmt::toString(vec_.x()),
                fmt::toString(vec_.y()),
                fmt::toString(vec_.z())
                );
    return result;
}

inline
QString toString(const QVector4D &vec_)
{
    QString result = QString("%1 %2 %3 %4")
            .arg(
                fmt::toString(vec_.x()),
                fmt::toString(vec_.y()),
                fmt::toString(vec_.z()),
                fmt::toString(vec_.w())
                );
    return result;
}

inline
QString toString(const QMatrix4x4 &mat4_)
{
    QString result;
    for(int i = 0; i < 4*4; ++i)
    {
        result += QString("%1 ").arg(mat4_.constData()[i]);
    }
    return result;
}

struct names
{
    static const char *walkEffectOnePassVertexShaderFilename;
    static const char *walkEffectOnePassFragmentShaderFilename;
    static const char *emptyFilename;
    static const char *baseVertexShaderFilename;
    static const char *baseFragmentShaderFilename;
};

#endif // DRAWINGDATAINITIALIZER_UTILS_H
