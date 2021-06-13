#include "drawingdata_utils.h"
#include <QJsonObject>
#include "../MastactvaBase/names.h"


namespace drawingdata
{

namespace details
{

void Variable::set(const QJsonArray &jsonArray_)
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

bool Variables::find(const QString &name_, VariablesMap::const_iterator &fit) const
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
    --fit;
    if(name_ != fit->first.name)
    {
        return false;
    }
    return true;
}

bool Variables::get(const QString &name_, QVector<int> &data_) const
{
    VariablesMap::const_iterator fit = std::cend(m_variables);
    if(!find(name_, fit) || std::cend(m_variables) == fit)
    {
        return false;
    }
    const_cast<details::Variable &>(fit->second).prepare(data_);
    fit->second.get(data_);
    return true;
}

bool Variables::get(const QString &name_, QVector<float> &data_) const
{
    VariablesMap::const_iterator fit = std::cend(m_variables);
    if(!find(name_, fit) || std::cend(m_variables) == fit)
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
        details::VariableName variableName(key_, index);
        ++index;
        m_variables.insert({variableName, std::move(newVar)});
        // TODO: add remove unreachable variables
    }
}

Details::Details()
{
    variables = std::make_shared<Variables>();
}

}
