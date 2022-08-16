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

#ifndef DRAWINGDATAARGSETSANDARGS_H
#define DRAWINGDATAARGSETSANDARGS_H


#include <memory>
#include <QVector>


class EffectArgData;
class EffectArgSetData;
class EffectArgValueData;


/*
 * the helper class for algorithm of processing effect data to the effect objects
 * the holder class to pass drawing data arg sets and arg data to the functions
*/
class DrawingDataArgSetsAndArgs
{
public:
    std::shared_ptr<QVector<EffectArgData *>> m_effectArgsData;
    std::shared_ptr<QVector<EffectArgSetData *>> m_effectArgSetsData;

    // main interface
    // find arguemnt by name for the current object in the arg sets
    // function is not const because it sets value if it has found any
    bool find(const QString &name_);
    // return value of founded argument
    QString getValue() const;

    // choose current arg set
    void setArgSetIndex(int index_);
    // set object local position
    // TODO: why we need object local position flag
    void setObjectLocalPosition(bool local_);
    // is local objct position set
    bool doAddVariableToLocalPosition() const;
    // set object artefcat id
    void setObjectArtefactId(int objectArtefactId_);
    // clear object artefact id
    void clearObjectArtefactId();

private:
    int m_argSetIndex = 0;
    bool m_addVariableToLocalPosition = false;
    int m_objectArtefactId = -1;
    const EffectArgValueData *m_value = nullptr;
};


#endif // DRAWINGDATAARGSETSANDARGS_H
