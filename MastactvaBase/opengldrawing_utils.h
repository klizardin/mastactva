#ifndef OPENGLDRAWING_UTILS_H
#define OPENGLDRAWING_UTILS_H


#include <QString>
#include <QVector>
#include <QStringList>


namespace opengl_drawing
{

class IVariables
{
public:
    virtual ~IVariables() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;
    virtual void set(const QString &name_, const QVector<double> &data_) = 0;
    virtual void set(const QString &name_, QVector<double> &&data_) = 0;
    virtual bool isUpdated(const QStringList &vars_) const = 0;
};

class VariablesExtended : public IVariables
{
public:
    VariablesExtended(IVariables *base_, IVariables *extend_);

    bool get(const QString &name_, QVector<double> &data_) const override;
    void set(const QString &name_, const QVector<double> &data_) override;
    void set(const QString &name_, QVector<double> &&data_) override;
    bool isUpdated(const QStringList &vars_) const override;

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
    const QStringList &getRequiredVariables() const;

protected:
    void setFilename(const QString &filename_);
    void setRequiredVariables(const QStringList &vars_);

private:
    QString m_filename;
    QStringList m_vars;
};

}


#endif // OPENGLDRAWING_UTILS_H
