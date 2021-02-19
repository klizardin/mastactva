#include "quizimagedata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaBase/utils.h"


static const char *g_rand = "rand";
static const char *g_renderRectSize = "renderRectSize";
//static const int g_renderState


void ArgumentInfo::setArgId(int argId_)
{
    m_argId = argId_;
}

int ArgumentInfo::getArgId() const
{
    return m_argId;
}

void ArgumentInfo::setName(const QString &name_)
{
    m_name = name_;
}

void ArgumentInfo::setType(const QString &type_)
{
    using TypeInfo = QPair<QString, QPair<int, QPair<bool, bool>>>;
    static const TypeInfo typeInfos[] = {
        {"int", {1, {false, false}}},
        {"float", {1, {true, false}}},
        {"vec2", {2, {true, false}}},
        {"vec3", {3, {true, false}}},
        {"vec4", {4, {true, false}}},
        {"mat2", {4, {true, true}}},
        {"mat3", {9, {true, true}}},
        {"mat4", {16, {true, true}}},
    };

    const auto fit = std::find_if(std::begin(typeInfos), std::end(typeInfos),
                                  [&type_](const TypeInfo &ti_)->bool
    {
        return ti_.first == type_;
    });

    Q_ASSERT(std::end(typeInfos) != fit);   // unknown type
    if(std::end(typeInfos) == fit) { return; }

    m_floatType = fit->second.second.first;
    m_matrixType = fit->second.second.second;
    m_size = fit->second.first;
    if(m_floatType) { m_valueFloat.resize(m_size); }
    else { m_valueInt.resize(m_size); }
}

void ArgumentInfo::setValue(const QString &value_)
{
    m_argumentsSetInitfuctionFunc = &ArgumentsSet::nullInitialization;
    if(value_.contains(g_rand))
    {
        if(m_floatType)
        {
            QVector<GLfloat> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformRealRands(args, m_valueFloat);
        }
        else
        {
            QVector<GLint> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformIntRands(args, m_valueInt);
        }
    }
    else if(value_.contains(g_renderRectSize))
    {
        m_argumentsSetInitfuctionFunc = &ArgumentsSet::getRenderRectSize;
    }
    else
    {
        if(m_floatType)
        {
            extractValues(value_, m_valueFloat);
        }
        else
        {
            extractValues(value_, m_valueInt);
        }
    }
}

void ArgumentInfo::setValue(const QVariantList &values_)
{
    if(m_floatType)
    {
        extractValues(values_, m_valueFloat);
    }
    else
    {
        extractValues(values_, m_valueInt);
    }
}

void ArgumentInfo::initValueFromArtumentsSet(ArgumentsSet *arguments_)
{
    QVariantList values;
    if(!(arguments_->*m_argumentsSetInitfuctionFunc)(values)) { return; }
    setValue(values);
}

void ArgumentInfo::initId(QOpenGLShaderProgram *program_)
{
    m_id = program_->uniformLocation(m_name);
}

void ArgumentInfo::setValue(QOpenGLShaderProgram *program_) const
{
    if(1 == m_size && !m_floatType)
    {
        program_->setUniformValue(m_id, m_valueInt[0]);
    }
    else if(1 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0]);
    }
    else if(2 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0],
                m_valueFloat[1]);
    }
    else if(3 == m_size && m_floatType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0],
                m_valueFloat[1], m_valueFloat[2]);
    }
    else if(4 == m_size && m_floatType && !m_matrixType)
    {
        program_->setUniformValue(m_id, m_valueFloat[0],
                m_valueFloat[1], m_valueFloat[2], m_valueFloat[3]);
    }
    else if(4 == m_size && m_floatType && m_matrixType)
    {
        QMatrix2x2 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(9 == m_size && m_floatType && m_matrixType)
    {
        QMatrix3x3 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(16 == m_size && m_floatType && m_matrixType)
    {
        QMatrix4x4 m(&m_valueFloat[0]);
        program_->setUniformValue(m_id, m);
    }
    else
    {
        Q_ASSERT(false); // unknown type
    }
}

bool ArgumentsSet::nullInitialization(QVariantList &values_)
{
    Q_UNUSED(values_);
    return false;
}

bool ArgumentsSet::getRenderRectSize(QVariantList &values_)
{
    // TODO: implement
    Q_UNUSED(values_);
    return false;
}


QuizImageData::QuizImageData()
{
    m_newFromImageUrl = g_noImage;
    m_newToImageUrl = g_noImage;
}

void QuizImageData::setFromImageUrl(const QString &fromImageUrl_)
{
    m_newFromImageUrl = fromImageUrl_;
    if(m_newFromImageUrl.isEmpty() || m_newFromImageUrl == g_noImageQRC)
    {
        m_newFromImageUrl = g_noImage;
    }
}

void QuizImageData::setToImageUrl(const QString &toImageUrl_)
{
    m_newToImageUrl = toImageUrl_;
    if(m_newToImageUrl.isEmpty() || m_newToImageUrl == g_noImageQRC)
    {
        m_newToImageUrl = g_noImage;
    }
}

bool QuizImageData::fromImageUrlChanged() const
{
    return m_newFromImageUrl != m_fromImageUrl;
}

void QuizImageData::useNewFromImageUrl()
{
    m_fromImageUrl = m_newFromImageUrl;
}

bool QuizImageData::toImageUrlChanged() const
{
    return m_newToImageUrl != m_toImageUrl;
}

void QuizImageData::useNewToImageUrl()
{
    m_toImageUrl = m_newToImageUrl;
}

bool QuizImageData::isSwapImages() const
{
    return m_newFromImageUrl != m_fromImageUrl &&
            m_newToImageUrl != m_toImageUrl &&
            m_newFromImageUrl == m_toImageUrl &&
            m_newToImageUrl == m_fromImageUrl
            ;
}

void QuizImageData::swapImages()
{
    if(!isSwapImages()) { return; }
    useNewFromImageUrl();
    useNewToImageUrl();
}

void QuizImageData::setEffectId(int effectId_)
{
    m_newEffectId = effectId_;
}

void QuizImageData::setArgumentSetId(int argumentSetId_)
{
    m_newArgumentSetId = argumentSetId_;
}

bool QuizImageData::effectChanged() const
{
    return m_newEffectId != m_effectId || m_newArgumentSetId != m_argumentSetId;
}

void QuizImageData::useNewEffect()
{
    m_newEffectId = m_effectId;
    m_newArgumentSetId = m_argumentSetId;
}

void QuizImageData::setShaders(const QString &vertexShader_, const QString &fragmentShader_)
{
    m_vertexShader = vertexShader_;
    m_fragmentShader = fragmentShader_;
}

void QuizImageData::setVertexShader(const QString &vertexShader_)
{
    m_vertexShader = vertexShader_;
}

void QuizImageData::setFragmentShader(const QString &fragmentShader_)
{
    m_fragmentShader = fragmentShader_;
}

void QuizImageData::initDefaultShaders()
{
    if(m_vertexShader.isEmpty())
    {
        m_vertexShader = ::loadTextFile(":/default.vert");
    }
    if(m_fragmentShader.isEmpty())
    {
        m_fragmentShader = ::loadTextFile(":/default.frag");
    }
}

void QuizImageData::setArguments(const QList<ArgumentInfo> &arguments_)
{
    m_arguments = arguments_;
}

void QuizImageData::clearArguments()
{
    m_arguments.clear();
}

void QuizImageData::appendArguments(const ArgumentInfo &argument_)
{
    m_arguments.push_back(argument_);
}

void QuizImageData::setArgumentValue(int argId_, const QString &value_)
{
    const auto fita = std::find_if(
                std::begin(m_arguments),
                std::end(m_arguments),
                [argId_](const ArgumentInfo &ai)->bool
    {
        return ai.getArgId() == argId_;
    });
    if(std::end(m_arguments) != fita)
    {
        fita->setValue(value_);
    }
}

bool QuizImageData::isFromImageIsUrl() const
{
    //QUrl url(m_fromImageUrl);
    //return url.scheme() != "qrc";
    return m_fromImageUrl != g_noImage;
}

bool QuizImageData::isToImageIsUrl() const
{
    //QUrl url(m_toImageUrl);
    //return url.scheme() != "qrc";
    return m_toImageUrl != g_noImage;
}

const QString &QuizImageData::getFromImageUrl() const
{
    return m_fromImageUrl;
}

const QString &QuizImageData::getToImageUrl() const
{
    return m_toImageUrl;
}

const QList<ArgumentInfo> &QuizImageData::getArguments() const
{
    return m_arguments;
}

QList<ArgumentInfo> &QuizImageData::getArgumentsNC()
{
    return m_arguments;
}

const QString &QuizImageData::getVertexShader() const
{
    return m_vertexShader;
}

const QString &QuizImageData::getFragmentShader() const
{
    return m_fragmentShader;
}

void QuizImageData::extractArguments(const Effect *effect_, const EffectArgSet *argumentSet_)
{
    if(nullptr == effect_)
    {
        clearArguments();
        setShaders(QString(), QString());
        initDefaultShaders();
        return;
    }

    // read shaders files
    ArtefactTypeModel *artefactTypeModel = static_cast<ArtefactTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactTypeModel)
                );
    Q_ASSERT(nullptr != artefactTypeModel && artefactTypeModel->sizeImpl() > 0);
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    Q_ASSERT(nullptr != sf);

    setShaders(QString(), QString());

    const EffectObjectsModel *effectObjectsModel = effect_->getEffectObjects();
    Q_ASSERT(nullptr != effectObjectsModel && effectObjectsModel->isListLoaded());

    for(int i = 0; i < effectObjectsModel->sizeImpl(); i++ )
    {
        const EffectObjects *effectObjects = effectObjectsModel->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectObjects);

        const ObjectArtefactModel *effectObjectArtefactModel = effectObjects->getObjectArtefacts();
        Q_ASSERT(nullptr != effectObjectArtefactModel && effectObjectArtefactModel->isListLoadedImpl());

        for(int j = 0; j < effectObjectArtefactModel->sizeImpl(); j++)
        {
            const ObjectArtefact *effectObjectArtefact = effectObjectArtefactModel->dataItemAtImpl(j);
            Q_ASSERT(nullptr != effectObjectArtefact);

            const ArtefactModel * artefactModel = effectObjectArtefact->getArtefact();
            Q_ASSERT(nullptr != artefactModel && artefactModel->isListLoadedImpl());

            for(int k = 0; k < artefactModel->sizeImpl(); k++)
            {
                const Artefact *artefact = artefactModel->dataItemAtImpl(k);
                Q_ASSERT(nullptr != artefact);
                Q_ASSERT(sf->isUrlDownloaded(artefact->filename()));

                ArtefactType *artefactType = artefactTypeModel->findDataItemByIdImpl(artefact->type());
                QString shaderText;
                Q_ASSERT(nullptr != artefactType);

                if(g_artefactTypeVertex == artefactType->type() ||
                        g_artefactTypeFragment == artefactType->type())
                {
                    shaderText = ::loadTextFileByUrl(artefact->filename());
                    if(g_artefactTypeVertex == artefactType->type())
                    {
                        setVertexShader(shaderText);
                    }
                    else if(g_artefactTypeFragment == artefactType->type())
                    {
                        setFragmentShader(shaderText);
                    }
                }
            }
        }
    }

    initDefaultShaders();

    ArtefactArgTypeModel *artefactArgTypeModel = static_cast<ArtefactArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    Q_ASSERT(nullptr != artefactArgTypeModel && artefactArgTypeModel->isListLoaded());

    clearArguments();

    // read default arguments values
    const EffectArgModel *effectArguments = effect_->getEffectArguments();
    Q_ASSERT(nullptr != effectArguments && effectArguments->isListLoaded());
    for(int i = 0; i < effectArguments->sizeImpl(); ++i)
    {
        ArgumentInfo ai;
        const EffectArg *effectArgument = effectArguments->dataItemAtImpl(i);
        Q_ASSERT(nullptr != effectArgument);
        ai.setName(effectArgument->name());
        const ArtefactArgType *argType = artefactArgTypeModel->findDataItemByIdImpl(effectArgument->argTypeId());
        Q_ASSERT(nullptr != argType);
        ai.setArgId(effectArgument->id());
        ai.setType(argType->type());
        ai.setValue(effectArgument->defaultValue());
        appendArguments(ai);
    }

    if(nullptr != argumentSet_)
    {
        const EffectArgValueModel *argumentValuesModel = argumentSet_->getArgumentValues();
        Q_ASSERT(nullptr != argumentValuesModel && argumentValuesModel->isListLoaded());
        for(int i = 0; i < argumentValuesModel->sizeImpl(); ++i)
        {
            const EffectArgValue *effectArgumentValue = argumentValuesModel->dataItemAtImpl(i);
            Q_ASSERT(nullptr != effectArgumentValue);
            const int argId = effectArgumentValue->getArgId();
            setArgumentValue(argId, effectArgumentValue->value());
        }
    }
}
