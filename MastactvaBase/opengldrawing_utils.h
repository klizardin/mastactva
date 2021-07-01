#ifndef OPENGLDRAWING_UTILS_H
#define OPENGLDRAWING_UTILS_H


#include <QString>
#include <QVector>


namespace opengl_drawing
{

class IVariables
{
public:
    virtual ~IVariables() = default;
    virtual bool get(const QString &name_, QVector<double> &data_) const = 0;
    virtual void set(const QString &name_, const QVector<double> &data_) = 0;
    virtual void set(const QString &name_, QVector<double> &&data_) = 0;
};

}


#endif // OPENGLDRAWING_UTILS_H
