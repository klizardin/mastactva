#include "drawingdata_utils.h"
#include <QJsonObject>


namespace drawingdata
{

void Variables::Variable::set(const QJsonArray &jsonArray_)
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

void Variables::Variable::prepare(QVector<float> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_floatData);
}

void Variables::Variable::prepare(QVector<int> &)
{
    prepareDataFromJsonArray(m_jsonArray, m_intData);
}

void Variables::Variable::get(QVector<float> &data_) const
{
    data_.clear();
    data_.reserve(m_floatData.size());
    std::copy(std::begin(m_floatData), std::end(m_floatData),
              std::back_inserter(data_));
}

void Variables::Variable::get(QVector<int> &data_) const
{
    data_.clear();
    data_.reserve(m_intData.size());
    std::copy(std::begin(m_intData), std::end(m_intData),
              std::back_inserter(data_));
}

bool Variables::get(const QString &name_, QVector<int> &data_) const
{
    auto fit = m_variables.find(name_);
    if(std::end(m_variables) == fit)
    {
        return false;
    }
    const_cast<Variable &>(fit->second).prepare(data_);
    fit->second.get(data_);
    return true;
}

bool Variables::get(const QString &name_, QVector<float> &data_) const
{
    auto fit = m_variables.find(name_);
    if(std::end(m_variables) == fit)
    {
        return false;
    }
    const_cast<Variable &>(fit->second).prepare(data_);
    fit->second.get(data_);
    return true;
}

static const char * g_jsonDataVariableValueName = "value";

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
        Variable newVar;
        newVar.set(val.toArray());
        m_variables.insert({key_, std::move(newVar)});
    }
}

Details::Details()
{
    variables = std::make_shared<Variables>();
}

}
