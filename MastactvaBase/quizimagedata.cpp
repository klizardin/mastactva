#include "quizimagedata.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/artefacttype.h"
#include "../MastactvaModels/objectartefact.h"
#include "../MastactvaModels/effect.h"
#include "../MastactvaBase/utils.h"


const QString &ArgumentBase::getName() const
{
    return m_name;
}

void ArgumentBase::setName(const QString &name_)
{
    m_name = name_;
}

const QString &ArgumentBase::getStorage() const
{
    return m_storage;
}

void ArgumentBase::setStorage(const QString &storage_)
{
    m_storage = storage_;
}

const QString &ArgumentBase::getType() const
{
    return m_type;
}

void ArgumentBase::setType(const QString &type_)
{
    m_type = type_;
}

const QString &ArgumentBase::getValue() const
{
    return m_value;
}

void ArgumentBase::setValue(const QString &value_)
{
    m_value = value_;
}

const QString &ArgumentBase::getDefaultValue() const
{
    return m_defaultValue;
}

void ArgumentBase::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;
}

ArgumentValueDataArray *ArgumentBase::createValueDataArray() const
{
    using TypeInfo = std::tuple<const char *, int, bool, bool, bool, int>;
    static const TypeInfo typeInfos[] = {
        { "int", 1, false, false, false, 1 },
        { "float", 1, true, false, false, 1 },
        { "vec2", 2, true, false, false, 2 },
        { "vec3", 3, true, false, false, 3 },
        { "vec4", 4, true, false, false, 4 },
        { "mat2", 4, true, true, false, 4 },
        { "mat3", 9, true, true, false, 9 },
        { "mat4", 16, true, true, false, 16 },
        { "strings", -1, true, true, true, 1 },
    };

    const auto fit = std::find_if(
                std::begin(typeInfos),
                std::end(typeInfos),
                [this] (const TypeInfo &ti_) -> bool
    {
        return std::get<0>(ti_) == getType();
    });

    Q_ASSERT(std::end(typeInfos) != fit);   // unknown type
    if(std::end(typeInfos) == fit) { return nullptr; }

    const int arraySize = std::get<1>(*fit);
    const bool isIntArrayType = !std::get<2>(*fit);
    const bool isFloatArrayType = std::get<2>(*fit);
    const bool isMatrixType = std::get<3>(*fit);
    const bool isStringArrayType = std::get<4>(*fit);
    const int tupleSize = std::get<5>(*fit);

    if(isIntArrayType)
    {
        return new ArgumentValueDataIntArray(*this, arraySize, tupleSize);
    }
    else if(isFloatArrayType)
    {
        return new ArgumentValueDataFloatArray(*this, arraySize, tupleSize, isMatrixType);
    }
    else if(isStringArrayType)
    {
        return new ArgumentValueDataStringArray(*this, arraySize, tupleSize);
    }
    else
    {
        return nullptr;
    }
}


ArgumentValueDataArray::ArgumentValueDataArray(const ArgumentBase &from_, int arraySize_, int tupleSize_)
    : ArgumentBase(from_),
      m_arraySize(arraySize_),
      m_tupleSize(tupleSize_)
{
    initStorage(getStorage());
}

void ArgumentValueDataArray::initData()
{
    if(!getValue().trimmed().isEmpty())
    {
        setArray(getValue());
    }
    else
    {
        setArray(getDefaultValue());
    }
}

int ArgumentValueDataArray::getArraySize() const
{
    return m_arraySize;
}

void ArgumentValueDataArray::initStorage(const QString &storage_)
{
    using StorageInfo = std::tuple<const char *, bool, bool, bool>;
    static const StorageInfo storageInfos[] = {
        { "attribute", true, false, false },
        { "uniform", false, true, false },
        { "indexes", false, false, true },
    };

    const auto fit = std::find_if(
                std::begin(storageInfos),
                std::end(storageInfos),
                [&storage_] (const StorageInfo &si_) -> bool
    {
        return std::get<0>(si_) == storage_;
    });

    Q_ASSERT(std::end(storageInfos) != fit);   // unknown type
    if(std::end(storageInfos) == fit) { return; }

    m_isAttribute = std::get<1>(*fit);
    m_isUniform = std::get<2>(*fit);
    m_isIndex = std::get<3>(*fit);
}


static const char *g_rand = "__rand";


ArgumentValueDataIntArray::ArgumentValueDataIntArray(const ArgumentBase &from_, int arraySize_, int tupleSize_)
    :ArgumentValueDataArray(from_, arraySize_, tupleSize_)
{
}

void ArgumentValueDataIntArray::initData()
{
    if(m_arraySize > 0) { m_values.resize(m_arraySize); }
    ArgumentValueDataArray::initData();
}

void ArgumentValueDataIntArray::setArray(const QVariantList &values_)
{
    if(m_isUniform)
    {
        extractValues(values_, m_values, m_arraySize);
    }
}

OpenGLArgumentValueBase *ArgumentValueDataIntArray::createOpenGlValue()
{
    if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataIntArray>(*this);
    }
    else
    {
        return nullptr; //TODO: implement
    }
}

const QVector<GLint> &ArgumentValueDataIntArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataIntArray::isMatrixType() const
{
    return false;
}

void ArgumentValueDataIntArray::setArray(const QString &value_)
{
    if(m_isUniform)
    {
        if(value_.contains(g_rand))
        {
            QVector<GLint> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformIntRands(args, m_values);
        }
        else
        {
            extractValues(value_, m_values);
        }
    }
}


ArgumentValueDataFloatArray::ArgumentValueDataFloatArray(const ArgumentBase &from_, int arraySize_, int tupleSize_, bool isMatrixType_)
    : ArgumentValueDataArray(from_, arraySize_, tupleSize_),
      m_isMatrixType(isMatrixType_)
{
}

void ArgumentValueDataFloatArray::initData()
{
    if(m_arraySize > 0) { m_values.resize(m_arraySize); }
    ArgumentValueDataArray::initData();
}

void ArgumentValueDataFloatArray::setArray(const QVariantList &values_)
{
    if(m_isUniform)
    {
        extractValues(values_, m_values, m_arraySize);
    }
}

OpenGLArgumentValueBase *ArgumentValueDataFloatArray::createOpenGlValue()
{
    if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataFloatArray>(*this);
    }
    else
    {
        return nullptr; //TODO: implement
    }
}

const QVector<GLfloat> &ArgumentValueDataFloatArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataFloatArray::isMatrixType() const
{
    return m_isMatrixType;
}

void ArgumentValueDataFloatArray::setArray(const QString &value_)
{
    if(m_isUniform)
    {
        if(value_.contains(g_rand))
        {
            QVector<GLfloat> args;
            args.resize(2);
            extractValues(value_, args);
            generateUniformRealRands(args, m_values);
        }
        else
        {
            extractValues(value_, m_values);
        }
    }
}


ArgumentValueDataStringArray::ArgumentValueDataStringArray(const ArgumentBase &from_, int arraySize_, int tupleSize_)
    : ArgumentValueDataArray(from_, arraySize_, tupleSize_)
{
}

void ArgumentValueDataStringArray::initData()
{
    if(m_arraySize > 0) { m_values.resize(m_arraySize); }
    ArgumentValueDataArray::initData();
}

void ArgumentValueDataStringArray::setArray(const QVariantList &values_)
{
    if(m_isUniform)
    {
        extractValues(values_, m_values, m_arraySize);
    }
}

OpenGLArgumentValueBase *ArgumentValueDataStringArray::createOpenGlValue()
{
    if(m_isUniform)
    {
        return new OpenGLArgumentUniformValueT<ArgumentValueDataStringArray>(*this);
    }
    else
    {
        return nullptr; //TODO: implement
    }
}

const QVector<QString> &ArgumentValueDataStringArray::getValues() const
{
    return m_values;
}

bool ArgumentValueDataStringArray::isMatrixType() const
{
    return false;
}

void ArgumentValueDataStringArray::setArray(const QString &value_)
{
    if(m_isUniform)
    {
        Q_ASSERT(m_values.size() <= 1);
        if(m_values.size() < 1) { m_values.resize(1); }
        m_values[0] = value_;
    }
}


void ArgumentValue::setDataArray(ArgumentValueDataArray *argumentValueDataArray_)
{
    m_argumentValueDataArray = argumentValueDataArray_;
}

ArgumentValueDataArray *ArgumentValue::getDataArray() const
{
    return m_argumentValueDataArray;
}

int ArgumentValue::getEffectArgumentId() const
{
    return m_effectArgumentId;
}

void ArgumentValue::setEffectArgumentId(int effectArgumentId_)
{
    m_effectArgumentId = effectArgumentId_;
}


void OpenGLArgumentValueBase::initAttribureValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    m_id = program_->attributeLocation(name_);
    program_->bindAttributeLocation(name_, m_id);
}

void OpenGLArgumentValueBase::initUniformValueId(QOpenGLShaderProgram *program_, const QString &name_)
{
    m_id = program_->uniformLocation(name_);
}

void OpenGLArgumentValueBase::initIndexArray(QOpenGLShaderProgram *program_)
{
}

void OpenGLArgumentValueBase::setAttributeValue(
        QOpenGLShaderProgram *program_,
        GLenum type_,
        int offset_,
        int tupleSize_) const
{
    program_->setAttributeBuffer(m_id, type_, offset_, tupleSize_, 0);
    program_->enableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::releaseAttributeValue(QOpenGLShaderProgram *program_) const
{
    program_->disableAttributeArray(m_id);
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLint> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    Q_UNUSED(isMatrixType);
    if(arraySize_ >= 1 && values_.size() >= 1)
    {
        program_->setUniformValue(m_id, values_[0]);
    }
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<GLfloat> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    if(1 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0]);
    }
    else if(2 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1]);
    }
    else if(3 == arraySize_)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1], values_[2]);
    }
    else if(4 == arraySize_ && !isMatrixType)
    {
        program_->setUniformValue(m_id, values_[0],
                values_[1], values_[2], values_[3]);
    }
    else if(4 == arraySize_ && isMatrixType)
    {
        QMatrix2x2 m(&values_[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(9 == arraySize_ && isMatrixType)
    {
        QMatrix3x3 m(&values_[0]);
        program_->setUniformValue(m_id, m);
    }
    else if(16 == arraySize_ && isMatrixType)
    {
        QMatrix4x4 m(&values_[0]);
        program_->setUniformValue(m_id, m);
    }
    else
    {
        Q_ASSERT(false); // unsupported case
    }
}

void OpenGLArgumentValueBase::setUniformValue(
        QOpenGLShaderProgram *program_,
        const QVector<QString> &values_,
        int arraySize_,
        bool isMatrixType) const
{
    Q_UNUSED(program_);
    Q_UNUSED(values_);
    Q_UNUSED(arraySize_);
    Q_UNUSED(isMatrixType);
    Q_ASSERT(false);
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
