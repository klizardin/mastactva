#include "drawingdata_utils.h"
#include <QJsonObject>
#include <QDebug>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"
#include "../MastactvaBase/utils.h"


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
            value(result.objectName) = objectNameJV.toString();
            has_value(result.objectName) = true;
        }
    }
    if(position_.contains(g_jsonDataVariableObjectStepIndexName))
    {
        const QJsonValue objectStepIndexJV = position_[g_jsonDataVariableObjectStepIndexName];
        if(!objectStepIndexJV.isUndefined()
                && objectStepIndexJV.isDouble())
        {
            value(result.objectStepIndex) = static_cast<int>(objectStepIndexJV.toDouble());
            has_value(result.objectStepIndex) = true;
        }
    }
    if(position_.contains(g_jsonDataVariableArtefactStepIndexName))
    {
        const QJsonValue artefactStepIndexJV = position_[g_jsonDataVariableArtefactStepIndexName];
        if(!artefactStepIndexJV.isUndefined()
                && artefactStepIndexJV.isDouble())
        {
            value(result.artefactStepIndex) = static_cast<int>(artefactStepIndexJV.toDouble());
            has_value(result.artefactStepIndex) = true;
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

    value(result.objectName) = position_->getObjectName();
    has_value(result.objectName) = true;
    value(result.objectStepIndex) = position_->getObjectStepIndex();
    has_value(result.objectStepIndex) = true;
    value(result.artefactStepIndex) = position_->getArtefactStepIndex();
    has_value(result.artefactStepIndex) = true;

    return result;
}

bool operator == (const VariablePosition &left_, const VariablePosition &right_)
{
    bool objectsEqual = true;
    if(has_value(left_.objectName) && has_value(right_.objectName))
    {
        objectsEqual = value(left_.objectName) == value(right_.objectName);
    }
    bool objectIndexesEqual = true;
    if(has_value(left_.objectStepIndex) && has_value(right_.objectStepIndex))
    {
        objectIndexesEqual = value(left_.objectStepIndex) == value(right_.objectStepIndex);
    }
    bool artefactIndexesEqual = true;
    if(has_value(left_.artefactStepIndex) && has_value(right_.artefactStepIndex))
    {
        artefactIndexesEqual = value(left_.artefactStepIndex) == value(right_.artefactStepIndex);
    }
    return objectsEqual && objectIndexesEqual && artefactIndexesEqual;
}


void ValiableData::set(const QJsonArray &jsonArray_)
{
    m_jsonArray = jsonArray_;
    m_floatData.clear();
    m_intData.clear();
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

void ValiableData::prepare(QVector<float> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_floatData);
}

void ValiableData::prepare(QVector<int> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_intData);
}

void ValiableData::get(QVector<float> &data_) const
{
    data_.clear();
    data_.reserve(m_floatData.size());
    std::copy(std::begin(m_floatData), std::end(m_floatData),
              std::back_inserter(data_));
}

void ValiableData::get(QVector<int> &data_) const
{
    data_.clear();
    data_.reserve(m_intData.size());
    std::copy(std::begin(m_intData), std::end(m_intData),
              std::back_inserter(data_));
}


void Variable::set(const QJsonArray &jsonArray_)
{
    if(m_data.operator bool())
    {
        m_data->set(jsonArray_);
    }
}

void Variable::setAlias(const Variable &var_)
{
    m_data = var_.m_data;
}

void Variable::setPosition(const QJsonObject &position_)
{
    m_position = VariablePosition::fromJson(position_);
}

void Variable::prepare(QVector<float> &data_) const
{
    if(m_data.operator bool())
    {
        m_data->prepare(data_);
    }
}

void Variable::prepare(QVector<int> &data_) const
{
    if(m_data.operator bool())
    {
        m_data->prepare(data_);
    }
}

void Variable::get(QVector<float> &data_) const
{
    if(m_data.operator bool())
    {
        m_data->get(data_);
    }
}

void Variable::get(QVector<int> &data_) const
{
    if(m_data.operator bool())
    {
        m_data->get(data_);
    }
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
    fit->second.prepare(data_);
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
    fit->second.prepare(data_);
    fit->second.get(data_);
    return true;
}

std::tuple<QJsonValue, bool> Variables::getJsonValue(const QJsonObject &varObject)
{
    if(!varObject.contains(g_jsonDataVariableValueName))
    {
        return {{}, false};
    }
    QJsonValue val = varObject[g_jsonDataVariableValueName];
    if(val.isUndefined()
            || !val.isArray())
    {
        return {{}, false};
    }
    return {val, true};
}

void Variables::addVariables(
        const QJsonObject &rootObject_,
        const std::tuple<details::Variable, bool> &dataSource_
        )
{
    const QStringList keys = rootObject_.keys();
    for(const QString &key_ : keys)
    {
        const QJsonValue var = rootObject_[key_];
        if(var.isUndefined()
                || !var.isObject())
        {
            continue;
        }
        const QJsonObject varObject = var.toObject();

        const auto jsonVal = has_value(dataSource_)
                ? std::make_tuple(QJsonValue{}, true)
                : getJsonValue(varObject)
                  ;
        if(!std::get<bool>(jsonVal))
        {
            continue;
        }

        if(key_ == g_jsonDataVariableNameObjectListName)
        {
            if(has_value(dataSource_))
            {
                // can't alias objectslist
                continue;
            }

            setObjectsList(value(jsonVal).toArray());
        }
        else
        {
            details::Variable newVar;
            if(has_value(dataSource_))
            {
                newVar.setAlias(value(dataSource_));
            }
            else
            {
                newVar.set(value(jsonVal).toArray());
            }

            const QJsonValue position = varObject[g_jsonDataVariablePositionName];
            if(!position.isUndefined()
                    && position.isObject())
            {
                newVar.setPosition(position.toObject());
            }

            details::VariableName variableName(key_, index);
            Q_ASSERT(index < std::numeric_limits<decltype (index)>::max());
            ++index;
            m_variables.insert({std::move(variableName), std::move(newVar)});
            // TODO: add remove unreachable variables
        }
    }
}

void Variables::add(const QJsonDocument &data_)
{
    if(!data_.isObject())
    {
        return;
    }
    QJsonObject rootObject = data_.object();
    addVariables(rootObject, {details::Variable{}, false});
}

void Variables::addAliases(const QJsonDocument &data_, const IPosition *position_)
{
    Q_UNUSED(data_);
    Q_UNUSED(position_);
}

bool Variables::getObjectsList(QStringList &objects_) const
{
    if(!m_hasObjectsList)
    {
        return false;
    }
    objects_ = m_objects;
    return true;
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

void Variables::setObjectsList(const QJsonArray &array_)
{
    m_objects.clear();
    m_objects.reserve(array_.size());
    for(int i = 0; i < array_.size(); ++i)
    {
        const QJsonValue v = array_.at(i);
        if(v.isUndefined()
                || !v.isString())
        {
            continue;
        }
        m_objects.push_back(v.toString());
    }
    m_hasObjectsList = true;
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
