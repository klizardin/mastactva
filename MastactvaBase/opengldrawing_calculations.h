#ifndef OPENGLDRAWING_CALCULATIONS_H
#define OPENGLDRAWING_CALCULATIONS_H


#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


namespace opengl_drawing
{
    class Objects;

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


class IEffectCalculationFactory
{
public:
    virtual std::unique_ptr<IEffectCalculation> get(const QString &effectCalculationName_) const = 0;

protected:
    static QStringList splitOnNameAndArgs(const QString &effectCalculationName_)
    {
        QStringList result;
        result.reserve(2);
        const int fb = effectCalculationName_.indexOf(g_startArguments);
        if(fb > 0)
        {
            result.push_back(effectCalculationName_.mid(0, fb));
            result.push_back(effectCalculationName_.mid(fb+1));
        }
        else
        {
            result.push_back(effectCalculationName_);
        }
        return result;
    }

    template<typename EffectCalculationClass_>
    std::unique_ptr<IEffectCalculation> getImpl(const QString &effectCalculationName_) const
    {
        static EffectCalculationClass_ effectCalcTempl;
        const QStringList args = splitOnNameAndArgs(effectCalculationName_);
        if(args.size() < 1)
        {
            return nullptr;
        }
        if(effectCalcTempl.getFilename() != args[0])
        {
            return nullptr;
        }
        std::unique_ptr<EffectCalculationClass_> result = std::make_unique<EffectCalculationClass_>();
        if(args.size() > 1)
        {
            QString effectCalculationArgs = args[1].trimmed();
            if(effectCalculationArgs.length() > 0
                    && effectCalculationArgs.at(effectCalculationArgs.length() - 1) == g_endArguments)
            {
                effectCalculationArgs = effectCalculationArgs.mid(0, effectCalculationArgs.length() - 1);
            }
            if(!result->init(effectCalculationArgs))
            {
                return nullptr;
            }
        }
        return result;
    }
};


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


using EffectCalculationFactory = EffectCalculationFactoryTmpl<
    WalkEffectRectMatrixCalculation,
    WalkEffectClipRectCalculation,
    ImagesGeometryMatrixCalculation
    >;


inline
const opengl_drawing::EffectCalculationFactory& getEffectCalculationFactory()
{
    static opengl_drawing::EffectCalculationFactory factory;
    return factory;
}


} // namespace opengl_drawing


#endif // OPENGLDRAWING_CALCULATIONS_H
