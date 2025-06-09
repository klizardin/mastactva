#include "drawingdatainitializer_utils.h"
#include <QRandomGenerator>
#include "../MastactvaBase/utils.h"


static const int g_trianglesCount = 2;
static const int g_triangleConers = 3;

void createGeometry(
        int geomertyPointsWidth_, int geometryPointsHeight_,
        const std::vector<QVector4D> &coords_,
        std::vector<GLfloat> &vertexData_
        )
{
    static const int coords[g_trianglesCount][g_triangleConers][2] =
    {
        {{ 1, 0 }, { 0, 0 }, { 0, 1 }},
        {{ 1, 0 }, { 0, 1 }, { 1, 1 }}
    };

    vertexData_.resize(geomertyPointsWidth_ * geometryPointsHeight_ *
                      g_trianglesCount * g_triangleConers * 4);
    for(int y = 0; y < geometryPointsHeight_; y++)
    {
        for(int x = 0; x < geomertyPointsWidth_; x++)
        {
            const int offsBase0 = (y * geomertyPointsWidth_ + x) *
                    g_trianglesCount * g_triangleConers * 4;
            for (int j = 0; j < g_trianglesCount; ++j)
            {
                for(int k = 0; k < g_triangleConers; k++)
                {
                    const int ptX = x + coords[j][k][0];
                    const int ptY = y + coords[j][k][1];
                    const int ptOffs = ptX + ptY * (geomertyPointsWidth_ + 1);

                    // vertex position
                    const int offs0 = offsBase0 + (j * g_triangleConers + k) * 4;
                    vertexData_[offs0 + 0] = coords_[ptOffs].x();
                    vertexData_[offs0 + 1] = 1.0 - coords_[ptOffs].y();
                    vertexData_[offs0 + 2] = coords_[ptOffs].z();
                    vertexData_[offs0 + 3] = coords_[ptOffs].w();
                }
            }
        }
    }
}


void MapFileSource::add(const QString &filename_, const QString &text_)
{
    m_texts.insert(filename_, text_);
}

void MapFileSource::add(const QString &filename_, const QImage &image_)
{
    m_images.insert(filename_, image_);
}

QString MapFileSource::getText(const FileSource &filename_) const
{
    return m_texts.value(filename_, QString());
}

QImage MapFileSource::getImage(const FileSource &filename_) const
{
    return m_images.value(filename_, QImage());
}


std::shared_ptr<MapFileSource> createMapFileSource()
{
    QRandomGenerator gen;
    gen.seed(time(nullptr));
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();

    filesource->add(names::walkEffectOnePassVertexShaderFilename,
                    loadTextFile(":/Shaders/Shaders/walkeffecttest/walkeffectonepass.vsh")
                    );
    filesource->add(names::walkEffectOnePassFragmentShaderFilename,
                    loadTextFile(":/Shaders/Shaders/walkeffecttest/walkeffectonepass.fsh")
                    );
    filesource->add(names::emptyFilename, QString{});
    return filesource;
}

const char *names::walkEffectOnePassVertexShaderFilename = "walkeffectonepass.vsh";
const char *names::walkEffectOnePassFragmentShaderFilename = "walkeffectonepass.fsh";
const char *names::emptyFilename = "empty.lua";
