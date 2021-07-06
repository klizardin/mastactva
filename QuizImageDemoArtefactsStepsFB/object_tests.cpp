#include "object_tests.h"
#include "drawing_tests.h"
#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include "drawingdata_effect.h"
#include "../MastactvaModels/objectinfo_data.h"
#include "../MastactvaBase/format.h"
#include "../MastactvaBase/wavefrontobj.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"


QString createQTGeomJson(QRandomGenerator &gen_, const char *objectName_ = nullptr, const int *step_ = nullptr);
QString createObjectsQTGeomJson(int cnt_, const char *objectName_);
QString createAliseQTGeomJson(
        const char *geom3dObjectName_,
        const char *objectName_ = nullptr,
        const int *step_ = nullptr
        );
QString createQTGeomLuaNewVariables(
        const char *geom3dObjectName_
        );
QString createQTGeomLuaSetVariables(
        const char *geom3dObjectName_
        );
QString createQTGeomLua1SetVariables(
        const char *dest_,
        const char *src_
        );

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


static const char *g_baseVertexShader0 =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec3 normal;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
    "    mediump float angle = max(dot(normal, toLight), 0.0);\n"
    "    mediump vec3 col = vec3(0.40, 1.0, 0.0);\n"
    "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
    "    color = clamp(color, 0.0, 1.0);\n"
    "    gl_Position = matrix * vertex;\n"
    "}\n";

static const char *g_baseVertexShader3DObject =
    "attribute highp vec4 %1;\n"
    "attribute mediump vec4 %2;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    mediump vec4 toLight = normalize(vec4(0.0, 0.3, 1.0, 0.0));\n"
    "    mediump float angle = max(dot(%2, toLight), 0.0);\n"
    "    mediump vec3 col = vec3(0.40, 1.0, 0.0);\n"
    "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
    "    color = clamp(color, 0.0, 1.0);\n"
    "    gl_Position = matrix * %1;\n"
    "}\n";

static const char *g_baseFragmatShader =
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = color;\n"
    "}\n";

static const char *g_luaScript2NewVariable =
    "function main ()\n"
    "    result = {}\n"
    "    result[\"%1\"] = getVariable(\"%2\")\n"
    "    result[\"%3\"] = getVariable(\"%4\")\n"
    "    return result;\n"
    "end\n";

static const char *g_luaScript2SetVariable =
    "function main ()\n"
    "    result = {}\n"
    "    setVariable(\"%1\", getVariable(\"%2\"))\n"
    "    setVariable(\"%3\", getVariable(\"%4\"))\n"
    "    return result;\n"
    "end\n";

static const char *g_luaScript1SetVariable =
    "function main ()\n"
    "    result = {}\n"
    "    setVariable(\"%1\", getVariable(\"%2\"))\n"
    "    return result;\n"
    "end\n";

static const char *g_luaScriptCalcMatrix =
    "function main ()\n"
    "   local pi = {4, 4}\n"
    "   local m = matrix.identity(pi)\n"
    "   local a = getVariable(\"angle\")\n"
    "   local pry = {a[1] * 90.0, 0.0, 1.0, 0.0}\n"
    "   m = matrix.rotate(m, pry)\n"
    "   local prx = {a[1] * 90.0, 1.0, 0.0, 0.0}\n"
    "   m = matrix.rotate(m, prx)\n"
    "   local prz = {a[1] * 90.0, 0.0, 0.0, 1.0}\n"
    "   m = matrix.rotate(m, prz)\n"
    "   local ps = {0.08, 0.08, 0.08}\n"
    "   m = matrix.scale(m , ps)\n"
    "   local pt = {0.0, -0.2, 0.0}\n"
    "   m = matrix.translate(m , pt)\n"
    "   setVariable(\"matrix\", m)\n"
    "   result = {}\n"
    "   return result\n"
    "end\n"
    ;

static const char *g_effectObjectQtLogoProgrammerName = "gt_logo";
static const char *g_baseVertexShaderFilename = "base.vsh";
static const char *g_baseFragmentShaderFilename = "base.fsh";
static const char *g_defaultVertexShaderFilename = "default.vsh";
static const char *g_defaultFragmentShaderFilename = "default.fsh";
static const char *g_dataJsonQTGeometryFilename = "qt_geom.json";
static const char *g_dataJsonQTGeometry0Filename = "qt_geom_0.json";
static const char *g_dataJsonQTGeometry1Filename = "qt_geom_1.json";
static const char *g_dataJsonQTGeometry2Filename = "qt_geom_2.json";
static const char *g_dataJsonObjectsOfQtGeomFilename = "qt_geom_1.json";
static const char *g_dataJson3DObjectSwiftFilename = "swift.obj";
static const char *g_dataJson3DObjectCubeFilename = "cube.obj";
static const char *g_3dObjectSwiftFragmentShaderFilename = "swift.vsh";
static const char *g_3dObjectCubeFragmentShaderFilename = "cube.vsh";
static const char *g_3dObjectDefaultFragmentShaderFilename = "cube.vsh";
static const char *g_dataJsonAliasSwiftFilename = "alias_swift.json";
static const char *g_dataLua2NewVariableFilename = "new_variables.lua";
static const char *g_dataLua2SetVariableFilename = "set_variables.lua";
static const char *g_dataLua1SetVariableAngleAsTFilename = "set_angle_as_t.lua";
static const char *g_dataLuaCalcMatrixFilename = "calc_matrix_from_angle.lua";


std::shared_ptr<MapFileSource> createMapFileSource()
{
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();
    filesource->add(g_baseVertexShaderFilename, g_baseVertexShader0);
    filesource->add(g_baseFragmentShaderFilename, g_baseFragmatShader);
    filesource->add(g_defaultVertexShaderFilename, loadTextFile(":/Shaders/Shaders/test000/default.vsh"));
    filesource->add(g_defaultFragmentShaderFilename, loadTextFile(":/Shaders/Shaders/test000/default.fsh"));
    QRandomGenerator gen;
    gen.seed(time(nullptr));
    filesource->add(g_dataJsonQTGeometryFilename, createQTGeomJson(gen));
    const int pos0 = 1;
    filesource->add(g_dataJsonQTGeometry0Filename, createQTGeomJson(gen, nullptr, &pos0));
    const int pos1 = 2;
    filesource->add(g_dataJsonQTGeometry1Filename, createQTGeomJson(gen, nullptr, &pos1));
    const int pos2 = 3;
    filesource->add(g_dataJsonQTGeometry2Filename, createQTGeomJson(gen, nullptr, &pos2));
    filesource->add(g_dataJsonObjectsOfQtGeomFilename, createObjectsQTGeomJson(
                        3,
                        g_effectObjectQtLogoProgrammerName
                        )
                    );
    filesource->add(g_dataJson3DObjectSwiftFilename, loadTextFile(":/obj3d/swift.obj"));
    filesource->add(g_dataJson3DObjectCubeFilename, loadTextFile(":/obj3d/cube.obj"));
    filesource->add(g_3dObjectSwiftFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject).arg("swift_vertex", "swift_normal")
                    );
    filesource->add(g_3dObjectCubeFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject).arg("cube_vertex", "cube_normal")
                    );
    filesource->add(g_3dObjectDefaultFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject).arg("vertex", "normal")
                    );
    filesource->add(g_dataJsonAliasSwiftFilename, createAliseQTGeomJson("swift_", nullptr, &pos2));
    filesource->add(g_dataLua2NewVariableFilename, createQTGeomLuaNewVariables("swift_"));
    filesource->add(g_dataLua2SetVariableFilename, createQTGeomLuaSetVariables("swift_"));
    filesource->add(g_dataLua1SetVariableAngleAsTFilename, createQTGeomLua1SetVariables("angle", g_renderTName));
    filesource->add(g_dataLuaCalcMatrixFilename, g_luaScriptCalcMatrix);
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

enum class ArgEn{id, type, storage, name, value};

// TODO: refactoring
std::unique_ptr<EffectObjectsData> createTestObject(
        int effectId,
        const QDateTime &now,
        int effectObjectStep,
        QRandomGenerator &gen,
        const int objectInfoId,
        const char *effectObjectName,
        const char *effectObjectProgrammerName
        )
{
    static const int effectObjectId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectObjectName,
                effectObjectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    // prepare data
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

    // for both artefacts
    static const int objectArtefactStep0 = 0;

    // vertex shader artefact
    const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, const char *, QString> vertexArgs[] = {
        {
            1,
            ArtefactArgTypeEn::vec3Type,
            ArtefactArgStorageEn::attributeStorage,
            "vertex",
            toString(vertexData)
        },
        {
            2,
            ArtefactArgTypeEn::vec3Type,
            ArtefactArgStorageEn::attributeStorage,
            "normal",
            toString(normalData)
        },
        {
            3,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "matrix",
            toString(modelview)
        }
    };
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                g_baseVertexShaderFilename,
                emptyStr,
                artefactType1,
                emptyStr,
                now
                );
    for(std::size_t i = 0; i < sizeof(vertexArgs)/sizeof(vertexArgs[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(vertexArgs[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::storage)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::name)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::value)>(vertexArgs[i]),
                emptyStr,
                now
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData1.release());

    // fragment shader artefact
    static const int artefactId2 = 2;
    static const char *artefactName2 = "fragment shader";
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::shaderFragmet;
    auto artefact2 = std::make_unique<ArtefactData>(
                artefactId2,
                artefactName2,
                g_baseFragmentShaderFilename,
                emptyStr,
                artefactType2,
                emptyStr,
                now
                );
    static const int objectArtefactId2 = 2;
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

std::unique_ptr<EffectObjectsData> createTestObject3DObject(
        int effectId,
        const QDateTime &now,
        int effectObjectStep,
        QRandomGenerator &gen,
        const int objectInfoId,
        const char *effectObjectName,
        const char *effectObjectProgrammerName,
        const char *object3DName_,
        const char *shaderFilename_
        )
{
    static const int effectObjectId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectObjectName,
                effectObjectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    // prepare data
    qreal fScale = 0.08;
    qreal fAngle = gen.generateDouble() * 360.0;
    QMatrix4x4 modelview;
    modelview.rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(fScale);
    modelview.translate(0.0f, -0.02f, 0.0f);

    // for both artefacts
    static const int objectArtefactStep0 = 0;

    // vertex shader artefact
    const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, QString, QString> vertexArgs[] = {
        {
            1,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            QString("%1vertex").arg(object3DName_),
            emptyStr
        },
        {
            2,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            QString("%1normal").arg(object3DName_),
            emptyStr
        },
        {
            3,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "matrix",
            toString(modelview)
        },
        {
            4,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "angle",
            "0.0"
        },
        {
            5,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderTName,
            "0.0"
        }
    };
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                shaderFilename_,
                emptyStr,
                artefactType1,
                emptyStr,
                now
                );
    for(std::size_t i = 0; i < sizeof(vertexArgs)/sizeof(vertexArgs[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(vertexArgs[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::storage)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::name)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::value)>(vertexArgs[i]),
                emptyStr,
                now
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData1.release());

    // fragment shader artefact
    static const int artefactId2 = 2;
    static const char *artefactName2 = "fragment shader";
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::shaderFragmet;
    auto artefact2 = std::make_unique<ArtefactData>(
                artefactId2,
                artefactName2,
                g_baseFragmentShaderFilename,
                emptyStr,
                artefactType2,
                emptyStr,
                now
                );
    static const int objectArtefactId2 = 2;
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

// TODO: refactoring
std::unique_ptr<EffectObjectsData> createTestObject2(
        int effectId,
        const char *effectName,
        const char *effectProgrammerName,
        const QDateTime &now,
        int effectObjectStep
        )
{
    static const int effectObjectId = 1;
    static const int objectInfoId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );

    // ObjectInfoData
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectName,
                effectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    // vertex shader
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    static const int objectArtefactId1 = 1;
    static const int objectArtefactStep0 = 0;
    enum class ArgEn{id, type, storage, name, value};
    static const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, const char *, const char *> vertexArgs[] = {
        {
            1,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            g_renderVertexAttributeName,
            emptyStr
        },
        {
            2,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            g_renderTextureAttributeName,
            emptyStr
        },
        {
            3,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderMatrixName,
            emptyStr
        },
        {
            4,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderFromImageMatrixName,
            emptyStr
        },
        {
            5,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderToImageMatrixName,
            emptyStr
        },
        {
            6,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderFacedGeometryCoefsName,
            "0.0 0.0"
        },
        {
            7,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderGeomertySizeName,
            "2.0 2.0"
        },
        {
            8,
            ArtefactArgTypeEn::intType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderIsGeomertySolidName,
            "1"
        },
        {
            9,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderScreenRectName,
            "1.0 1.0"
        },
        {
            10,
            ArtefactArgTypeEn::vec2Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderWindowSizeName,
            "1.0 1.0"
        }
    };
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                g_defaultVertexShaderFilename,
                emptyStr,
                artefactType1,
                emptyStr,
                now
                );
    for(std::size_t i = 0; i < sizeof(vertexArgs)/sizeof(vertexArgs[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(vertexArgs[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::storage)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::name)>(vertexArgs[i]),
                std::get<to_underlying(ArgEn::value)>(vertexArgs[i]),
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

    // fragment shader
    static const int artefactId2 = 2;
    static const char *artefactName2 = "fragment shader";
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::shaderFragmet;
    static const int objectArtefactId2 = 2;
    static const std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, const char *, const char *> fragmentArgs[] = {
        {
            101,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderOpacityName,
            "1.0"
        },
        {
            102,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderTName,
            "0.5"
        },
    };
    auto artefact2 = std::make_unique<ArtefactData>(
                artefactId2,
                artefactName2,
                g_defaultFragmentShaderFilename,
                emptyStr,
                artefactType2,
                emptyStr,
                now
                );
    for(std::size_t i = 0; i < sizeof(fragmentArgs)/sizeof(fragmentArgs[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(fragmentArgs[i]),
                artefactId1,
                std::get<to_underlying(ArgEn::type)>(fragmentArgs[i]),
                std::get<to_underlying(ArgEn::storage)>(fragmentArgs[i]),
                std::get<to_underlying(ArgEn::name)>(fragmentArgs[i]),
                std::get<to_underlying(ArgEn::value)>(fragmentArgs[i]),
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

    // textures artefacts
    enum class TextureEn{name, filename};
    static std::tuple<const char *, const char *> textures[] =
    {
        {g_renderFromImageName, ":/Images/Images/no-image-001.png"},
        {g_renderToImageName, ":/Images/Images/no-image-002.png"}
    };
    static const int textureBaseArtefactId = 3;
    static const int textureBaseObjectArtefactIdBase = 3;
    for(std::size_t i = 0; i < sizeof(textures)/sizeof(textures[0]); ++i)
    {
        auto textureArtefact = std::make_unique<ArtefactData>(
                    textureBaseArtefactId + i,
                    std::get<to_underlying(TextureEn::name)>(textures[i]),
                    std::get<to_underlying(TextureEn::filename)>(textures[i]),
                    emptyStr,
                    ArtefactTypeEn::texture2D,
                    emptyStr,
                    now
                    );
        auto textureObjectArtefactData = std::make_unique<ObjectArtefactData>(
                    textureBaseObjectArtefactIdBase + i,
                    effectId,
                    textureBaseArtefactId + i,
                    objectArtefactStep0,
                    textureArtefact.release()
                    );
        effectObject->m_objectArtefactData->push_back(textureObjectArtefactData.release());
    }

    return effectObject;
}

QJsonObject getJsonDataVariablePosition(const char *objectName_, const int *step_)
{
    QJsonObject position;
    if(!objectName_ && !step_)
    {
        return position;
    }
    if(objectName_)
    {
        position.insert(g_jsonDataVariableObjectNameName, QJsonValue::fromVariant(QVariant::fromValue(QString(objectName_))));
    }
    if(step_)
    {
        position.insert(g_jsonDataVariableObjectStepIndexName, QJsonValue::fromVariant(QVariant::fromValue(*step_)));
    }
    return position;
}

QString createQTGeomJson(QRandomGenerator &gen_, const char *objectName_ /*= nullptr*/, const int *step_ /*= nullptr*/)
{
    std::vector<QVector3D> vertexData;
    std::vector<QVector3D> normalData;
    test::createGeometry(vertexData, normalData);

    qreal fScale = 1;
    qreal fAngle = gen_.generateDouble() * 360.0;
    QMatrix4x4 modelview;
    modelview.rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview.rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview.rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview.scale(fScale);
    modelview.translate(0.0f, -0.2f, 0.0f);

    const bool needPosition = objectName_ || step_;
    QJsonValue position;
    if(needPosition)
    {
        position = getJsonDataVariablePosition(objectName_, step_);
    }

    QJsonArray vertexJA;
    for(const QVector3D &vec_ : vertexData)
    {
        vertexJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.x())));
        vertexJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.y())));
        vertexJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.z())));
    }
    QJsonObject vertexJO;
    vertexJO.insert(g_jsonDataVariableValueName, vertexJA);
    if(needPosition)
    {
        vertexJO.insert(g_jsonDataVariablePositionName, position);
    }
    QJsonArray normalJA;
    for(const QVector3D &vec_ : normalData)
    {
        normalJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.x())));
        normalJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.y())));
        normalJA.append(QJsonValue::fromVariant(QVariant::fromValue(vec_.z())));
    }
    QJsonObject normalJO;
    normalJO.insert(g_jsonDataVariableValueName, normalJA);
    if(needPosition)
    {
        normalJO.insert(g_jsonDataVariablePositionName, position);
    }
    QJsonArray matrixJA;
    for(std::size_t i = 0; i < 4*4; ++i)
    {
        matrixJA.append(QJsonValue::fromVariant(QVariant::fromValue(modelview.constData()[i])));
    }
    QJsonObject matrixJO;
    matrixJO.insert(g_jsonDataVariableValueName, matrixJA);
    if(needPosition)
    {
        matrixJO.insert(g_jsonDataVariablePositionName, position);
    }

    QJsonObject object;
    object.insert("vertex", vertexJO);
    object.insert("normal", normalJO);
    object.insert("matrix", matrixJO);

    return QJsonDocument(object).toJson();
}

QString createAliseQTGeomJson(
        const char *geom3dObjectName_,
        const char *objectName_ /*= nullptr*/,
        const int *step_ /*= nullptr*/
        )
{
    const bool needPosition = objectName_ || step_;
    QJsonValue position;
    if(needPosition)
    {
        position = getJsonDataVariablePosition(objectName_, step_);
    }

    QJsonObject vertexNewNameNameJO;
    if(needPosition)
    {
        vertexNewNameNameJO.insert(g_jsonDataVariablePositionName, position);
    }
    QJsonObject vertexNewNameJO;
    vertexNewNameJO.insert("vertex", vertexNewNameNameJO);
    QJsonObject vertexJO;
    vertexJO.insert(g_jsonDataVariableNamesName, vertexNewNameJO);

    QJsonObject normalNewNameNameJO;
    if(needPosition)
    {
        normalNewNameNameJO.insert(g_jsonDataVariablePositionName, position);
    }
    QJsonObject normalNewNameJO;
    normalNewNameJO.insert("normal", normalNewNameNameJO);
    QJsonObject normalJO;
    normalJO.insert(g_jsonDataVariableNamesName, normalNewNameJO);

    QJsonObject object;
    object.insert(QString("%1vertex").arg(geom3dObjectName_), vertexJO);
    object.insert(QString("%1normal").arg(geom3dObjectName_), normalJO);

    return QJsonDocument(object).toJson();
}

QString createQTGeomLuaNewVariables(
        const char *geom3dObjectName_
        )
{
    const QString inv1 = QString("%1vertex").arg(geom3dObjectName_);
    const QString inv2 = QString("%1normal").arg(geom3dObjectName_);
    QString result = QString(g_luaScript2NewVariable)
            .arg(QString("vertex"), inv1,
                 QString("normal"), inv2
                 );
    return result;
}

QString createQTGeomLuaSetVariables(
        const char *geom3dObjectName_
        )
{
    const QString inv1 = QString("%1vertex").arg(geom3dObjectName_);
    const QString inv2 = QString("%1normal").arg(geom3dObjectName_);
    QString result = QString(g_luaScript2SetVariable)
            .arg(QString("vertex"), inv1,
                 QString("normal"), inv2
                 );
    return result;
}

QString createQTGeomLua1SetVariables(
        const char *dest_,
        const char *src_
        )
{
    QString result = QString(g_luaScript1SetVariable)
            .arg(QString(dest_), QString(src_));
    return result;
}


QString createObjectsQTGeomJson(int cnt_, const char *objectName_)
{
    QJsonArray objectsJA;
    for(int i = 0; i < cnt_; ++i)
    {
        objectsJA.append(QJsonValue::fromVariant(QVariant::fromValue(QString(objectName_))));
    }
    QJsonObject objectsJO;
    objectsJO.insert(g_jsonDataVariableValueName, objectsJA);
    QJsonObject object;
    object.insert(g_jsonDataVariableNameObjectListName, objectsJO);

    return QJsonDocument(object).toJson();
}

using Argument = std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, QString, QString>;

std::unique_ptr<EffectObjectsData> createTestObject3(
        int effectId,
        const QDateTime &now,
        int effectObjectStep,
        const int artefactId,
        const ArtefactTypeEn artefactType,
        const char * artefactName,
        const char * artefactFilename,
        const int objectInfoId,
        const char *effectObjectName,
        const char *effectObjectProgrammerName,
        const std::vector<Argument> &arguments_ = std::vector<Argument>()
        )
{
    static const int effectObjectId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId,
                objectInfoId,
                effectObjectStep
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectObjectName,
                effectObjectProgrammerName,
                now
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId,
                artefactName,
                artefactFilename,
                emptyStr,
                artefactType,
                emptyStr,
                now
                );
    for(const auto &arg_ : arguments_)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(arg_),
                artefactId,
                std::get<to_underlying(ArgEn::type)>(arg_),
                std::get<to_underlying(ArgEn::storage)>(arg_),
                std::get<to_underlying(ArgEn::name)>(arg_),
                std::get<to_underlying(ArgEn::value)>(arg_),
                emptyStr,
                now
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    static const int objectArtefactStep0 = 0;
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId,
                objectArtefactStep0,
                artefact1.release()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData1.release());

    return effectObject;
}

std::unique_ptr<EffectData> createTestData1()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int objectInfoId = 1;
    static const char *effectObjectName = "qt logo";
    static const char *effectObjectProgrammerName = "gtlogo";

    auto effectObject1 = createTestObject(
                effectId,
                now,
                effectObjectStep0,
                gen,
                objectInfoId,
                effectObjectName,
                effectObjectProgrammerName
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
    static const int objectInfoId1 = 1;
    static const int objectInfoId2 = 2;
    static const char *effectObjectName = "qt logo";
    static const char *effectObjectProgrammerName = "gtlogo";

    auto effectObject1 = createTestObject(
                effectId,
                now,
                effectObjectStep0,
                gen,
                objectInfoId1,
                effectObjectName,
                effectObjectProgrammerName
                );
    auto effectObject2 = createTestObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId2,
                effectObjectName,
                effectObjectProgrammerName
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
    static const char *effectProgrammerName = "effect1";
    const QDateTime now = QDateTime::currentDateTime();
    static const int effectObjectStep0 = 0;

    auto effectObject1 = createTestObject2(
                effectId,
                effectName,
                effectProgrammerName,
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

std::unique_ptr<EffectData> createTestData4()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataJson;
    static const char *artefactName1 = "data json object";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectName = "qt logo";

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometryFilename,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId + 1,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName
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

std::unique_ptr<EffectData> createTestData5()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataJson;
    static const char *artefactName1 = "data json object";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectName = "qt logo";
    static const char *effectObjectProgrammerName = "gt_logo";

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometry0Filename,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometry1Filename,
                objectInfoId + 1,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject3 = createTestObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId + 2,
                effectObjectName,
                effectObjectProgrammerName
                );
    auto effectObject4 = createTestObject(
                effectId,
                now,
                effectObjectStep1 + 1,
                gen,
                objectInfoId + 3,
                effectObjectName,
                effectObjectProgrammerName
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    effect->m_effectObjectsData->push_back(effectObject4.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData6()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataJson;
    static const char *artefactName1 = "data json object";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectName = "qt logo";

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometry0Filename,
                objectInfoId,
                effectObjectDataName,
                g_defaultObjectInfoProgrammerName
                );
    auto effectObject2 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1 + 1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometry1Filename,
                objectInfoId + 1,
                effectObjectDataName,
                g_defaultObjectInfoProgrammerName
                );
    auto effectObject3 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1 + 1,
                artefactType1,
                artefactName1,
                g_dataJsonQTGeometry2Filename,
                objectInfoId + 2,
                effectObjectDataName,
                g_defaultObjectInfoProgrammerName
                );
    auto effectObject0 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1 + 2,
                artefactType1,
                artefactName1,
                g_dataJsonObjectsOfQtGeomFilename,
                objectInfoId + 2,
                effectObjectDataName,
                g_defaultObjectInfoProgrammerName
                );
    auto effectObject4 = createTestObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId + 3,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject0.release());
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    effect->m_effectObjectsData->push_back(effectObject4.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData7(
        const char *data3DObjectFilename_,
        const char *object3DName_,
        const char *shaderFilename_
        )
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataObj3D;
    static const char *artefactName1 = "data json object";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectName = "qt logo";

    gen.seed(time(nullptr));

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                data3DObjectFilename_,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject3DObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId + 1,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName,
                object3DName_,
                shaderFilename_
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

std::unique_ptr<EffectData> createTestData8(
        const char *data3DObjectFilename_,
        const char *dataAliasFilename_,
        const char *shaderFilename_
        )
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataObj3D;
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::convertNamesJson;
    static const char *artefactName1 = "data json object";
    static const char *artefactName2 = "data json aliases";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectDataName2 = "data aliases";
    static const char *effectObjectDataProgrammerName2 = "data_aliases_qt_logo";
    static const char *effectObjectName = "qt logo";

    gen.seed(time(nullptr));

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                data3DObjectFilename_,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject3(
                effectId,
                now,
                effectObjectStep0 + 1,
                artefactId1 + 1,
                artefactType2,
                artefactName2,
                dataAliasFilename_,
                objectInfoId + 1,
                effectObjectDataName2,
                effectObjectDataProgrammerName2
                );
    auto effectObject3 = createTestObject3DObject(
                effectId,
                now,
                effectObjectStep0 + 2,
                gen,
                objectInfoId + 2,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName,
                "",
                shaderFilename_
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData9(
        const char *data3DObjectFilename_,
        const char *dataLuaScriptFilename_,
        const char *shaderFilename_
        )
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataObj3D;
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::scriptLua;
    static const char *artefactName1 = "data json object";
    static const char *artefactName2 = "lua script new variables";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectDataName2 = "lua script new variables";
    static const char *effectObjectDataProgrammerName2 = "lua_new_variables";
    static const char *effectObjectName = "qt logo";

    gen.seed(time(nullptr));

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                data3DObjectFilename_,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject3(
                effectId,
                now,
                effectObjectStep0 + 1,
                artefactId1 + 1,
                artefactType2,
                artefactName2,
                dataLuaScriptFilename_,
                objectInfoId + 1,
                effectObjectDataName2,
                effectObjectDataProgrammerName2
                );
    auto effectObject3 = createTestObject3DObject(
                effectId,
                now,
                effectObjectStep0 + 2,
                gen,
                objectInfoId + 2,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName,
                "",
                shaderFilename_
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    return effect;
}

std::unique_ptr<EffectData> createTestData10(
        const char *data3DObjectFilename_,
        const char *dataLuaScriptFilename_,
        const char *luaRuntime1Filename_,
        const std::vector<Argument> &args1_,
        const char *luaRuntime2Filename_,
        const std::vector<Argument> &args2_,
        const char *shaderFilename_
        )
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataObj3D;
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::scriptLua;
    static const ArtefactTypeEn artefactType3 = ArtefactTypeEn::scriptLuaRuntime;
    static const char *artefactName1 = "data json object";
    static const char *artefactName2 = "lua script new variables";
    static const char *artefactName3 = "lua script runtime";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName = "data for object";
    static const char *effectObjectDataProgrammerName = "data_for_qt_logo";
    static const char *effectObjectDataName2 = "lua script new variables";
    static const char *effectObjectDataProgrammerName2 = "lua_new_variables";
    static const char *effectObjectName = "qt logo";

    gen.seed(time(nullptr));

    auto effectObject1 = createTestObject3(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                data3DObjectFilename_,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createTestObject3(
                effectId,
                now,
                effectObjectStep0 + 1,
                artefactId1 + 1,
                artefactType2,
                artefactName2,
                dataLuaScriptFilename_,
                objectInfoId + 1,
                effectObjectDataName2,
                effectObjectDataProgrammerName2
                );
    auto effectObject3 = createTestObject3(
                effectId,
                now,
                effectObjectStep0 + 2,
                artefactId1 + 2,
                artefactType3,
                artefactName3,
                luaRuntime1Filename_,
                objectInfoId + 2,
                "",
                g_defaultObjectInfoProgrammerName,
                args1_
                );
    auto effectObject4 = createTestObject3(
                effectId,
                now,
                effectObjectStep0 + 3,
                artefactId1 + 3,
                artefactType3,
                artefactName3,
                luaRuntime2Filename_,
                objectInfoId + 3,
                "",
                g_defaultObjectInfoProgrammerName,
                args2_
                );
    auto effectObject5 = createTestObject3DObject(
                effectId,
                now,
                effectObjectStep0 + 4,
                gen,
                objectInfoId + 4,
                effectObjectName,
                g_effectObjectQtLogoProgrammerName,
                "",
                shaderFilename_
                );
    std::unique_ptr<EffectData> effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    effect->m_effectObjectsData->push_back(effectObject4.release());
    effect->m_effectObjectsData->push_back(effectObject5.release());
    return effect;
}

namespace drawing_objects
{

// TODO: refactoring
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

void DataTestBase::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData4();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DataTestPosition::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData5();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DataTestObjectsList::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData6();
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DataTest3DObjectSwift::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData7(
                g_dataJson3DObjectSwiftFilename,
                "swift_",
                g_3dObjectSwiftFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DataTest3DObjectCube::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData7(
                g_dataJson3DObjectCubeFilename,
                "cube_",
                g_3dObjectCubeFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void DataTestAlias::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData8(
                g_dataJson3DObjectSwiftFilename,
                g_dataJsonAliasSwiftFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void LuaScriptTestNewVariable::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData9(
                g_dataJson3DObjectSwiftFilename,
                g_dataLua2NewVariableFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void LuaScriptTestSetVariable::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData9(
                g_dataJson3DObjectSwiftFilename,
                g_dataLua2SetVariableFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

void LuaScriptTestRuntime::initialize(drawing_data::QuizImageObjects &data_) const
{
    auto filesource = createMapFileSource();
    const std::vector<Argument> args1 = {
        {
            1,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderTName,
            "0.0"
        }
    };
    const std::vector<Argument> args2 = {
        {
            1,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "angle",
            "0.0"
        }
    };
    auto effectObjectsData = createTestData10(
                g_dataJson3DObjectSwiftFilename,
                g_dataLua2SetVariableFilename,
                g_dataLua1SetVariableAngleAsTFilename, args1,
                g_dataLuaCalcMatrixFilename, args2,
                g_3dObjectDefaultFragmentShaderFilename
                );
    ::DrawingDataEffect drawingDataEffect(std::move(*effectObjectsData));
    drawingDataEffect.init(filesource);
    drawingDataEffect.initialize(data_);
}

}
