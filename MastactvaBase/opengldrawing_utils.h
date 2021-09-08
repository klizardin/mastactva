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


#include <QString>
#include <QVector>
#include <QStringList>
#include <QSet>


namespace opengl_drawing
{

class IVariables
{
public:
    virtual ~IVariables() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;
    virtual void set(const QString &name_, const QVector<double> &data_) = 0;
    virtual void set(const QString &name_, QVector<double> &&data_) = 0;
    virtual bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const = 0;
};

class VariablesExtended : public IVariables
{
public:
    VariablesExtended(IVariables *base_, IVariables *extend_);

    bool get(const QString &name_, QVector<double> &data_) const override;
    void set(const QString &name_, const QVector<double> &data_) override;
    void set(const QString &name_, QVector<double> &&data_) override;
    bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override;

private:
    IVariables *m_base = nullptr;
    IVariables *m_extend = nullptr;
};


// stateless (except this members)
class IEffectCalculation
{
public:
    virtual ~IEffectCalculation() = default;
    virtual void calculate(IVariables *variables_) const = 0;

    const QString &getFilename() const;
    bool doExtend(IEffectCalculation *calculation_) const;
    const QSet<QString> &getRequiredVariables() const;

protected:
    void setFilename(const QString &filename_);
    void setRequiredVariables(const QStringList &vars_);

private:
    QString m_filename;
    QSet<QString> m_vars;
};

}


#endif // OPENGLDRAWING_UTILS_H
