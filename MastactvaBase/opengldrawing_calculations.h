#ifndef OPENGLDRAWING_CALCULATIONS_H
#define OPENGLDRAWING_CALCULATIONS_H


#include "../MastactvaBase/opengldrawing_utils.h"


namespace opengl_drawing
{


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
};


} // namespace opengl_drawing


#endif // OPENGLDRAWING_CALCULATIONS_H
