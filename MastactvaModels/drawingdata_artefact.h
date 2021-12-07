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


class IDrawingDataArtefact
{
public:
    virtual ~IDrawingDataArtefact() = default;
    virtual bool setVertexShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual bool setFragmentShader(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addArguments(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual bool hasArguments() const = 0;
    virtual bool hasVariables() const = 0;
    virtual void addVariables(
            const drawingdata::Details &details_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const = 0;
    virtual void addTexture(
            drawing_data::QuizImageObject &object_
            ) const = 0;
    virtual void addData(
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addCalculations(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual void addMainCalculations(
            drawing_data::QuizImageObjects &objects_,
            const drawingdata::Details &details_
            ) const = 0;
    virtual bool hasAddon() const = 0;
    virtual void getAddonNames(QStringList &names_) const = 0;
    virtual void runAddons(
            const drawingdata::Details &details_,
            const QStringList &addonNames_
            ) const = 0;
};


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
    bool hasArguments() const override;
    bool hasVariables() const override;
    void addVariables(
            const drawingdata::Details &details_,
            DrawingDataArgSetsAndArgs *argSetsAndArgs_ = nullptr
            ) const override;
    void addTexture(
            drawing_data::QuizImageObject &object_
            ) const override;
    void addData(
            const drawingdata::Details &details_
            ) const override;
    void addCalculations(
            drawing_data::QuizImageObject &object_,
            const drawingdata::Details &details_
            ) const override;
    void addMainCalculations(
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
