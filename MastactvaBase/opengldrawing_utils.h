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

#ifndef OPENGLDRAWING_UTILS_H
#define OPENGLDRAWING_UTILS_H


#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/QOpenGLFramebufferObject>
#include <vector>
#include <QString>
#include <QVector>
#include <QStringList>
#include <QSet>


/*
 * namespace for drawing utils under opengl
*/
namespace opengl_drawing
{

/*
 * IVariables interface for the drawing data objects under opengl
*/
class IVariables
{
public:
    virtual ~IVariables() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;   // return the current value of the variable as the vector of double
    virtual void set(const QString &name_, const QVector<double> &data_) = 0;   // set the current variable value as the vector of double
    virtual void set(const QString &name_, QVector<double> &&data_) = 0;        // set the current variable value as the vector of double (move semantic)
    virtual bool get(const QString &name_, QStringList &data_) const = 0;       // get the current variable value as the vector of strings
    virtual void set(const QString &name_, const QStringList &data_) = 0;       // set the current variable value as the vector of strings
    virtual void set(const QString &name_, QStringList &&data_) = 0;            // set the current variable value as the vector of strings (move sematics)
    virtual bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const = 0;    // is the any of variables updated
    virtual const IVariables * getRoot() const;                                 // return const root of the variables tree
    virtual IVariables * getRoot();                                             // return root of the variables tree
                                                                                // variables can be for all objects - root variable
                                                                                // and for a concrete object - leaf variables
};

/*
 * helper interface to deal with root variables correctly
*/
class VariablesExtended : public IVariables
{
public:
    /*
     * first return base thah extend pointer
     * extend is the root pointer
    */
    VariablesExtended(IVariables *base_, IVariables *extend_);

    bool get(const QString &name_, QVector<double> &data_) const override;
    void set(const QString &name_, const QVector<double> &data_) override;
    void set(const QString &name_, QVector<double> &&data_) override;
    bool get(const QString &name_, QStringList &data_) const override;
    void set(const QString &name_, const QStringList &data_) override;
    void set(const QString &name_, QStringList &&data_) override;
    bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override;

    const IVariables * getRoot() const override;
    IVariables * getRoot() override;
private:
    IVariables *m_base = nullptr;
    IVariables *m_extend = nullptr;
};


/*
 * helper class for the base of calculations
 * should be stateless (except this members TODO: why)
 * as the calculation class should not to change itself after creation
*/
class IEffectCalculation
{
public:
    virtual ~IEffectCalculation() = default;
    virtual void calculate(IVariables *variables_) const = 0;

    const QString &getFilename() const;                             // filename to detect calculation
    bool doExtend(const IEffectCalculation *calculation_) const;    // if this calculation do extend some another
    const QSet<QString> &getRequiredVariables() const;              // list of required variables if any was changed
                                                                    // then process we need to process calculate()
    bool doNeedUpdate() const;                                      // if calculation should be called anywhere
    void setUpdated();                                              // set updated
    void clearUpdated();                                            // clear updated (after this calculation will be called anywhere)

protected:
    void setFilename(const QString &filename_);
    void setRequiredVariables(const QStringList &vars_);

private:
    QString m_filename;
    QSet<QString> m_vars;
    bool m_needUpdate = true;
};

/*
 * function to create default geometry for image swap effects
*/
/* float width_, float height_, */
void makeGeometry(
        int geomertyPointsWidth_, int geometryPointsHeight_,
        float facedGeometryXCoef_, float facedGeometryYCoef_,
        int geometryVertexCoords_, int geometryTextureCoords_,
        bool hasTextureCoords_,
        bool isGeometrySolid_,
        std::vector<GLfloat> &vertexData_,
        std::vector<GLfloat> &textureData_
        );

/*
 * is not needed now
*/
QMatrix4x4 calculatePreserveAspectFitTextureMatrix(
        const QSize &imageSize_,
        const QSize &rectSize_,
        bool inverse_ = false
        );
/*
 * is not needed now
*/
QMatrix4x4 calculateTransfromMatrixBy4Points(
        const QVector<QVector2D> &srcPts_,
        const QVector<QVector2D> &destPts_
        );


}


#endif // OPENGLDRAWING_UTILS_H
