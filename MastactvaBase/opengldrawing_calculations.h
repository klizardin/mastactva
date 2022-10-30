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

#ifndef OPENGLDRAWING_CALCULATIONS_H
#define OPENGLDRAWING_CALCULATIONS_H


#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


/*
 * open gl drawing calculation (runtime)
 * and helper classes for calculations
*/
namespace opengl_drawing
{
    class Objects;

/*
 * simple walk effect matrix calculations
*/
class WalkEffectRectMatrixCalculation
        : public IEffectCalculation
{
public:
    WalkEffectRectMatrixCalculation();
    ~WalkEffectRectMatrixCalculation() override;

    bool init(const QString &args_);

    void calculate(IVariables *variables_) const override;

private:
    QString m_vertexAttributeName;
    QString m_renderImageName;
    QString m_rectMatrixUniformName;
    int m_rows = 2;
    int m_cols = 2;
};


/*
 * simple walk effect clip rect calculations
*/
class WalkEffectClipRectCalculation
        : public IEffectCalculation
{
public:
    WalkEffectClipRectCalculation();
    ~WalkEffectClipRectCalculation() override;

    bool init(const QString &args_);

    void calculate(IVariables *variables_) const override;

private:
    QVector4D getImageClipRect(
            opengl_drawing::Objects *objects_,
            const QString &imageName_,
            const QSize &windowSize_
            ) const;
    QVector4D minClipRect(const QVector4D &cr1_, const QVector4D &cr2_) const;
private:
    QString m_renderFromImageName;
    QString m_renderToImageName;
};


/*
 * image geometry matrix calculations
*/
class ImagesGeometryMatrixCalculation
        : public IEffectCalculation
{
public:
    ImagesGeometryMatrixCalculation();
    ~ImagesGeometryMatrixCalculation() override;

    bool init(const QString &args_);

    void calculate(IVariables *variables_) const override;

private:
    QVector<QRectF> getImageRects(
            opengl_drawing::Objects *objects_,
            const QStringList &imageNames_,
            const QSize &windowSize_
            ) const;
    QMatrix4x4 getGeometryMatrix(
            opengl_drawing::Objects *objects_,
            const QSize &windowSize_,
            const QVector<QRectF> &imagesRects_
            ) const;

private:
    QStringList m_renderImages;
};


/*
 * base class for a factory of calculations
*/
class IEffectCalculationFactory
{
public:
    /*
     * return calculation if there is any
    */
    virtual std::unique_ptr<IEffectCalculation> get(const QString &effectCalculationName_) const = 0;

protected:
    /*
     * extract calculation function name and its arguments
    */
    static QStringList splitOnNameAndArgs(const QString &effectCalculationName_)
    {
        QStringList result;
        result.reserve(2);
        const int fb = effectCalculationName_.indexOf(g_startArguments);
        if(fb > 0)
        {
            // name has arguments
            result.push_back(effectCalculationName_.mid(0, fb));
            result.push_back(effectCalculationName_.mid(fb+1));
        }
        else
        {
            // name is without arguments
            result.push_back(effectCalculationName_);
        }
        return result;
    }

    /*
     * impelmentation of the get functions of this interface
    */
    template<typename EffectCalculationClass_>
    std::unique_ptr<IEffectCalculation> getImpl(const QString &effectCalculationName_) const
    {
        static EffectCalculationClass_ effectCalcTempl;
        const QStringList args = splitOnNameAndArgs(effectCalculationName_);
        if(args.size() < 1)
        {
            // no name
            return nullptr;
        }
        // base name check
        if(effectCalcTempl.getFilename() != args[0])
        {
            // it is not the current calculation to use
            return nullptr;
        }
        // create object of calculation
        // not it can have more detailed name
        std::unique_ptr<EffectCalculationClass_> result = std::make_unique<EffectCalculationClass_>();
        if(args.size() > 1)
        {
            // if we have arguments process them
            QString effectCalculationArgs = args[1].trimmed();
            if(effectCalculationArgs.length() > 0
                    && effectCalculationArgs.at(effectCalculationArgs.length() - 1) == QChar(g_endArguments[0]))
            {
                effectCalculationArgs = effectCalculationArgs.mid(0, effectCalculationArgs.length() - 1);
            }
            if(!result->init(effectCalculationArgs))
            {
                // if it is failed to init exit
                return nullptr;
            }
        }
        return result;
    }
};


/*
 * factory template of base interfaces from the arguments list
*/
template<class ... EffectCalculationClasses_>
class EffectCalculationFactoryTmpl;


template<class EffectCalculationClass_, class ... EffectCalculationClasses_>
class EffectCalculationFactoryTmpl<EffectCalculationClass_, EffectCalculationClasses_ ...>
        : virtual public IEffectCalculationFactory
        , public EffectCalculationFactoryTmpl<EffectCalculationClasses_ ...>
{
public:
    std::unique_ptr<IEffectCalculation> get(const QString &effectCalculationName_) const override
    {
        std::unique_ptr<IEffectCalculation> result = IEffectCalculationFactory::getImpl<EffectCalculationClass_>(
                    effectCalculationName_
                    );
        if(result)
        {
            return result;
        }
        return EffectCalculationFactoryTmpl<EffectCalculationClasses_ ...>::get(effectCalculationName_);
    }
};


template<class EffectCalculationClass_>
class EffectCalculationFactoryTmpl<EffectCalculationClass_>
        : virtual public IEffectCalculationFactory
{
public:
    std::unique_ptr<IEffectCalculation> get(const QString &effectCalculationName_) const override
    {
        return IEffectCalculationFactory::getImpl<EffectCalculationClass_>(effectCalculationName_);
    }
};


/*
 * factory impl
*/
using EffectCalculationFactory = EffectCalculationFactoryTmpl<
    WalkEffectRectMatrixCalculation,
    WalkEffectClipRectCalculation,
    ImagesGeometryMatrixCalculation
    >;


/*
 * helper function to get factory impl
*/
inline
const opengl_drawing::EffectCalculationFactory& getEffectCalculationFactory()
{
    static opengl_drawing::EffectCalculationFactory factory;
    return factory;
}


} // namespace opengl_drawing


#endif // OPENGLDRAWING_CALCULATIONS_H
