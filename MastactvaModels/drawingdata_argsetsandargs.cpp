#include "drawingdata_argsetsandargs.h"
#include "../MastactvaModels/drawingdata_effectargset.h"


void DrawingDataArgSetsAndArgs::setArgSetIndex(int index_)
{
    m_argSetIndex = index_;
}

void DrawingDataArgSetsAndArgs::setObjectLocalPosition(bool local_)
{
    m_addVariableToLocalPosition = local_;
}

bool DrawingDataArgSetsAndArgs::doAddVariableToLocalPosition() const
{
    return m_addVariableToLocalPosition;
}

void DrawingDataArgSetsAndArgs::setObjectArtefactId(int objectArtefactId_)
{
    m_objectArtefactId = objectArtefactId_;
}

void DrawingDataArgSetsAndArgs::clearObjectArtefactId()
{
    m_objectArtefactId = -1;
}

bool DrawingDataArgSetsAndArgs::find(const QString &name_) const
{
    if(!m_effectArgSetsData
            || m_effectArgSetsData->size() >= m_argSetIndex)
    {
        return false;
    }
    EffectArgSetData *argSetData = m_effectArgSetsData->at(m_argSetIndex);
    if(!argSetData)
    {
        return false;
    }
    const auto fit = std::find_if(
                std::begin(*(argSetData->m_effectArgValuesData)),
                std::end(*(argSetData->m_effectArgValuesData)),
                [this, &name_](const EffectArgValueData *argValue_)->bool
    {
        if(!argValue_
                || !argValue_->m_effectArgsData
                || argValue_->m_effectArgsData->size() != 1
                || !argValue_->m_effectArgsData->at(0))
        {
            return false;
        }
        if(argValue_->m_effectArgsData->at(0)->m_objectArtefactId
                != m_objectArtefactId
                )
        {
            return false;
        }
        if(!argValue_->m_effectArgValuesData
                || argValue_->m_effectArgValuesData->size() != 1
                || !argValue_->m_effectArgValuesData->at(0))
        {
            return false;
        }
        return argValue_->m_effectArgValuesData->at(0)->m_name == name_;
    });
    return fit != std::end(*(argSetData->m_effectArgValuesData));
}

QString DrawingDataArgSetsAndArgs::getValue(const QString &name_) const
{
    if(!m_effectArgSetsData
            || m_effectArgSetsData->size() >= m_argSetIndex)
    {
        return QString();
    }
    EffectArgSetData *argSetData = m_effectArgSetsData->at(m_argSetIndex);
    if(!argSetData)
    {
        return QString();
    }
    const auto fit = std::find_if(
                std::begin(*(argSetData->m_effectArgValuesData)),
                std::end(*(argSetData->m_effectArgValuesData)),
                [this, &name_](const EffectArgValueData *argValue_)->bool
    {
        if(!argValue_
                || !argValue_->m_effectArgsData
                || argValue_->m_effectArgsData->size() != 1
                || !argValue_->m_effectArgsData->at(0))
        {
            return false;
        }
        if(argValue_->m_effectArgsData->at(0)->m_objectArtefactId
                != m_objectArtefactId
                )
        {
            return false;
        }
        if(!argValue_->m_effectArgValuesData
                || argValue_->m_effectArgValuesData->size() != 1
                || !argValue_->m_effectArgValuesData->at(0))
        {
            return false;
        }
        return argValue_->m_effectArgValuesData->at(0)->m_name == name_;
    });
    return !(*fit)->m_value.isEmpty()
        ? (*fit)->m_value
        : (*fit)->m_effectArgValuesData->at(0)->m_defaultValue
        ;
}
