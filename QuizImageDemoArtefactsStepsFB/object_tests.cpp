#include "object_tests.h"
#include "drawing_tests.h"
#include <QRandomGenerator>
#include "drawingdata_effect.h"
#include "../MastactvaModels/objectinfo_data.h"
#include "../MastactvaBase/format.h"
#include "../MastactvaBase/utils.h"


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


static const char *g_baseVertexShader =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec3 normal;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
    "    float angle = max(dot(normal, toLight), 0.0);\n"
    "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
    "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
    "    color = clamp(color, 0.0, 1.0);\n"
    "    gl_Position = matrix * vertex;\n"
    "}\n";

static const char *g_baseFragmatShader =
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = color;\n"
    "}\n";

static const char *g_baseVertexShaderFilename = "base.vsh";
static const char *g_baseFragmentShaderFilename = "base.fsh";
static const char *g_defaultVertexShaderFilename = "default.vsh";
static const char *g_defaultFragmentShaderFilename = "default.fsh";

std::shared_ptr<MapFileSource> createMapFileSource()
{
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();
    filesource->add(g_baseVertexShaderFilename, g_baseVertexShader);
    filesource->add(g_baseFragmentShaderFilename, g_baseFragmatShader);
    filesource->add(g_defaultVertexShaderFilename, loadTextFile(":/Shaders/Shaders/test000/default.vsh"));
    filesource->add(g_defaultFragmentShaderFilename, loadTextFile(":/Shaders/Shaders/test000/default.fsh"));
    return filesource;
}

QString toString(const std::vector<QVector2D> &data_)
{
    QString result;
    for(const QVector2D &vec_ : data_)
    {
        result += QString("%1 %2 ").arg(
                    fmt::toString(vec_.x()),
                    fmt::toString(vec_.y())
                    );
    }
    return result;
}

QString toString(const std::vector<QVector3D> &data_)
{
    QString result;
    for(const QVector3D &vec_ : data_)
    {
        result += QString("%1 %2 %3 ").arg(
                    fmt::toString(vec_.x()),
                    fmt::toString(vec_.y()),
                    fmt::toString(vec_.z())
                    );
    }
    return result;
}

QString toString(const std::vector<QVector4D> &data_)
{
    QString result;
    for(const QVector4D &vec_ : data_)
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

QString toString(const int &data_)
{
    QString result = QString("%1")
            .arg(
                fmt::toString(data_)
                );
    return result;
}

QString toString(const float &data_)
{
    QString result = QString("%1")
            .arg(
                fmt::toString(data_)
                );
    return result;
}

QString toString(const QVector2D &vec_)
{
    QString result = QString("%1 %2")
            .arg(
                fmt::toString(vec_.x()),
                fmt::toString(vec_.y())
                );
    return result;
}

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

QString toString(const QMatrix4x4 &mat4_)
{
    QString result;
    for(int i = 0; i < 4*4; ++i)
    {
        result += QString("%1 ").arg(mat4_.constData()[i]);
    }
    return result;
}

static const char *emptyStr = "";


std::unique_ptr<EffectObjectsData> createTestObject(
        int effectId,
        const char *effectName,
        const QDateTime &now,
        int effectObjectStep,
        QRandomGenerator &gen
        )
{
    static const int effectObjectId = 1;
    static const int objectInfoId = 1;
    static const char *effectProgrammerName = "effect1";
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    static const int objectArtefactId1 = 1;
    static const int objectArtefactStep0 = 0;
    static const int artefactId2 = 2;
    static const char *artefactName2 = "fragment shader";
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::shaderFragmet;
    static const int objectArtefactId2 = 2;
    static const int artefactArgId1 = 1;
    static const ArtefactArgTypeEn artefactArgType1 = ArtefactArgTypeEn::vec3Type;
    static const ArtefactArgStorageEn artefactArgStorage1 = ArtefactArgStorageEn::attributeStorage;
    static const char *artefactArgName1 = "vertex";
    static const int artefactArgId2 = 2;
    static const ArtefactArgTypeEn artefactArgType2 = ArtefactArgTypeEn::vec3Type;
    static const ArtefactArgStorageEn artefactArgStorage2 = ArtefactArgStorageEn::attributeStorage;
    static const char *artefactArgName2 = "normal";
    static const int artefactArgId3 = 3;
    static const ArtefactArgTypeEn artefactArgType3 = ArtefactArgTypeEn::mat4Type;
    static const ArtefactArgStorageEn artefactArgStorage3 = ArtefactArgStorageEn::uniformStorage;
    static const char *artefactArgName3 = "matrix";

    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectName,
                effectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                g_baseVertexShaderFilename,
                emptyStr,
                artefactType1,
                emptyStr,
                now
                );

    std::vector<QVector3D> vertexData;
    std::vector<QVector3D> normalData;
    test::createGeometry(vertexData, normalData);

    qreal fScale = 1;
    qreal fAngle = gen.generateDouble() * 360.0;
    QMatrix4x4 modelview;
    modelview.rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);

    auto vertexArg = std::make_unique<ArtefactArgData>(
                artefactArgId1,
                artefactId1,
                artefactArgType1,
                artefactArgStorage1,
                artefactArgName1,
                toString(vertexData),
                emptyStr,
                now
                );
    artefact1->m_artefactArgData->push_back(vertexArg.release());
    auto normalArg = std::make_unique<ArtefactArgData>(
                artefactArgId2,
                artefactId1,
                artefactArgType2,
                artefactArgStorage2,
                artefactArgName2,
                toString(normalData),
                emptyStr,
                now
                );
    artefact1->m_artefactArgData->push_back(normalArg.release());
    auto matrixArg = std::make_unique<ArtefactArgData>(
                artefactArgId3,
                artefactId1,
                artefactArgType3,
                artefactArgStorage3,
                artefactArgName3,
                toString(modelview),
                emptyStr,
                now
                );
    artefact1->m_artefactArgData->push_back(matrixArg.release());
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData1.release());
    auto artefact2 = std::make_unique<ArtefactData>(
                artefactId2,
                artefactName2,
                g_baseFragmentShaderFilename,
                emptyStr,
                artefactType2,
                emptyStr,
                now
                );
    auto objectArtefactData2 = std::make_unique<ObjectArtefactData>(
                objectArtefactId2,
                effectId,
                artefactId2,
                objectArtefactStep0,
                artefact2.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData2.release());
    return effectObject;
}

std::unique_ptr<EffectObjectsData> createTestObject2(
        int effectId,
        const char *effectName,
        const QDateTime &now,
        int effectObjectStep
        )
{
    static const int effectObjectId = 1;
    static const int objectInfoId = 1;
    static const char *effectProgrammerName = "effect1";
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    static const int objectArtefactId1 = 1;
    static const int objectArtefactStep0 = 0;
    static const int artefactId2 = 2;
    static const char *artefactName2 = "fragment shader";
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::shaderFragmet;
    static const int objectArtefactId2 = 2;
    enum class ArgEn{id, type, storage, name, value};
    static const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, const char *, const char *> args1[] = {
        {
            1,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            "renderVertexAttribute",
            emptyStr
        },
        {
            2,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            "renderTextureAttribute",
            emptyStr
        },
        {
            3,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "renderMatrix",
            emptyStr
        },
        {
            4,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "renderFromImageMatrix",
            emptyStr
        },
        {
            5,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "renderToImageMatrix",
            emptyStr
        },
        {
            6,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            "renderFacedGeometryCoefs",
            "0.0 0.0"
        },
        {
            7,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            "renderGeomertySize",
            "2.0 2.0"
        },
        {
            8,
            ArtefactArgTypeEn::intType,
            ArtefactArgStorageEn::uniformStorage,
            "renderIsGeomertySolid",
            "1"
        }
    };
    static const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, const char *, const char *> args2[] = {
        {
            101,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "renderOpacity",
            "1.0"
        },
        {
            102,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "renderT",
            "0.5"
        },
    };

    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectName,
                effectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                g_defaultVertexShaderFilename,
                emptyStr,
                artefactType1,
                emptyStr,
                now
                );

    for(std::size_t i = 0; i < sizeof(args1)/sizeof(args1[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(args1[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(args1[i]),
                std::get<to_underlying(ArgEn::storage)>(args1[i]),
                std::get<to_underlying(ArgEn::name)>(args1[i]),
                std::get<to_underlying(ArgEn::value)>(args1[i]),
                emptyStr,
                now
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }

    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData1.release());
    auto artefact2 = std::make_unique<ArtefactData>(
                artefactId2,
                artefactName2,
                g_defaultFragmentShaderFilename,
                emptyStr,
                artefactType2,
                emptyStr,
                now
                );
    for(std::size_t i = 0; i < sizeof(args2)/sizeof(args2[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(args2[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(args2[i]),
                std::get<to_underlying(ArgEn::storage)>(args2[i]),
                std::get<to_underlying(ArgEn::name)>(args2[i]),
                std::get<to_underlying(ArgEn::value)>(args1[i]),
                emptyStr,
                now
                );
        artefact2->m_artefactArgData->push_back(arg.release());
    }
    auto objectArtefactData2 = std::make_unique<ObjectArtefactData>(
                objectArtefactId2,
                effectId,
                artefactId2,
                objectArtefactStep0,
                artefact2.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData2.release());

    static const int artefactId3 = 3;
    static const char *artefactName3 = "renderFromImage";
    static const char *artefactFile3 = ":/Images/Images/no-image-001.png";
    static const ArtefactTypeEn artefactType3 = ArtefactTypeEn::texture2D;
    static const int objectArtefactId3 = 3;

    auto artefact3 = std::make_unique<ArtefactData>(
                artefactId3,
                artefactName3,
                artefactFile3,
                emptyStr,
                artefactType3,
                emptyStr,
                now
                );
    auto objectArtefactData3 = std::make_unique<ObjectArtefactData>(
                objectArtefactId3,
                effectId,
                artefactId3,
                objectArtefactStep0,
                artefact3.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData3.release());

    static const int artefactId4 = 4;
    static const char *artefactName4 = "renderToImage";
    static const char *artefactFile4 = ":/Images/Images/no-image-002.png";
    static const ArtefactTypeEn artefactType4 = ArtefactTypeEn::texture2D;
    static const int objectArtefactId4 = 4;

    auto artefact4 = std::make_unique<ArtefactData>(
                artefactId4,
                artefactName4,
                artefactFile4,
                emptyStr,
                artefactType4,
                emptyStr,
                now
                );
    auto objectArtefactData4 = std::make_unique<ObjectArtefactData>(
                objectArtefactId4,
                effectId,
                artefactId4,
                objectArtefactStep0,
                artefact4.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData4.release());

    return effectObject;
}

std::unique_ptr<EffectData> createTestData1()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;

    auto effectObject1 = createTestObject(
                effectId,
                effectName,
                now,
                effectObjectStep0,
                gen
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData2()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;

    auto effectObject1 = createTestObject(
                effectId,
                effectName,
                now,
                effectObjectStep0,
                gen
                );
    auto effectObject2 = createTestObject(
                effectId,
                effectName,
                now,
                effectObjectStep1,
                gen
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData3()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    static const int effectObjectStep0 = 0;

    auto effectObject1 = createTestObject2(
                effectId,
                effectName,
                now,
                effectObjectStep0
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    return effect;
}

namespace drawing_objects
{

void BaseTest::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData1();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void MultipleObjectsTest::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData2();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DefaultTest::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData3();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

}
