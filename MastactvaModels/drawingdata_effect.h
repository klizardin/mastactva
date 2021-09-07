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

#ifndef DRAWINGDATA_EFFECT_H
#define DRAWINGDATA_EFFECT_H


#include "../MastactvaModels/effect_data.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"


class DrawingDataEffect :
        public EffectData,
        public IDefaultData<drawing_data::QuizImageObjects>
{
public:
    DrawingDataEffect() = default;
    DrawingDataEffect(EffectData &&data_);

    // DIP initializer
    void init(std::shared_ptr<drawingdata::IFileSource> filesources_);

    // method to form QuizImageObjects data
    void initialize(drawing_data::QuizImageObjects &data_) const override;

private:
    drawingdata::Details m_details;
};


#endif // DRAWINGDATA_EFFECT_H
