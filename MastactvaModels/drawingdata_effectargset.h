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

#ifndef DRAWINGDATA_EFFECTARGSET_H
#define DRAWINGDATA_EFFECTARGSET_H


#include "../MastactvaModels/effectargset_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaModels/drawingdata_effectarg.h"


/*
 * it is used only as data container
*/
class DrawingDataEffectArgSet :
        public EffectArgSetData
{
public:
    DrawingDataEffectArgSet() = default;
    DrawingDataEffectArgSet(EffectArgSetData &&data_);
};


#endif // DRAWINGDATA_EFFECTARGSET_H
