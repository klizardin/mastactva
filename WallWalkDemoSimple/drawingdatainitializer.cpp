#include "drawingdatainitializer.h"
#include <QRandomGenerator>
#include <QDir>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaModels/effect_data.h"
#include "../MastactvaBase/names.h"

/*
 * simple IFileSource implementation
 * (for test purpose)
*/
class MapFileSource : public drawingdata::IFileSource
{
public:
    MapFileSource() = default;
    ~MapFileSource() override = default;

    void add(const QString &filename_, const QString &text_);
    void add(const QString &filename_, const QImage &image_);

    QString getText(const FileSource &filename_) const override;
    QImage getImage(const FileSource &filename_) const override;

private:
    QHash<QString, QString> m_texts;
    QHash<QString, QImage> m_images;
};


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

static const char *g_walkEffectOnePassVertexShaderFilename = "walkeffectonepass.vsh";
static const char *g_walkEffectOnePassFragmentShaderFilename = "walkeffectonepass.fsh";


std::shared_ptr<MapFileSource> createMapFileSource()
{
    QRandomGenerator gen;
    gen.seed(time(nullptr));
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();

    filesource->add(g_walkEffectOnePassVertexShaderFilename,
                    loadTextFile(":/Shaders/Shaders/walkeffecttest/walkeffectonepass.vsh")
                    );
    filesource->add(g_walkEffectOnePassFragmentShaderFilename,
                    loadTextFile(":/Shaders/Shaders/walkeffecttest/walkeffectonepass.fsh")
                    );
    return filesource;
}

std::unique_ptr<EffectData> createWalkEffectOnePassTestData()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    static const char *effectProgrammerName = "effect1";
    static const char *effectNameMain = "global data";
    static QString effectProgrammerNameMain = QString(g_defaultObjectInfoProgrammerName) + "_globalData";
    const QDateTime now = QDateTime::currentDateTime();
    static const int effectObjectStep0 = 0;

    // TODO: use data from xml

    //QDir addonsDir;
    //findDynamicLibrariesDir(QDir("./"), addonsDir);
    //auto modules = std::make_shared<AddonModules>();
    //modules->create(addonsDir);

    //QString inputJson = QString(g_inputJson).arg(
    //            absoluteHomePath("~/Pictures/test_images/20220116_145321.jpg"),
    //            absoluteHomePath("~/Pictures/test_images/20220116_145325.jpg"),
    //            absoluteHomePath("~/tmp/")
    //            );

    //QString inputJson = QString(g_inputJson).arg(
    //            absoluteHomePath("~/Pictures/test_images/from_image.jpg"),
    //            absoluteHomePath("~/Pictures/test_images/to_image.jpg"),
    //            absoluteHomePath("~/tmp/")
    //            );
    //20220116_145321.jpg",
    //20220116_145325.jpg",
    //QJsonDocument result = modules->call("WalkEffect", QJsonDocument::fromJson(inputJson.toUtf8()));

    std::vector<QVector4D> fromValues, toValues;
    //convertJsonResultToCoordinates(result.object().value("1").toObject(), fromValues);
    //convertJsonResultToCoordinates(result.object().value("0").toObject(), toValues);

    //qDebug() << "fromValues" << fromValues;
    //qDebug() << "toValues" << toValues;

    std::vector<GLfloat> fromCoords, toCoords;
    createGeometry(10, 10, fromValues, fromCoords);
    createGeometry(10, 10, toValues, toCoords);

    //qDebug() << "fromCoords" << fromCoords;
    //qDebug() << "toCoords" << toCoords;

    auto effectObject0 = createWalkEffectOnePassTestObject(
                effectId,
                effectName,
                effectProgrammerName,
                now,
                effectObjectStep0,
                g_walkEffectOnePassVertexShaderFilename,
                g_walkEffectOnePassFragmentShaderFilename,
                absoluteHomePath("~/Pictures/test_images/20220116_145321.jpg"),
                absoluteHomePath("~/Pictures/test_images/20220116_145325.jpg"),
                //absoluteHomePath("~/Pictures/test_images/from_image.jpg"),
                //absoluteHomePath("~/Pictures/test_images/to_image.jpg"),
                fromCoords,
                toCoords,
                QPoint(10, 10)
                );
    auto effectObject1 = createGlobalDataTestObject(
                effectId,
                effectNameMain,
                effectProgrammerNameMain,
                now,
                effectObjectStep0,
                QVector3D(0.0, 0.0, 1.0),
                QString(g_renderWalkEffectRectMatrixCalculation)
                    + QString("(")
                        + QString(g_renderFromImageName) + QString(g_argumentsSplitter)
                        + QString("vaFromMatrix")
                    + QString(")")
                + g_renderObjectsStatesSpliter
                + QString(g_renderWalkEffectRectMatrixCalculation)
                    + QString("(")
                        + QString(g_renderToImageName) + QString(g_argumentsSplitter)
                        + QString("vaToMatrix")
                    + QString(")")
                + g_renderObjectsStatesSpliter
                + QString(g_renderImageGeometryMatrixMultipleCalculation)
                    + QString("(")
                        + QString(g_renderToImageName) + QString(g_argumentsSplitter)
                        + QString(g_renderFromImageName)
                    + QString(")")
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now,
                MergeId()
                );
    effect->m_effectObjectsData->push_back(effectObject0.release());
    effect->m_effectObjectsData->push_back(effectObject1.release());
    return effect;
}

namespace drawing_objects
{

void WalkEffectOnePass::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);
    Q_UNUSED(data_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createWalkEffectOnePassTestData();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

std::pair<const char *, const char *> WalkEffectOnePass::getDescription() const
{
    return {"WalkEffectOnPathTest", "walk effect on path version"};
}

} //namespace drawing_objects
