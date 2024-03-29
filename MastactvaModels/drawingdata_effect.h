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
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/drawingdata_utils.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/addon.h"


class DrawingDataEffectObjects;


/*
 * implementation of drawing effect data that extend effect data.
 * implement interfaces quiz image object initializer, effect GUI addon
*/
class DrawingDataEffect :
        public EffectData,
        public IDefaultData<drawing_data::QuizImageObjects>,
        public IEffectAddOn
{
public:
    DrawingDataEffect() = default;
    DrawingDataEffect(EffectData &&data_);

    // DIP initializer
    // {
    /*
     * set up IFileSource dependency
    */
    void init(std::shared_ptr<drawingdata::IFileSource> filesources_);

    /*
     * set up IAddonModules dependency
    */
    void init(std::shared_ptr<IAddonModules> modules_);
    // }

    // IDefaultData<drawing_data::QuizImageObjects>
    // {
    // method to form QuizImageObjects data
    void initialize(drawing_data::QuizImageObjects &data_, int argsSetIndex_ = 0) const override;
    // }

    // IEffectAddOn
    // {
    void getAddonNames(QStringList &names_) const override;
    void runAddons(const QStringList &names_) const override;
    // }

private:
    using SortedEffectObjects = std::multimap<int, const DrawingDataEffectObjects *>;
    using SortedByProgrammerNameEffectObjects = std::multimap<QString, const DrawingDataEffectObjects *>;

private:
    void extractMainObjects(
            SortedEffectObjects &sortedEffectObjects_,
            SortedEffectObjects &sortedMainEffectObjects_,
            SortedByProgrammerNameEffectObjects &sortedByProgrammerNameEffectObjects_
            ) const;
    void runObjects(
            drawing_data::QuizImageObjects &data_,
            SortedEffectObjects &objects_,
            bool mainObjects_ = false
            ) const;
    void addArguments(
            DrawingDataArgSetsAndArgs &argSetsAndArgs_,
            SortedEffectObjects &objects_,
            bool local_
            ) const;
    void processByObjectListOrder(
            drawing_data::QuizImageObjects &data_,
            const QStringList &objectsToRun_,
            const SortedByProgrammerNameEffectObjects &sortedByProgrammerNameEffectObjects_
            ) const;
    void processByDefaultOrder(
            drawing_data::QuizImageObjects &data_,
            const SortedEffectObjects &sortedEffectObjects_
            ) const;
    void getAddonNames(
            SortedEffectObjects &objects_,
            QStringList &names_
            ) const;
    void runObjectsAndAddons(
            drawing_data::QuizImageObjects &data_,
            SortedEffectObjects &objects_,
            const QStringList &names_
            ) const;

private:
    drawingdata::Details m_details;
};


#endif // DRAWINGDATA_EFFECT_H
