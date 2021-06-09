#include "object_tests.h"
#include <QRandomGenerator>
#include "drawingdata_effectobjects.h"
#include "../MastactvaModels/objectinfo_data.h"
#include "drawing_tests.h"


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

std::shared_ptr<MapFileSource> createMapFileSource()
{
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();
    filesource->add(g_baseVertexShaderFilename, g_baseVertexShader);
    filesource->add(g_baseFragmentShaderFilename, g_baseFragmatShader);
    return filesource;
}

QString toString(const std::vector<QVector3D> &data_)
{
    QString result;
    for(const QVector3D &vec_ : data_)
    {
        result += QString("%1 %2 %3 ").arg(vec_.x(), vec_.y(), vec_.z());
    }
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

std::unique_ptr<EffectObjectsData> createTestData()
{
    static const int effectObjectId = 1;
    static const int effectId = 1;
    static const int objectInfoId = 1;
    static const int effectObjectStep0 = 0;
    static const char *effectName = "effect #1";
    static const char *effectProgrammerName = "effect1";
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const char *emptyStr = "";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    const QDateTime now = QDateTime::currentDateTime();
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

    std::unique_ptr<EffectObjectsData> result = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep0
                );
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectName,
                effectProgrammerName,
                now
                );
    result->m_objectInfoData->push_back(objectInfoData.release());
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

    QRandomGenerator gen;
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
                emptyStr,
                toString(normalData),
                now
                );
    artefact1->m_artefactArgData->push_back(normalArg.release());
    auto matrixArg = std::make_unique<ArtefactArgData>(
                artefactArgId3,
                artefactId1,
                artefactArgType3,
                artefactArgStorage3,
                artefactArgName3,
                emptyStr,
                toString(modelview),
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
    result->m_objectArtefactData->push_back(objectArtefactData1.release());
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
    result->m_objectArtefactData->push_back(objectArtefactData2.release());
    return result;
}

namespace drawing_objects
{

void BaseTest::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData();
    ::DrawingDataEffectObjects drawingDataEffectObjects(std::move(*effectObjectsData));
    drawingDataEffectObjects.init(filesource);
    drawingDataEffectObjects.initialize(data_);
}

}
