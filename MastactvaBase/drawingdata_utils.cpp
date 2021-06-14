#include "drawingdata_utils.h"
#include <QJsonObject>
#include <QDebug>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


namespace drawingdata
{

namespace details
{

VariablePosition VariablePosition::fromJson(const QJsonObject &position_)
{
    VariablePosition result;
    if(position_.isEmpty())
    {
        return result;
    }
    if(position_.contains(g_jsonDataVariableObjectNameName))
    {
        const QJsonValue objectNameJV = position_[g_jsonDataVariableObjectNameName];
        if(!objectNameJV.isUndefined()
                && objectNameJV.isString())
        {
            result.objectName = objectNameJV.toString();
            result.hasObjectName = true;
        }
    }
    if(position_.contains(g_jsonDataVariableObjectStepIndexName))
    {
        const QJsonValue objectStepIndexJV = position_[g_jsonDataVariableObjectStepIndexName];
        if(!objectStepIndexJV.isUndefined()
                && objectStepIndexJV.isDouble())
        {
            result.objectStepIndex = static_cast<int>(objectStepIndexJV.toDouble());
            result.hasObjectStepIndex = true;
        }
    }
    if(position_.contains(g_jsonDataVariableArtefactStepIndexName))
    {
        const QJsonValue artefactStepIndexJV = position_[g_jsonDataVariableArtefactStepIndexName];
        if(!artefactStepIndexJV.isUndefined()
                && artefactStepIndexJV.isDouble())
        {
            result.artefactStepIndex = static_cast<int>(artefactStepIndexJV.toDouble());
            result.hasArtefactStepIndex = true;
        }
    }
    return result;
}

VariablePosition VariablePosition::fromCurrent(const IPosition *position_)
{
    VariablePosition result;
    if(nullptr == position_)
    {
        return result;
    }

    result.hasObjectName = true;
    result.objectName = position_->getObjectName();
    result.hasObjectStepIndex = true;
    result.objectStepIndex = position_->getObjectStepIndex();
    result.hasArtefactStepIndex = true;
    result.artefactStepIndex = position_->getArtefactStepIndex();

    return result;
}

bool operator == (const VariablePosition &left_, const VariablePosition &right_)
{
    bool objectsEqual = true;
    if(left_.hasObjectName && right_.hasObjectName)
    {
        objectsEqual = left_.objectName == right_.objectName;
    }
    bool objectIndexesEqual = true;
    if(left_.hasObjectStepIndex && right_.hasObjectStepIndex)
    {
        objectIndexesEqual = left_.objectStepIndex == right_.objectStepIndex;
    }
    bool artefactIndexesEqual = true;
    if(left_.hasArtefactStepIndex && right_.hasArtefactStepIndex)
    {
        artefactIndexesEqual = left_.artefactStepIndex == right_.artefactStepIndex;
    }
    return objectsEqual && objectIndexesEqual && artefactIndexesEqual;
}


void Variable::set(const QJsonArray &jsonArray_)
{
    m_jsonArray = jsonArray_;
    m_floatData.clear();
    m_intData.clear();
}

void Variable::setPosition(const QJsonObject &position_)
{
    m_position = VariablePosition::fromJson(position_);
}

template<typename Type_> static inline
void prepareDataFromJsonArray(const QJsonArray &jsonArray_, QVector<Type_> &data_)
{
    if(!data_.empty())
    {
        return;
    }

    data_.reserve(jsonArray_.size());
    for(int i = 0; i < jsonArray_.size(); ++i)
    {
        const QJsonValue v = jsonArray_.at(i);
        if(v.isUndefined() || !v.isDouble())
        {
            continue;
        }
        data_.push_back(static_cast<Type_>(v.toDouble()));
    }
}

void Variable::prepare(QVector<float> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_floatData);
}

void Variable::prepare(QVector<int> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_intData);
}

void Variable::get(QVector<float> &data_) const
{
    data_.clear();
    data_.reserve(m_floatData.size());
    std::copy(std::begin(m_floatData), std::end(m_floatData),
              std::back_inserter(data_));
}

void Variable::get(QVector<int> &data_) const
{
    data_.clear();
    data_.reserve(m_intData.size());
    std::copy(std::begin(m_intData), std::end(m_intData),
              std::back_inserter(data_));
}

bool Variable::match(const VariablePosition &pos_) const
{
    return m_position == pos_;
}


VariableName::VariableName(const QString &name_ /*= QString()*/,int index_ /*= 0*/, bool hasIndex_ /*= true*/)
    : name(name_),
      index(index_),
      hasIndex(hasIndex_)
{
}

bool operator == (const VariableName &left_, const VariableName &right_)
{
    return left_.name == right_.name && (left_.hasIndex && right_.hasIndex && left_.index == right_.index);
}

bool operator < (const VariableName &left_, const VariableName &right_)
{
    const int nameCompareResult = left_.name.compare(right_.name);
    if(0 != nameCompareResult)
    {
        return nameCompareResult < 0;
    }
    else if(left_.hasIndex && right_.hasIndex)
    {
        return left_.index < right_.index;
    }
    else
    {
        return false;
    }
}

}


bool Variables::get(const QString &name_, const IPosition *position_, QVector<int> &data_) const
{
    VariablesMap::const_iterator fit = std::cend(m_variables);
    if(!find(name_, position_, fit) || std::cend(m_variables) == fit)
    {
        return false;
    }
    const_cast<details::Variable &>(fit->second).prepare(data_);
    fit->second.get(data_);
    return true;
}

bool Variables::get(const QString &name_, const IPosition *position_, QVector<float> &data_) const
{
    VariablesMap::const_iterator fit = std::cend(m_variables);
    if(!find(name_, position_, fit) || std::cend(m_variables) == fit)
    {
        return false;
    }
    const_cast<details::Variable &>(fit->second).prepare(data_);
    fit->second.get(data_);
    return true;
}

void Variables::add(const QJsonDocument &data_)
{
    if(!data_.isObject())
    {
        return;
    }
    QJsonObject rootObject = data_.object();
    const QStringList keys = rootObject.keys();
    for(const QString &key_ : keys)
    {
        const QJsonValue var = rootObject[key_];
        if(var.isUndefined()
                || !var.isObject())
        {
            continue;
        }
        const QJsonObject varObject = var.toObject();
        if(!varObject.contains(g_jsonDataVariableValueName))
        {
            continue;
        }
        const QJsonValue val = varObject[g_jsonDataVariableValueName];
        if(val.isUndefined()
                || !val.isArray())
        {
            continue;
        }
        details::Variable newVar;
        newVar.set(val.toArray());

        const QJsonValue position = varObject[g_jsonDataVariablePositionName];
        if(!position.isUndefined()
                && position.isObject())
        {
            newVar.setPosition(position.toObject());
        }

        details::VariableName variableName(key_, index);
        Q_ASSERT(index < std::numeric_limits<decltype (index)>::max());
        ++index;
        m_variables.insert({variableName, std::move(newVar)});
        // TODO: add remove unreachable variables
    }
}

void Variables::clear()
{
    m_variables.clear();
    index = std::numeric_limits<decltype (index)>::min();
}

bool Variables::find(const QString &name_, const IPosition *position_, VariablesMap::const_iterator &fit) const
{
    if(m_variables.empty())
    {
        return false;
    }
    details::VariableName variableName(name_, 0, false);
    fit = m_variables.upper_bound(variableName);
    if(std::begin(m_variables) == fit)
    {
        return false;
    }
    details::VariablePosition currentPos = details::VariablePosition::fromCurrent(position_);
    for(--fit; name_ == fit->first.name; --fit)
    {
        if(fit->second.match(currentPos))
        {
            return true;
        }
        if(std::begin(m_variables) == fit)
        {
            return false;
        }
    }
    return false;
}


void Position::setObject(const QString &name_, int objectStepIndex_)
{
    objectName = name_;
    objectStepIndex = objectStepIndex_;
    artefactStepIndex = std::numeric_limits<decltype (artefactStepIndex)>::max();
#if defined(TRACE_EFFECT_OBJECT_POSITION)
    qDebug() << objectName << objectStepIndex << artefactStepIndex;
#endif
}

void Position::resetArtefactStepIndex(int stepIndex_)
{
    artefactStepIndex = stepIndex_;
#if defined(TRACE_EFFECT_OBJECT_POSITION)
    qDebug() << objectName << objectStepIndex << artefactStepIndex;
#endif
}

void Position::setArtefactStepIndex(int stepIndex_)
{
    Q_ASSERT(artefactStepIndex <= stepIndex_);
    artefactStepIndex = stepIndex_;
#if defined(TRACE_EFFECT_OBJECT_POSITION)
    qDebug() << objectName << objectStepIndex << artefactStepIndex;
#endif
}

const QString &Position::getObjectName() const
{
    return objectName;
}

int Position::getObjectStepIndex() const
{
    return objectStepIndex;
}

int Position::getArtefactStepIndex() const
{
    return artefactStepIndex;
}

void Position::clear()
{
    objectName.clear();
    std::numeric_limits<decltype (objectStepIndex)>::max();
    artefactStepIndex = std::numeric_limits<decltype (artefactStepIndex)>::max();
}


Details::Details()
{
    variables = std::make_shared<Variables>();
    position = std::make_shared<Position>();
}

void Details::clear()
{
    if(variables.operator bool())
    {
        variables->clear();
    }
    if(position.operator bool())
    {
        position->clear();
    }
}

}
