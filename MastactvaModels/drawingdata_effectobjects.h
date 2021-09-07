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

#ifndef DRAWINGDATA_EFFECTOBJECTS_H
#define DRAWINGDATA_EFFECTOBJECTS_H


#include <memory>
#include "../MastactvaModels/effectobjects_data.h"
#include "../MastactvaBase/utils.h"


class DrawingDataEffectObjects :
        public EffectObjectsData
{
public:
    DrawingDataEffectObjects() = default;
    DrawingDataEffectObjects(EffectObjectsData &&data_);

    void addObjects(
            drawing_data::QuizImageObjects &data_,
            const drawingdata::Details &details_,
            int stepIndexShift_ = 0
            ) const;
    bool isMain() const;
    int getStepIndex() const;
    QString getProgrammerName() const;

private:
    void setupPosition(const drawingdata::Details &details_, int stepIndexShift_) const;
};


#endif // DRAWINGDATA_EFFECTOBJECTS_H
