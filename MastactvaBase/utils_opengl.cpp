#include "utils_opengl.h"
#include <random>
#include <QStringList>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QTimeZone>
#include <QTextStream>
#include <QDebug>


bool set_value(const QString &valStr_, GLint& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toInt(&ok);
    return ok;
}

bool set_value(const QString &valStr_, GLfloat& val_)
{
    bool ok = false;
    val_ = QVariant::fromValue(valStr_).toDouble(&ok);
    return ok;
}

bool set_value(const QString &valStr_, QString& val_)
{
    val_ = valStr_;
    return true;
}

void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}

void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_)
{
    if(args_.size() < 2) { return; }
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(args_[0], args_[1]);
    for(int i = 0; i < valuesArray_.size(); i++)
    {
        valuesArray_[i] = dis(gen);
    }
}


QStringList getOpenGLErrors()
{
    QStringList res;
    using ErrorInfoType = std::tuple<GLenum, const char*>;
    static const ErrorInfoType errors[] = {
        {GL_NO_ERROR, "GL_NO_ERROR"},
        {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
        {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
        {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
        {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"},
        {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    };

    GLenum error = GL_NO_ERROR;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
        {
            const auto fit = std::find_if(
                        std::begin(errors),
                        std::end(errors),
                        [&error](const ErrorInfoType &errorInfo_)->bool
            {
               return error == std::get<0>(errorInfo_);
            });
            if(std::end(errors) != fit)
            {
                res.append(std::get<1>(*fit));
            }
        }
    } while (error != GL_NO_ERROR);
    return res;
}
