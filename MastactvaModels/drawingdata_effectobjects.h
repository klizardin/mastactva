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
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/utils.h"


class ObjectNamesIndexes
{
public:
    void cleanIndexies();
    int getIndex(const QString &objectName_) const;
    int addIndex(const QString &objectName_);

private:
    QMap<QString, int> m_indexes;
};


class DrawingDataEffectObjects :
        public EffectObjectsData
{
public:
    DrawingDataEffectObjects() = default;
    DrawingDataEffectObjects(EffectObjectsData &&data_);

    /*
     * add objects to the image objects data
    */
    void addObjects(
            drawing_data::QuizImageObjects &data_,      // image objects data
            const drawingdata::Details &details_,       // details from the parent class
            ObjectNamesIndexes &indexes_,
            int stepIndexShift_ = 0,                    // shift index for the objects to add
            const QStringList &addonNames_ = QStringList{}, // addon names to run
            bool mainObjects_ = false                   // is it the main object
            ) const;

    /*
     * add arguments to the detail class
    */
    void addArgs(
            const drawingdata::Details &details_,       // details from the parent class
            DrawingDataArgSetsAndArgs &argSetsAndArgs_, // arg sets
            ObjectNamesIndexes &indexes_,
            int stepIndexShift_ = 0                     // shift of the objects
            ) const;

    /*
     * return addon names
    */
    void getAddonNames(
            QStringList &names_
            ) const;

    /*
     * return true if the object main
    */
    bool isMain() const;

    /*
     * return step index
    */
    int getStepIndex() const;

    /*
     * return programmer name
     * TODO: replace this data function with actions
    */
    QString getProgrammerName() const;

private:
    /*
     * set up position in the details data
    */
    void setupPosition(
            const drawingdata::Details &details_,
            int stepIndexShift_,
            ObjectNamesIndexes &indexes_
            ) const;
};


#endif // DRAWINGDATA_EFFECTOBJECTS_H
