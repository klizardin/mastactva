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
qreal cube(qreal val_);
bool isSpace(const QChar &ch_);
bool isSymbol(const QChar &ch_);
bool isLetterNumeric(const QChar &ch_);
bool isNumeric(const QChar &ch_);
bool isNumeric(const QString &str_);
void getShaderComments(const QString &shaderText_, QVector<Comment> &comments_);
QString calculateHash(const QString &data_);
QString calculateFileURLHash(const QString &fileUrl_);
QDateTime nowTz();
QDateTime dateTimeFromJsonString(const QString& dateTimeZ_);
QString dateTimeToJsonString(const QDateTime &dt_);
QString loadTextFile(const QString &filename_);
QString loadTextFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);


static const char *g_englishLanguage = "English";
static const char *g_belarusLanguage = "Belarus";
static const char *g_shaderName = "shader";
static const char *g_argumentName = "argument";
static const char *g_typeName = "type";
static const char *g_nameName = "name";
static const char *g_defaultValueName = "defaultValue";
static const char *g_descriptionName = "description";
static const char *g_geometrySolidName = "geomentrySolid";
static const char *g_geometryFacedName = "geomentryFaced";
static const char *g_geometrySizeName = "geomentrySize";
static const char *g_facedGeometryCoefName = "facedGeometryCoef";
static const char *g_colorsAttributeName = "attributeColors";
static const char *g_shadersRelPath = "shaders";
static const char *g_imagesRelPath = "images";
static const char *g_shaderTypeVertex = "Vertex";
static const char *g_shaderTypeFragment = "Fragment";
static const char *g_shaderTypeModel = "ShaderTypeModel";
static const char *g_shaderArgTypeModel = "ShaderArgTypeModel";
static const char *g_easingTypeModel = "EasingTypeModel";
static const char *g_galleryModel = "GalleryModel";
static const char *g_allImagesOfGalleryModel = "AllImagesOfGallery";
static const char *g_sqlText = "TEXT";
static const char *g_sqlInt = "INTEGER";
static const char *g_sqlDouble = "REAL";
static const char *g_dbNameRW = "mastactva_rw";
static const char *g_dbNameRO = "mastactva_ro";
static const char *g_dbNameExt = ".db3";
static const char *g_splitTableRef = "_by_";
static const char *g_insertFieldSpliter = " , ";
static const char *g_refPrefix = "ref_";
static const char *g_errorDetailTag = "detail";
static const int g_userId = 1;
static const char *g_noImage = ":/resources/no-image.png";


#endif // UTILS_H
