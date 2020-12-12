#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QHash>
#include <QDateTime>


class Comment
{
public:
    Comment(int cb_ = -1, int ce_ = -1,int lb_ = -1, int le_ = -1);
    bool isAlignedToLeft(const QString &shaderText_) const;
    void findLeftLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);
    void findRightLine(const QVector<int> &indexesOfNL_, const QString &shaderText_);
    void extractValues(const QString &shaderText_);
    void extractLineValues(const QString &shaderText_);
    void extractArgumentsLineValues(const QString &shaderText_);
    const QHash<QString, QString> &values() const;

private:
    int cb = 0;
    int ce = 0;
    int lb = 0;
    int le = 0;
    QHash<QString, QString> m_values;
};


QString leftDoubleCR(const QString &str_);
qreal sqr(qreal val_);
bool isSymbol(const QChar &ch_);
void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_);
QString calculateFileURLHash(const QString &fileUrl_);
QDateTime nowTz();
QDateTime dateTimeFromJsonString(const QString& dateTimeZ_);
QString dateTimeToJsonString(const QDateTime &dt_);


static const QString g_englishLanguage = "English";
static const QString g_belarusLanguage = "Belarus";
static const QString g_shaderName = "shader";
static const QString g_argumentName = "argument";
static const QString g_typeName = "type";
static const QString g_nameName = "name";
static const QString g_defaultValueName = "defaultvalue";
static const QString g_descriptionName = "description";
static const QString g_shadersRelPath = "shaders";
static const QString g_imagesRelPath = "images";
static const QString g_shaderTypeVertex = "Vertex";
static const QString g_shaderTypeFragmet = "Fragmet";
static const QString g_shaderTypeModel = "ShaderTypeModel";
static const QString g_shaderArgTypeModel = "ShaderArgTypeModel";
static const QString g_easingTypeModel = "EasingTypeModel";
static const QString g_galleryModel = "GalleryModel";
static const QString g_allImagesOfGalleryModel = "AllImagesOfGallery";


#endif // UTILS_H
