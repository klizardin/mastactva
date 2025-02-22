#ifndef UTILS_OPENGL_H
#define UTILS_OPENGL_H


#include <type_traits>
#include <vector>
#include <memory>
#include <QString>
#include <QHash>
#include <QDateTime>
#include <QByteArray>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLShader>


/*
 * open gl help functions
*/
bool set_value(const QString &valStr_, GLint& val_);    // set value to GLint from the string
bool set_value(const QString &valStr_, GLfloat& val_);  // set value to GLfloat from the string
bool set_value(const QString &valStr_, QString& val_);  // set value to QString from the string
void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_);   // generate random values
void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_);       // generate random values
QStringList getOpenGLErrors();  // return open gl error as list of strings


#endif // UTILS_OPENGL_H
