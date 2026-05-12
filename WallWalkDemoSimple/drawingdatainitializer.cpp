#include "drawingdatainitializer.h"
#include <QRandomGenerator>
#include <QDir>
#include "drawingdatainitializer_utils.h"
#include "tests/logoGeomerty.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaModels/effect_data.h"
#include "../MastactvaModels/drawingdata_effectdemo.h"
#include "../MastactvaBase/format.h"
#include "../MastactvaBase/names.h"

static const char *emptyStr = "";

std::unique_ptr<EffectObjectsData> createEffectObjectDataWithObjectInfo(
        int effectId,
        const QString &effectName,
        const QString &effectProgrammerName,
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
    return effectObject;
}

enum class ArgEn{id, type, storage, name, value};
using ArgumentsTuple = std::tuple<int, ArtefactArgTypeEn, ArtefactArgStorageEn, QString, QString>;

template<std::size_t size_>
void processArtefact(
        std::unique_ptr<EffectObjectsData> &effectObject_,
        const QString &shaderFilename_,
        const int &artefactId_,
        const char *artefactName_,
        const ArtefactTypeEn &artefactType_,
        const int &objectArtefactId_,
        const int &objectArtefactStep_,
        const int &effectId_,
        const QDateTime &now_,
        const ArgumentsTuple (&vertexArgs_)[size_]
        )
{
    std::unique_ptr<ArtefactData> artefact = std::make_unique<ArtefactData>(
                    artefactId_,
                    artefactName_,
                    shaderFilename_,
                    emptyStr,
                    artefactType_,
                    emptyStr,
                    now_,
                    MergeId()
                    );
    for(std::size_t i = 0; i < sizeof(vertexArgs_)/sizeof(vertexArgs_[0]); ++i)
    {
        auto arg = std::make_unique<ArtefactArgData>(
                std::get<to_underlying(ArgEn::id)>(vertexArgs_[i]),
                artefactId_,
                std::get<to_underlying(ArgEn::type)>(vertexArgs_[i]),
                std::get<to_underlying(ArgEn::storage)>(vertexArgs_[i]),
                std::get<to_underlying(ArgEn::name)>(vertexArgs_[i]),
                std::get<to_underlying(ArgEn::value)>(vertexArgs_[i]),
                emptyStr,
                now_,
                MergeId(),
                false, false, QString(), QString()
                );
        artefact->m_artefactArgData->push_back(arg.release());
    }
    auto objectArtefactData = std::make_unique<ObjectArtefactData>(
                objectArtefactId_,
                effectId_,
                artefactId_,
                objectArtefactStep_,
                artefact.release(),
                MergeId()
                );
    effectObject_->m_objectArtefactData->push_back(objectArtefactData.release());
}

enum class TextureEn{name, filename};
using TextureTuple = std::tuple<QString, QString>;

template<std::size_t size_>
void processTexturesArtefacts(
        std::unique_ptr<EffectObjectsData> &effectObject_,
        const int &textureBaseArtefactId_,
        const int &textureBaseObjectArtefactId_,
        const int &objectArtefactStep_,
        const int &effectId_,
        const QDateTime &now_,
        const TextureTuple (&textures_)[size_]
        )
{
    for(std::size_t i = 0; i < sizeof(textures_)/sizeof(textures_[0]); ++i)
    {
        auto textureArtefact = std::make_unique<ArtefactData>(
                    textureBaseArtefactId_ + i,
                    std::get<to_underlying(TextureEn::name)>(textures_[i]),
                    std::get<to_underlying(TextureEn::filename)>(textures_[i]),
                    emptyStr,
                    ArtefactTypeEn::texture2D,
                    emptyStr,
                    now_,
                    MergeId()
                    );
        auto textureObjectArtefactData = std::make_unique<ObjectArtefactData>(
                    textureBaseObjectArtefactId_ + i,
                    effectId_,
                    textureBaseArtefactId_ + i,
                    objectArtefactStep_,
                    textureArtefact.release(),
                    MergeId()
                    );
        effectObject_->m_objectArtefactData->push_back(textureObjectArtefactData.release());
    }
}

std::unique_ptr<EffectObjectsData> createWalkEffectOnePassTestObject(
        int effectId,
        const char *effectName,
        const char *effectProgrammerName,
        const QDateTime &now,
        int effectObjectStep,
        const QString &vertexShaderFilename_,
        const QString &fragmentShaderFilename_,
        const QString &fromImage_,
        const QString &toImage_,
        const std::vector<GLfloat> &fromCoords_,
        const std::vector<GLfloat> &toCoords_,
        const QPoint &geometrySize_
        )
{
    std::unique_ptr<EffectObjectsData> effectObject = createEffectObjectDataWithObjectInfo(
                effectId,
                effectName,
                effectProgrammerName,
                now,
                effectObjectStep
                );

    std::vector<GLfloat> vertexData;
    std::vector<GLfloat> textureData;

    const QString geomertySizeStr = QString("%1 %2").arg(geometrySize_.x()).arg(geometrySize_.y());

    //opengl_drawing::makeGeometry(geometrySize_.x(), geometrySize_.y(), 0.0, 0.0, 4, 4, true, true, vertexData, textureData);

    // vertex shader artefact
    const ArgumentsTuple vertexArgs1[] =
    {
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
            geomertySizeStr
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
        },
        {
            11,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            "textureAttributeFrom",
            toString(fromCoords_)
        },
        {
            12,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::attributeStorage,
            "textureAttributeTo",
            toString(toCoords_)
        },
        {
            13,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::uniformStorage,
            QString(g_renderFromImageName) + QString(g_renderBorderColorStateName),
            "0.0 0.0 0.0 0.0"
        },
        {
            14,
            ArtefactArgTypeEn::vec4Type,
            ArtefactArgStorageEn::uniformStorage,
            QString(g_renderToImageName) + QString(g_renderBorderColorStateName),
            "0.0 0.0 0.0 0.0"
        },
        {
            15,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "vaFromMatrix",
            toString(QMatrix4x4{})
        },
        {
            16,
            ArtefactArgTypeEn::mat4Type,
            ArtefactArgStorageEn::uniformStorage,
            "vaToMatrix",
            toString(QMatrix4x4{})
        }
    };

    //qDebug() << "toString(fromCoords_)" << toString(fromCoords_);
    //qDebug() << "toString(toCoords_)" << toString(toCoords_);

    static const int objectArtefactStep0 = 0;
    processArtefact(
        effectObject,
        vertexShaderFilename_,
        1,
        "vertext shader",
        ArtefactTypeEn::shaderVertex,
        1,
        objectArtefactStep0,
        effectId,
        now,
        vertexArgs1
    );

    // fragment shader artefact
    const ArgumentsTuple fragmentArgs1[] =
    {
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
    processArtefact(
        effectObject,
        fragmentShaderFilename_,
        2,
        "fragment shader",
        ArtefactTypeEn::shaderFragmet,
        2,
        objectArtefactStep0,
        effectId,
        now,
        fragmentArgs1
    );

    // textures artefacts
    static const TextureTuple textures[] =
    {
        { g_renderFromImageName, fromImage_ },
        { g_renderToImageName, toImage_ }
    };
    static const int textureBaseArtefactId = 3;
    static const int textureBaseObjectArtefactId = 3;
    processTexturesArtefacts(
                effectObject,
                textureBaseArtefactId,
                textureBaseObjectArtefactId,
                objectArtefactStep0,
                effectId,
                now,
                textures
                );
    return effectObject;
}

std::unique_ptr<EffectObjectsData> createGlobalDataTestObject(
        int effectId,
        const QString &effectName,
        const QString &effectProgrammerName,
        const QDateTime &now,
        int effectObjectStep,
        const QVector3D &fillColor_,
        const QString &globalCalculations_ = QString(),
        const QStringList &textureNamesList_ = QStringList{}
        )
{
    std::unique_ptr<EffectObjectsData> effectObject = createEffectObjectDataWithObjectInfo(
                effectId,
                effectName,
                effectProgrammerName,
                now,
                effectObjectStep
                );

    // vertex shader artefact
    const ArgumentsTuple globalArgs[] =
    {
        {
            1,
            ArtefactArgTypeEn::vec3Type,
            ArtefactArgStorageEn::uniformStorage,
            g_renderFillColor,
            toString(fillColor_)
        },
        {
            2,
            ArtefactArgTypeEn::stringsType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderGlobalCalculationsName,
            globalCalculations_
        },
        {
            103,
            ArtefactArgTypeEn::stringsType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderTexturesListName,
            textureNamesList_.join(g_renderObjectsStatesSpliter)
        }
    };
    static const int objectArtefactStep0 = 0;
    processArtefact(
        effectObject,
        names::emptyFilename,
        1,
        "effect global setting",
        ArtefactTypeEn::scriptLua,
        1,
        objectArtefactStep0,
        effectId,
        now,
        globalArgs
    );

    return effectObject;
}

std::unique_ptr<EffectObjectsData> createDrawingQtLogoEffectObject(
        int effectId_,
        const QDateTime &now_,
        int effectObjectStep_,
        QRandomGenerator &gen_,
        const int objectInfoId_,
        const char *effectObjectName_,
        const char *effectObjectProgrammerName_,
        float alpha_ = 1.0,
        const QString &alphaBlendingMode_
            = QString(g_alphaBlendingDisable)
                + QString(g_renderObjectsStatesSpliter)
                + QString(g_depthTestEnable)
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
        },
        {
            4,
            ArtefactArgTypeEn::floatType,
            ArtefactArgStorageEn::uniformStorage,
            "alpha",
            toString(alpha_)
        },
        {
            4,
            ArtefactArgTypeEn::stringsType,
            ArtefactArgStorageEn::uniformStorage,
            g_renderObjectsStatesName,
            alphaBlendingMode_
        }
    };
    static const int artefactId1 = 1;
    static const char *artefactName1 = "vertext shader";
    static const ArtefactTypeEn artefactType1 = ArtefactTypeEn::shaderVertex;
    auto artefact1 = std::make_unique<ArtefactData>(
                artefactId1,
                artefactName1,
                names::baseVertexShaderFilename,
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
                names::baseFragmentShaderFilename,
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

std::unique_ptr<EffectData> createWalkEffectOnePassTestData(
        const QPair<QString,QString> &filenames_,
        const QPair<std::vector<QVector4D>, std::vector<QVector4D>> &coordinates_
        )
{
    static const int effectId = 1;
    static const char *effectName = "effect #1";
    static const char *effectProgrammerName = "effect1";
    static const char *effectNameMain = "global data";
    static QString effectProgrammerNameMain = QString(g_defaultObjectInfoProgrammerName) + "_globalData";
    const QDateTime now = QDateTime::currentDateTime();
    static const int effectObjectStep0 = 0;

    std::vector<GLfloat> fromCoords, toCoords;
    createGeometry(10, 10, coordinates_.first, fromCoords);
    createGeometry(10, 10, coordinates_.second, toCoords);

    //qDebug() << "fromCoords" << fromCoords;
    //qDebug() << "toCoords" << toCoords;

    auto effectObject0 = createWalkEffectOnePassTestObject(
                effectId,
                effectName,
                effectProgrammerName,
                now,
                effectObjectStep0,
                names::walkEffectOnePassVertexShaderFilename,
                names::walkEffectOnePassFragmentShaderFilename,
                absoluteHomePath(filenames_.first),
                absoluteHomePath(filenames_.second),
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

TestsBase::TestsBase()
{
}

void TestsBase::initData(const QVector<QPair<QString, QString>>& filenames_,
                                 const QVector<QPair<std::vector<QVector4D>, std::vector<QVector4D>>>& coordinates_,
                                 const QDir& sourceImageDir_)
{
    m_sourceImageDir = sourceImageDir_;
    m_filenames = filenames_;
    m_coordinates = coordinates_;
}

void BaseTest::initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ /*= 0*/) const
{
    Q_UNUSED(argsSetIndex_);

    std::shared_ptr<MapFileSource> filesource = createMapFileSource();
    auto effectObjectsData = createTestData1();
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

std::pair<const char *, const char *> BaseTest::getDescription() const
{
    return {"BaseTest", "base test"};
}

void WalkEffectOnePass::initialize(
        drawing_data::QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);
    Q_UNUSED(data_);

    std::shared_ptr<MapFileSource> filesource = createMapFileSource();
    filesource->loadImage(m_filenames.front().first, m_sourceImageDir);
    filesource->loadImage(m_filenames.front().second, m_sourceImageDir);
    auto effectObjectsData = createWalkEffectOnePassTestData(m_filenames.front(), m_coordinates.front());
    auto drawingDataEffect = std::make_unique<::DrawingDataEffect>(std::move(*effectObjectsData));
    drawingDataEffect->init(filesource);
    drawingDataEffect->initialize(data_);
}

std::pair<const char *, const char *> WalkEffectOnePass::getDescription() const
{
    return {"WalkEffectOnPathTest", "walk effect one path version"};
}

} //namespace drawing_objects
