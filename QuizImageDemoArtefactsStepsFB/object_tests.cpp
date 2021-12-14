/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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


QString createVariablesOfQTLogoJson(
        QRandomGenerator &gen_,
        const char *objectName_ = nullptr,
        const int *step_ = nullptr,
        bool addGeometry_ = true
        );
QString createVariableValueJson(
        const QString &name_,
        const double &value_,
        const char *objectName_ = nullptr,
        const int *step_ = nullptr
        );
QString createObjectsListJson(int cnt_, const char *objectName_);
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

static const char *g_baseFragmentShader =
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
    "   local pry = {a[1] * 90.0 + 90.0, 0.0, 1.0, 0.0}\n"
    "   m = matrix.rotate(m, pry)\n"
    "   local prx = {a[1] * 90.0 + 195.0, 1.0, 0.0, 0.0}\n"
    "   m = matrix.rotate(m, prx)\n"
    "   local prz = {a[1] * 90.0 + 115.0, 0.0, 0.0, 1.0}\n"
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

static const char *g_luaScriptCreateMatrxiVariablesForObjectList =
    "function getMatrixVariable ( a, shift )\n"
    "    local sz = {4, 4}\n"
    "    local m = matrix.identity(sz)\n"
    "    local pry = {(a[1] + shift) * 90.0 + 90.0, 0.0, 1.0, 0.0}\n"
    "    m = matrix.rotate(m, pry)\n"
    "    local prx = {(a[1] + shift) * 90.0 + 195.0, 1.0, 0.0, 0.0}\n"
    "    m = matrix.rotate(m, prx)\n"
    "    local prz = {(a[1] + shift) * 90.0 + 115.0, 0.0, 0.0, 1.0}\n"
    "    m = matrix.rotate(m, prz)\n"
    "    local ps = {0.9, 0.9, 0.9}\n"
    "    m = matrix.scale(m , ps)\n"
    "    local pt = {0.0, -0.2, 0.0}\n"
    "    m = matrix.translate(m , pt)\n"
    "    return m\n"
    "end\n"
    "function main ()\n"
    "    local a = getVariable(\"angle\")\n"
    "    local m = getMatrixVariable(a, 0.0)\n"
    "    setVariableWithPosition(\"%1\", m, \"%2\", 1, -1 )\n"
    "    m = getMatrixVariable(a, 0.25)\n"
    "    setVariableWithPosition(\"%1\", m, \"%2\", 2, -1 )\n"
    "    m = getMatrixVariable(a, 0.5)\n"
    "    setVariableWithPosition(\"%1\", m, \"%2\", 3, -1 )\n"
    "    result = {}\n"
    "    return result;\n"
    "end\n";
    ;


static const char *g_effectObjectQtLogoProgrammerName = "gt_logo";
static const char *g_baseVertexShaderFilename = "base.vsh";
static const char *g_baseFragmentShaderFilename = "base.fsh";
static const char *g_defaultVertexShaderFilename = "default.vsh";
static const char *g_defaultFragmentShaderFilename = "default.fsh";
static const char *g_dataJsonQTGeometryFilename = "qt_geom.json";
static const char *g_dataJsonQTLogoVariableMatrix0Filename = "qt_variables_0.json";
static const char *g_dataJsonQTLogoVariableMatrix1Filename = "qt_variables_1.json";
static const char *g_dataJsonQTLogoVariableMatrix2Filename = "qt_variables_2.json";
static const char *g_dataJsonObjectsListOfQtGeomFilename = "objects_list_qt_logo.json";
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
static const char *g_dataLuaCreateMatrixVariablesForObjectListFilename
    = "create_matrix_variables_for_object_list.lua";
static const char *g_angle_0_1_ValueFileName = "angle_0_1_value.json";


std::shared_ptr<MapFileSource> createMapFileSource()
{
    std::shared_ptr<MapFileSource> filesource = std::make_shared<MapFileSource>();
    filesource->add(g_baseVertexShaderFilename, g_baseVertexShader0);
    filesource->add(g_baseFragmentShaderFilename, g_baseFragmentShader);
    filesource->add(g_defaultVertexShaderFilename,
                    loadTextFile(":/Shaders/Shaders/test000/default.vsh")
                    );
    filesource->add(g_defaultFragmentShaderFilename,
                    loadTextFile(":/Shaders/Shaders/test000/default.fsh")
                    );
    QRandomGenerator gen;
    gen.seed(time(nullptr));
    filesource->add(g_dataJsonQTGeometryFilename,
                    createVariablesOfQTLogoJson(gen)
                    );
    const int pos0 = 1;
    filesource->add(g_dataJsonQTLogoVariableMatrix0Filename,
                    createVariablesOfQTLogoJson(
                        gen, nullptr, &pos0, false
                        )
                    );
    const int pos1 = 2;
    filesource->add(g_dataJsonQTLogoVariableMatrix1Filename,
                    createVariablesOfQTLogoJson(
                        gen, nullptr, &pos1, false
                        )
                    );
    const int pos2 = 3;
    filesource->add(g_dataJsonQTLogoVariableMatrix2Filename,
                    createVariablesOfQTLogoJson(
                        gen, nullptr, &pos2, false
                        )
                    );
    filesource->add(g_dataJsonObjectsListOfQtGeomFilename,
                    createObjectsListJson(
                        3,
                        g_effectObjectQtLogoProgrammerName
                        )
                    );
    filesource->add(g_dataJson3DObjectSwiftFilename,
                    loadTextFile(":/obj3d/swift.obj")
                    );
    filesource->add(g_dataJson3DObjectCubeFilename,
                    loadTextFile(":/obj3d/cube.obj")
                    );
    filesource->add(g_3dObjectSwiftFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject)
                        .arg("swift_vertex", "swift_normal")
                    );
    filesource->add(g_3dObjectCubeFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject)
                        .arg("cube_vertex", "cube_normal")
                    );
    filesource->add(g_3dObjectDefaultFragmentShaderFilename,
                    QString(g_baseVertexShader3DObject)
                        .arg("vertex", "normal")
                    );
    filesource->add(g_dataJsonAliasSwiftFilename,
                    createAliseQTGeomJson(
                        "swift_", nullptr, &pos2
                        )
                    );
    filesource->add(g_dataLua2NewVariableFilename,
                    createQTGeomLuaNewVariables(
                        "swift_"
                        )
                    );
    filesource->add(g_dataLua2SetVariableFilename,
                    createQTGeomLuaSetVariables("swift_")
                    );
    filesource->add(g_dataLua1SetVariableAngleAsTFilename,
                    createQTGeomLua1SetVariables(
                        "angle", g_renderTName
                        )
                    );
    filesource->add(g_dataLuaCalcMatrixFilename, g_luaScriptCalcMatrix);
    filesource->add(g_dataLuaCreateMatrixVariablesForObjectListFilename,
                    QString(g_luaScriptCreateMatrxiVariablesForObjectList)
                        .arg("matrix", g_effectObjectQtLogoProgrammerName)
                    );
    filesource->add(g_angle_0_1_ValueFileName,
                    createVariableValueJson(
                        "angle", 0.1
                        )
                    );
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
std::unique_ptr<EffectObjectsData> createDrawingQtLogoEffectObject(
        int effectId_,
        const QDateTime &now_,
        int effectObjectStep_,
        QRandomGenerator &gen_,
        const int objectInfoId_,
        const char *effectObjectName_,
        const char *effectObjectProgrammerName_
        )
{
    static const int effectObjectId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId_,
                objectInfoId_,
                effectObjectStep_,
                MergeId()
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId_,
                effectObjectName_,
                effectObjectProgrammerName_,
                now_,
                MergeId()
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    // prepare data
    std::vector<QVector3D> vertexData;
    std::vector<QVector3D> normalData;
    test::createQTLogoGeometry(vertexData, normalData);

    qreal fScale = 1;
    qreal fAngle = gen_.generateDouble() * 360.0;
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
                now_,
                MergeId()
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
                now_,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId_,
                artefactId1,
                objectArtefactStep0,
                artefact1.release(),
                MergeId()
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
                now_,
                MergeId()
                );
    static const int objectArtefactId2 = 2;
    auto objectArtefactData2 = std::make_unique<ObjectArtefactData>(
                objectArtefactId2,
                effectId_,
                artefactId2,
                objectArtefactStep0,
                artefact2.release(),
                MergeId()
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
                effectObjectStep,
                MergeId()
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectObjectName,
                effectObjectProgrammerName,
                now,
                MergeId()
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
                now,
                MergeId()
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
                now,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release(),
                MergeId()
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
                now,
                MergeId()
                );
    static const int objectArtefactId2 = 2;
    auto objectArtefactData2 = std::make_unique<ObjectArtefactData>(
                objectArtefactId2,
                effectId,
                artefactId2,
                objectArtefactStep0,
                artefact2.release(),
                MergeId()
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
                effectObjectStep,
                MergeId()
                );

    // ObjectInfoData
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId,
                effectName,
                effectProgrammerName,
                now,
                MergeId()
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
                now,
                MergeId()
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
                now,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact1->m_artefactArgData->push_back(arg.release());
    }
    auto objectArtefactData1 = std::make_unique<ObjectArtefactData>(
                objectArtefactId1,
                effectId,
                artefactId1,
                objectArtefactStep0,
                artefact1.release(),
                MergeId()
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
                now,
                MergeId()
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
                now,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact2->m_artefactArgData->push_back(arg.release());
    }
    auto objectArtefactData2 = std::make_unique<ObjectArtefactData>(
                objectArtefactId2,
                effectId,
                artefactId2,
                objectArtefactStep0,
                artefact2.release(),
                MergeId()
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
                    now,
                    MergeId()
                    );
        auto textureObjectArtefactData = std::make_unique<ObjectArtefactData>(
                    textureBaseObjectArtefactIdBase + i,
                    effectId,
                    textureBaseArtefactId + i,
                    objectArtefactStep0,
                    textureArtefact.release(),
                    MergeId()
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

QString createVariablesOfQTLogoJson(
        QRandomGenerator &gen_,
        const char *objectName_ /*= nullptr*/,
        const int *step_ /*= nullptr*/,
        bool addGeometry_ /*= true*/
        )
{
    std::vector<QVector3D> vertexData;
    std::vector<QVector3D> normalData;
    test::createQTLogoGeometry(vertexData, normalData);

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
    if(addGeometry_)
    {
        object.insert("vertex", vertexJO);
        object.insert("normal", normalJO);
    }
    object.insert("matrix", matrixJO);

    return QJsonDocument(object).toJson();
}

QString createVariableValueJson(
        const QString &name_,
        const double &value_,
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

    QJsonArray objectsJA;
    objectsJA.append(QJsonValue::fromVariant(QVariant::fromValue(value_)));
    QJsonObject valueJO;
    valueJO.insert(g_jsonDataVariableValueName, objectsJA);
    if(needPosition)
    {
        valueJO.insert(g_jsonDataVariablePositionName, position);
    }

    QJsonObject object;
    object.insert(name_, valueJO);

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


QString createObjectsListJson(int cnt_, const char *objectName_)
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

std::unique_ptr<EffectObjectsData> createEffectObjectWithOneArtefactWithArguments(
        int effectId_,
        const QDateTime &now_,
        int effectObjectStep_,
        const int artefactId_,
        const ArtefactTypeEn artefactType_,
        const char * artefactName_,
        const char * artefactFilename_,
        const int objectInfoId_,
        const char *effectObjectName_,
        const QString& effectObjectProgrammerName_,
        const std::vector<Argument> &arguments_ = std::vector<Argument>()
        )
{
    static const int effectObjectId = 1;
    std::unique_ptr<EffectObjectsData> effectObject = std::make_unique<EffectObjectsData>(
                effectObjectId,
                effectId_,
                objectInfoId_,
                effectObjectStep_,
                MergeId()
                );

    // object info
    auto objectInfoData = std::make_unique<ObjectInfoData>(
                objectInfoId_,
                effectObjectName_,
                effectObjectProgrammerName_,
                now_,
                MergeId()
                );
    effectObject->m_objectInfoData->push_back(objectInfoData.release());

    auto artefact = std::make_unique<ArtefactData>(
                artefactId_,
                artefactName_,
                artefactFilename_,
                emptyStr,
                artefactType_,
                emptyStr,
                now_,
                MergeId()
                );
    for(const auto &arg_ : arguments_)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(arg_),
                artefactId_,
                std::get<to_underlying(ArgEn::type)>(arg_),
                std::get<to_underlying(ArgEn::storage)>(arg_),
                std::get<to_underlying(ArgEn::name)>(arg_),
                std::get<to_underlying(ArgEn::value)>(arg_),
                emptyStr,
                now_,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact->m_artefactArgData->push_back(arg.release());
    }
    static const int objectArtefactId1 = 1;
    static const int objectArtefactStep0 = 0;
    auto objectArtefactData = std::make_unique<ObjectArtefactData>(
                objectArtefactId1 + artefactId_,
                effectId_,
                artefactId_,
                objectArtefactStep0,
                artefact.release(),
                MergeId()
                );
    effectObject->m_objectArtefactData->push_back(objectArtefactData.release());

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

    auto effectObject1 = createDrawingQtLogoEffectObject(
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
                now,
                MergeId()
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

    auto effectObject1 = createDrawingQtLogoEffectObject(
                effectId,
                now,
                effectObjectStep0,
                gen,
                objectInfoId1,
                effectObjectName,
                effectObjectProgrammerName
                );
    auto effectObject2 = createDrawingQtLogoEffectObject(
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
                now,
                MergeId()
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
                now,
                MergeId()
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject2 = createDrawingQtLogoEffectObject(
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
                now,
                MergeId()
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTLogoVariableMatrix0Filename,
                objectInfoId,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject2 = createEffectObjectWithOneArtefactWithArguments(
                effectId,
                now,
                effectObjectStep0,
                artefactId1,
                artefactType1,
                artefactName1,
                g_dataJsonQTLogoVariableMatrix1Filename,
                objectInfoId + 1,
                effectObjectDataName,
                effectObjectDataProgrammerName
                );
    auto effectObject3 = createDrawingQtLogoEffectObject(
                effectId,
                now,
                effectObjectStep1,
                gen,
                objectInfoId + 2,
                effectObjectName,
                effectObjectProgrammerName
                );
    auto effectObject4 = createDrawingQtLogoEffectObject(
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
                now,
                MergeId()
                );
    effect->m_effectObjectsData->push_back(effectObject1.release());
    effect->m_effectObjectsData->push_back(effectObject2.release());
    effect->m_effectObjectsData->push_back(effectObject3.release());
    effect->m_effectObjectsData->push_back(effectObject4.release());
    return effect;
}

std::unique_ptr<EffectData> createEffectDataForTestOfObjectList()
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

    // create affect
    auto effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now,
                MergeId()
                );
    // create Object List Json Artefact as main object with duplication of object to draw
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1 + 2,
                    artefactType1,
                    artefactName1,
                    g_dataJsonObjectsListOfQtGeomFilename,
                    objectInfoId + 2,
                    effectObjectDataName,
                    g_defaultObjectInfoProgrammerName
                    ).release()
                );
    // create variable matrix for qt_logo, at pos 1
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1,
                    artefactType1,
                    artefactName1,
                    g_dataJsonQTLogoVariableMatrix0Filename,
                    objectInfoId,
                    effectObjectDataName,
                    g_defaultObjectInfoProgrammerName
                    ).release()
                );
    // create variable matrix for qt_logo, at pos 2
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1 + 1,
                    artefactType1,
                    artefactName1,
                    g_dataJsonQTLogoVariableMatrix1Filename,
                    objectInfoId + 1,
                    effectObjectDataName,
                    g_defaultObjectInfoProgrammerName
                    ).release()
                );
    // add artefact to draw qt logo with shaders and geometry variables and default matrix variable
    effect->m_effectObjectsData->push_back(
                createDrawingQtLogoEffectObject(
                    effectId,
                    now,
                    effectObjectStep1,
                    gen,
                    objectInfoId + 3,
                    effectObjectName,
                    g_effectObjectQtLogoProgrammerName
                    ).release()
                );
    return effect;
}

std::unique_ptr<EffectData> createEffectDataForTestOfLuaScriptArguments(int variation_)
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataJson;
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::scriptLua;
    static const char *artefactName1 = "data json object";
    static const char *artefactName2 = "lua script to create variables";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName1 = "data for object";
    static const char *effectObjectDataName2 = "lua script to create variables";
    static const char *effectObjectName = "qt logo";

    // create affect
    auto effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now,
                MergeId()
                );
    // create Object List Json Artefact as main object with duplication of object to draw
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1,
                    artefactType1,
                    artefactName1,
                    g_dataJsonObjectsListOfQtGeomFilename,
                    objectInfoId,
                    effectObjectDataName1,
                    g_defaultObjectInfoProgrammerName
                    ).release()
                );
    // add variable angle json data
    if(variation_ == 1)
    {
        effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1 + 1,
                    artefactType1,
                    artefactName1,
                    g_angle_0_1_ValueFileName,
                    objectInfoId + 1,
                    effectObjectDataName1,
                    g_defaultObjectInfoProgrammerName
                    ).release()
                );
    }
    const std::vector<Argument> luaScriptArgs = {
        {
            1,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "angle",
            "-0.5"
        }
    };
    // create variable matrix for qt_logo, at positions 1,2,3
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1 + 2,
                    artefactType2,
                    artefactName2,
                    g_dataLuaCreateMatrixVariablesForObjectListFilename,
                    objectInfoId + 2,
                    effectObjectDataName2,
                    g_defaultObjectInfoProgrammerName,
                    luaScriptArgs
                    ).release()
                );
    // add artefact to draw qt logo with shaders and geometry variables and default matrix variable
    effect->m_effectObjectsData->push_back(
                createDrawingQtLogoEffectObject(
                    effectId,
                    now,
                    effectObjectStep1,
                    gen,
                    objectInfoId + 3,
                    effectObjectName,
                    g_effectObjectQtLogoProgrammerName
                    ).release()
                );
    return effect;
}

class TestDataEffectArgs: public EffectArgumentData
{
public:
    TestDataEffectArgs(
            int argStorageId_,
            int argTypeId_,
            const QString &name_,
            const QString &defaultValue_,
            int objectArtefactId_
            )
        : EffectArgumentData(argStorageId_, argTypeId_, name_, defaultValue_, false, false, QString(), QString()),
          m_objectArtefactId(objectArtefactId_)
    {
    }

    int m_objectArtefactId = -1;
};

std::unique_ptr<QVector<EffectArgData *>> createEffectArgsData(
        const QVector<TestDataEffectArgs> &args_,
        int effectId_,
        int effectArgsBaseId_,
        const QString &description_,
        const QDateTime &now_
        )
{
    std::unique_ptr<QVector<EffectArgData *>> argsData = std::make_unique<QVector<EffectArgData *>>();
    int idShift = 0;
    for(const TestDataEffectArgs &arg_ : args_)
    {
        auto arg = std::make_unique<EffectArgData>();
        static_cast<EffectArgumentData&>(*arg) = static_cast<const EffectArgumentData&>(arg_);
        arg->m_id = effectArgsBaseId_ + idShift;
        ++idShift;
        arg->m_effectId = effectId_;
        arg->m_objectArtefactId = arg_.m_objectArtefactId;
        arg->m_description = QString(description_).arg(arg->m_id);
        arg->m_created = now_;
        argsData->push_back(arg.release());
    }
    return argsData;
}

class TestArgValue
{
public:
    TestArgValue(
            int argId_,
            const QString &value_
            )
        : m_argId(argId_),
          m_value(value_)
    {
    }
    int m_argId = -1;
    QString m_value;
};

std::unique_ptr<QVector<EffectArgSetData *>> createEffectArgSetsData(
        const QVector<QVector<TestArgValue>> &argSets_,
        const QVector<EffectArgData *> *argsData_,
        int effectId_,
        int argSetBaseId_,
        int argValueBaseId_,
        const QString &argValueDescription_,
        const QString &argSetDescription_,
        const QDateTime &now_
        )
{
    std::unique_ptr<QVector<EffectArgSetData *>> argSets =
            std::make_unique<QVector<EffectArgSetData *>>();
    if(!argsData_)
    {
        return argSets;
    }

    int argValueShiftId = 0;
    int argSetShiftId = 0;
    for(const QVector<TestArgValue> &args_: argSets_)
    {
        auto effectArgSet = std::make_unique<EffectArgSetData>();
        for(const TestArgValue & arg_ : args_)
        {
            auto effectArgValue = std::make_unique<EffectArgValueData>();
            const auto fit = std::find_if(
                        std::begin(*argsData_),
                        std::end(*argsData_),
                        [&arg_](const EffectArgData *argData_)
            {
                return argData_ && argData_->m_id == arg_.m_argId;
            });
            if(std::end(*argsData_) == fit)
            {
                continue;
            }

            effectArgValue->m_effectArgsData->push_back((*fit)->getDataCopy().release());
            auto effectArgumentData = std::make_unique<EffectArgumentData>();
            effectArgumentData->m_argStorageId = (*fit)->m_argStorageId;
            effectArgumentData->m_argTypeId = (*fit)->m_argTypeId;
            effectArgumentData->m_name = (*fit)->m_name;
            effectArgumentData->m_defaultValue = (*fit)->m_defaultValue;
            effectArgValue->m_effectArgValuesData->push_back(effectArgumentData.release());
            effectArgValue->m_id = argValueBaseId_ + argValueShiftId;
            ++argValueShiftId;
            effectArgValue->m_argSetId = argSetBaseId_ + argSetShiftId;
            effectArgValue->m_argId = (*fit)->m_id;
            effectArgValue->m_value = arg_.m_value;
            effectArgValue->m_description = QString(argValueDescription_).arg(effectArgValue->m_id);
            effectArgValue->m_created = now_;
            effectArgSet->m_effectArgValuesData->push_back(effectArgValue.release());
        }
        effectArgSet->m_id = argSetBaseId_ + argSetShiftId;
        ++argSetShiftId;
        effectArgSet->m_effectId = effectId_;
        effectArgSet->m_easingTypeId = 1;
        effectArgSet->m_description = QString(argSetDescription_).arg(effectArgSet->m_id);
        effectArgSet->m_created = now_;
        argSets->push_back(effectArgSet.release());
    }
    return argSets;
}

std::unique_ptr<EffectData> createEffectDataForTestOfArgSet()
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    const QDateTime now = QDateTime::currentDateTime();
    QRandomGenerator gen;
    static const int effectObjectStep0 = 0;
    static const int effectObjectStep1 = 1;
    static const int artefactId1 = 1;
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::dataJson;
    static const ArtefactTypeEn artefactType2 = ArtefactTypeEn::scriptLua;
    static const char *artefactName1 = "data json object";
    static const char *artefactName2 = "lua script to create variables";
    static const int objectInfoId = 1;
    static const char *effectObjectDataName1 = "data for object";
    static const char *effectObjectDataName2 = "lua script to create variables";
    static const char *effectObjectName = "qt logo";

    // create affect
    auto effect = std::make_unique<EffectData>(
                effectId,
                effectName,
                emptyStr,
                now,
                MergeId()
                );

    QVector<TestDataEffectArgs> args = {
        {
            int(ArtefactArgStorageEn::uniformStorage),
            int(ArtefactArgTypeEn::floatType),
            "angle",
            "-0.5",
            artefactId1 + 2 + 1
        }
    };
    effect->m_effectArgsData = createEffectArgsData(
                args,
                effectId,
                1,
                "description of %d",
                now
                );
    QVector<QVector<TestArgValue>> argSets = {
        {
            {
                1,
                "0.5"
            },
        },
    };
    effect->m_effectArgSetsData = createEffectArgSetsData(
                argSets,
                effect->m_effectArgsData.get(),
                effectId,
                1,
                1,
                "arg value description %d",
                "arg set description %d",
                now
                );
    // create Object List Json Artefact as main object with duplication of object to draw
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1,
                    artefactType1,
                    artefactName1,
                    g_dataJsonObjectsListOfQtGeomFilename,
                    objectInfoId,
                    effectObjectDataName1,
                    QString(g_defaultObjectInfoProgrammerName) + "_objectList"
                    ).release()
                );
    const std::vector<Argument> luaScriptArgs = {
        {
            1,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "angle",
            "-0.5"
        }
    };
    // create variable matrix for qt_logo, at positions 1,2,3
    effect->m_effectObjectsData->push_back(
                createEffectObjectWithOneArtefactWithArguments(
                    effectId,
                    now,
                    effectObjectStep0,
                    artefactId1 + 2,
                    artefactType2,
                    artefactName2,
                    g_dataLuaCreateMatrixVariablesForObjectListFilename,
                    objectInfoId + 2,
                    effectObjectDataName2,
                    QString(g_defaultObjectInfoProgrammerName) + "_luaScriptMatrixCreate",
                    luaScriptArgs
                    ).release()
                );
    // add artefact to draw qt logo with shaders and geometry variables and default matrix variable
    effect->m_effectObjectsData->push_back(
                createDrawingQtLogoEffectObject(
                    effectId,
                    now,
                    effectObjectStep1,
                    gen,
                    objectInfoId + 3,
                    effectObjectName,
                    g_effectObjectQtLogoProgrammerName
                    ).release()
                );
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
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
                now,
                MergeId()
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject2 = createEffectObjectWithOneArtefactWithArguments(
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
                now,
                MergeId()
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject2 = createEffectObjectWithOneArtefactWithArguments(
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
                now,
                MergeId()
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

    auto effectObject1 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject2 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject3 = createEffectObjectWithOneArtefactWithArguments(
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
    auto effectObject4 = createEffectObjectWithOneArtefactWithArguments(
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
                now,
                MergeId()
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
void BaseTest::initialize(drawing_data::QuizImageObjects &data_,
                          int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData1();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void MultipleObjectsTest::initialize(drawing_data::QuizImageObjects &data_,
                                     int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData2();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DefaultTest::initialize(drawing_data::QuizImageObjects &data_,
                             int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData3();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTestBase::initialize(drawing_data::QuizImageObjects &data_,
                              int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData4();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTestPosition::initialize(drawing_data::QuizImageObjects &data_,
                                  int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData5();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTestObjectsList::initialize(drawing_data::QuizImageObjects &data_,
                                     int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createEffectDataForTestOfObjectList();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTest3DObjectSwift::initialize(drawing_data::QuizImageObjects &data_,
                                       int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData7(
                g_dataJson3DObjectSwiftFilename,
                "swift_",
                g_3dObjectSwiftFragmentShaderFilename
                );
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTest3DObjectCube::initialize(drawing_data::QuizImageObjects &data_,
                                      int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData7(
                g_dataJson3DObjectCubeFilename,
                "cube_",
                g_3dObjectCubeFragmentShaderFilename
                );
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void DataTestAlias::initialize(drawing_data::QuizImageObjects &data_,
                               int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData8(
                g_dataJson3DObjectSwiftFilename,
                g_dataJsonAliasSwiftFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void LuaScriptTestNewVariable::initialize(drawing_data::QuizImageObjects &data_,
                                          int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData9(
                g_dataJson3DObjectSwiftFilename,
                g_dataLua2NewVariableFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void LuaScriptTestSetVariable::initialize(drawing_data::QuizImageObjects &data_,
                                          int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createTestData9(
                g_dataJson3DObjectSwiftFilename,
                g_dataLua2SetVariableFilename,
                g_3dObjectDefaultFragmentShaderFilename
                );
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void LuaScriptTestRuntime::initialize(drawing_data::QuizImageObjects &data_,
                                      int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

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
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void LuaScriptArgTest0::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createEffectDataForTestOfLuaScriptArguments(0);
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void LuaScriptArgTest1::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createEffectDataForTestOfLuaScriptArguments(1);
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

void ArgSetBaseTest::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    auto filesource = createMapFileSource();
    auto effectObjectsData = createEffectDataForTestOfArgSet();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

}
