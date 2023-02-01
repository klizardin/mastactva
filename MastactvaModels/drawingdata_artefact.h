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

#ifndef DRAWINGDATA_ARTEFACT_H
#define DRAWINGDATA_ARTEFACT_H


#include "../MastactvaModels/artefact_data.h"
#include "../MastactvaModels/drawingdata_argsetsandargs.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "../MastactvaBase/drawingdata_utils.h"


/*
 * interface for drawing data artefact
*/
class IDrawingDataArtefact
{
public:
    virtual ~IDrawingDataArtefact() = default;

    /*
     * set vertex shader to the object if the artefact is vertex shader
    */
    virtual bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * set fragment shader to the object if the artefact is fragment shader
    */
    virtual bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * add arguments of the artefact if any
     * add values of arguments to the object values
     * if there is no variable for this argument
    */
    virtual void addArguments(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * add texture targets
    */
    virtual void addTetureTargets(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * return true if the artefact has arguments
    */
    virtual bool hasArguments() const = 0;

    /*
     * return true if the artefact has global arguments
     * arguments that is used for global drawing object states
    */
    virtual bool hasGlobalArguments() const = 0;

    /*
     * return true if the artefact has variables
     * variables are the values that sets not to the object but to the
     * variables. variables can be used by scripts by arguments
     * argument of shader is the default value that uses if there is no
     * variable value for this argument
     * lua scripts setup variables and use variables
    */
    virtual bool hasVariables() const = 0;

    /*
     * add variables if there is any
    */
    virtual void addVariables(
            const drawingdata::Details &details_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const = 0;

    /*
     * add global arguments
     * (special setup) can be specified just for main objects
    */
    virtual void addGlobalArguments(
            drawing_data::QuizImageObjects &data_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * add texture if the artefact is a texture
    */
    virtual void addTexture(
            drawing_data::QuizImageObject &object_
            ) const = 0;

    /*
     * add some type of data to the drawing object
     * if the artefact is a some kind of data
    */
    virtual void addData(
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * add calculations if the artefact is a calculation
    */
    virtual bool addCalculations(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * add calculations if the artefact is a calculation
    */
    virtual bool addMainCalculations(
            drawing_data::QuizImageObjects &objects_,
            const drawingdata::Details &details_
            ) const = 0;

    /*
     * do artefact is an addon (GUI addons by IEffectAddOn interface)
    */
    virtual bool hasAddon() const = 0;

    /*
     * return addon's names
    */
    virtual void getAddonNames(QStringList &names_) const = 0;

    /*
     * run GUI addons
    */
    virtual void runAddons(
            const drawingdata::Details &details_,
            const QStringList &addonNames_
            ) const = 0;
};


/*
 * implementation of the interface IDrawingDataArtefact for the data ArtefactData
*/
class DrawingDataArtefact :
        public ArtefactData,
        public IDrawingDataArtefact
{
public:
    DrawingDataArtefact() = default;
    DrawingDataArtefact(ArtefactData &&data_);

    bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addArguments(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addTetureTargets(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    bool hasArguments() const override;
    bool hasGlobalArguments() const override;
    bool hasVariables() const override;
    void addVariables(
            const drawingdata::Details &details_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const override;
    void addGlobalArguments(
            drawing_data::QuizImageObjects &data_,
            const drawingdata::Details &details_
            ) const override;
    void addTexture(
            drawing_data::QuizImageObject &object_
            ) const override;
    void addData(
            const drawingdata::Details &details_
            ) const override;
    bool addCalculations(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    bool addMainCalculations(
            drawing_data::QuizImageObjects &objects_,
            const drawingdata::Details &details_
            ) const override;
    bool hasAddon() const override;
    void getAddonNames(QStringList &names_) const override;
    void runAddons(
            const drawingdata::Details &details_,
            const QStringList &addonNames_
            ) const override;

    std::shared_ptr<QVector<EffectArgumentData *>> m_effectArgData;
};


#endif // DRAWINGDATA_ARTEFACT_H
