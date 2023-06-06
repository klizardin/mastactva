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

#ifndef DRAWINGDATA_OBJECTARTEFACT_H
#define DRAWINGDATA_OBJECTARTEFACT_H


#include "../MastactvaModels/objectartefact_data.h"
#include "../MastactvaModels/drawingdata_artefact.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/drawingdata_utils.h"


/*
 * object artefacts drawing data
 * one artefact object can contain multiple artefacts
*/
class DrawingDataObjectArtefact :
        public ObjectArtefactData,
        public IDrawingDataArtefact
{
public:
    DrawingDataObjectArtefact() = default;
    DrawingDataObjectArtefact(ObjectArtefactData &&data_);

    /*
     * return step index
    */
    int getStepIndex() const;

    /*
     * initialize with first vertex shader from the artefacts the image object
     * return true if any artefact is a vertex shader artefact
    */
    bool setVertexShader(
            drawing_data::QuizImageObject &object_,     // image object
            const drawingdata::Details &details_        // details from parent class
            ) const override;

    /*
     * initialize with first fragment shader from the artetacts the image object
     * return true if any artefact is a fragment shader artefact
    */
    bool setFragmentShader(
            drawing_data::QuizImageObject &object_,     // image object
            const drawingdata::Details &details_        // details from parent class
            ) const override;

    /*
     * add arguments from the artefacts to the image object
    */
    void addArguments(
            drawing_data::QuizImageObject &object_,     // image object
            const drawingdata::Details &details_        // details from parent class
            ) const override;

    /*
     * return true if any arterfact has arguments
    */
    bool hasArguments() const override;

    /*
     * return true if any arterfact has global arguments
    */
    bool hasGlobalArguments() const override;

    /*
     * return true if any arterfact has variables
    */
    bool hasVariables() const override;

    /*
     * add variables of the artefacts to the image object
    */
    void addVariables(
            const drawingdata::Details &details_,        // details from parent class
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr    // arg sets data
            ) const override;

    /*
     * add global arguments of the artefacts to the image object
    */
    void addGlobalArguments(
            drawing_data::QuizImageObjects &data_,      // image object
            const drawingdata::Details &details_        // details from parent class
            ) const override;

    /*
     * add texture from the artefacts to the image object if any artefact is a texture artefact
    */
    void addTexture(
            drawing_data::QuizImageObject &object_
            ) const override;

    /*
     * add drawing data from the artefacta to the image object if any artefact is a artefact of a drawing data
    */
    void addData(
            const drawingdata::Details &details_
            ) const override;

    /*
     * add calculation from the artefacts to the image object if any artefact is a artefact of a calculation data
    */
    bool addCalculations(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;

    /*
     * add main calculation from the artefacts to the image object if any artefact is a artefact of a main calculation data
    */
    bool addMainCalculations(
            drawing_data::QuizImageObjects &objects_,
            const drawingdata::Details &details_
            ) const override;

    /*
     * return true if any artefact has addons
    */
    bool hasAddon() const override;

    /*
     * return addon names
    */
    void getAddonNames(QStringList &names_) const override;

    /*
     * ran specific addon names
    */
    void runAddons(
            const drawingdata::Details &details_,
            const QStringList &addonNames_
            ) const override;

    /*
     * to order this classe in a list by step index value
    */
    friend bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_);

    /*
     * to update step index for variables in the details
    */
    void updateStepIndex(drawingdata::IPosition *position_, bool first_) const;

    /*
     * to check step index for variables in the details
    */
    void checkStepIndex(drawingdata::IPosition *position_) const;

private:
    /*
     * iterator over the artefacts in ther artefact object with specific function
    */
    bool forArtefacts(std::function<bool(const DrawingDataArtefact *)> func_) const;
};

/*
 * to order this classe in a list by step index value
*/
bool operator < (const DrawingDataObjectArtefact &left_, const DrawingDataObjectArtefact &right_);

#endif // DRAWINGDATA_OBJECTARTEFACT_H
