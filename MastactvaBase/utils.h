#ifndef UTILS_H
#define UTILS_H


#include <QString>
#include <QHash>
#include <QDateTime>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <type_traits>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>


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

class WavefrontOBJItem
{
public:
    WavefrontOBJItem() = default;
    void setLine(int line_);
    int getLine() const;
    void setComment(const QString &comment_);
    const QString &getComment() const;

private:
    int m_line = 0;
    QString m_comment;
};

class WavefrontOBJVertex : public WavefrontOBJItem, public QVector4D
{
public:
    WavefrontOBJVertex() = default;
};

class WavefrontOBJVertexTexture : public WavefrontOBJItem, public QVector3D
{
public:
    WavefrontOBJVertexTexture() = default;
};

class WavefrontOBJVertexNormal : public WavefrontOBJItem, public QVector3D
{
public:
    WavefrontOBJVertexNormal() = default;
};

class WavefrontOBJVertexParameter : public WavefrontOBJItem, public QVector3D
{
public:
    WavefrontOBJVertexParameter() = default;
};

class Vector3di
{
public:
    Vector3di() = default;
    Vector3di(int x_, int y_, int z_);
    int x() const;
    void setX(int x_);
    int y() const;
    void setY(int y_);
    int z() const;
    void setZ(int z_);

    int operator[] (std::size_t index_) const;
    int& operator[] (std::size_t index_);

    void mask(const Vector3di &val_);

    bool operator == (const Vector3di &val_) const;
    bool operator < (const Vector3di &val_) const;

private:
    int m_x = 0;
    int m_y = 0;
    int m_z = 0;
};

class WavefrontOBJFaceElement : public WavefrontOBJItem, public QVector<Vector3di>
{
public:
    WavefrontOBJFaceElement() = default;
};

class WavefrontOBJLineElement : public WavefrontOBJItem, public QVector<int>
{
public:
    WavefrontOBJLineElement() = default;
};

class WavefrontOBJObjectName : public WavefrontOBJItem, public QString
{
public:
    WavefrontOBJObjectName() = default;
};

class WavefrontOBJGroupName : public WavefrontOBJItem, public QString
{
public:
    WavefrontOBJGroupName() = default;
};

class WavefrontOBJMaterialLib : public WavefrontOBJItem, public QString
{
public:
    WavefrontOBJMaterialLib() = default;
};

class WavefrontOBJMaterialName : public WavefrontOBJItem, public QString
{
public:
    WavefrontOBJMaterialName() = default;
};

class Bool
{
public:
    Bool() = default;
    bool get();
    void set(bool val_);
    bool val();
    bool& ref();
private:
    bool m_val = false;
};

class WavefrontOBJSmoothing :  public WavefrontOBJItem, public Bool
{
public:
    WavefrontOBJSmoothing() = default;
};


void parseWavefrontOBJLine(const QStringRef &line_, QVector4D &data_);
void parseWavefrontOBJLine(const QStringRef &line_, QVector3D &data_);
void parseWavefrontOBJLine(const QStringRef &line_, QVector<Vector3di> &data_);
void parseWavefrontOBJLine(const QStringRef &line_, QVector<int> &data_);
void parseWavefrontOBJLine(const QStringRef &line_, QString &data_);
void parseWavefrontOBJLine(const QStringRef &line_, Bool &data_);


class WavefrontOBJ
{
public:
    const QVector<WavefrontOBJItem> &getComments() const;
    const QVector<WavefrontOBJVertex> &getVertex() const;
    const QVector<WavefrontOBJVertexTexture> &getVertexTexture() const;
    const QVector<WavefrontOBJVertexNormal> &getNormal() const;
    const QVector<WavefrontOBJVertexParameter> &getVertexParameter() const;
    const QVector<WavefrontOBJFaceElement> &getFaceElements() const;
    const QVector<WavefrontOBJLineElement> &getLineElements() const;
    const QVector<WavefrontOBJObjectName> &getObjectNames() const;
    const QVector<WavefrontOBJGroupName> &getGroupNames() const;
    const QVector<WavefrontOBJSmoothing> &getSmoothing() const;

    bool processLine(const QStringRef &line_, const QStringRef &comment_, int lineNumber_);
    void correct();
    bool validate() const;
    QJsonDocument toJsonData() const;

protected:
    static bool startsWith(const QStringRef &line_, const QString &str_, QStringRef &dataLine_);
    bool hasTextureIndicies() const;
    bool hasNormalIndicies() const;
    void buildObject(int startLineNumber_, int endLineNumber_, const Vector3di &mask_, QJsonObject &obj_) const;
    static QVector<WavefrontOBJFaceElement>::const_iterator lower_bound(
            const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_);
    static QVector<WavefrontOBJFaceElement>::const_iterator upper_bound(
            const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_);

private:
    QVector<WavefrontOBJItem> m_comments;
    QVector<WavefrontOBJVertex> m_vertex;
    QVector<WavefrontOBJVertexTexture> m_vertexTexture;
    QVector<WavefrontOBJVertexNormal> m_normal;
    QVector<WavefrontOBJVertexParameter> m_vertexParameter;
    QVector<WavefrontOBJFaceElement> m_faceElements;
    QVector<WavefrontOBJLineElement> m_lineElements;
    QVector<WavefrontOBJObjectName> m_objectNames;
    QVector<WavefrontOBJGroupName> m_groupNames;
    QVector<WavefrontOBJMaterialLib> m_materialLibs;
    QVector<WavefrontOBJMaterialName> m_materialNames;
    QVector<WavefrontOBJSmoothing> m_smoothing;
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
QByteArray loadBinaryFile(const QString &filename_);
QByteArray loadBinaryFileByUrl(const QString &filenameUrl_, bool useServerFiles_ = true);
QString getTextFromBinaryData(const QByteArray &data_);
QJsonDocument graphicsOBJtoJson(const QString &objData_);
WavefrontOBJ *parseGraphicsOBJ(const QString &objData_);


#if QT_CONFIG(opengl)

bool set_value(const QString &valStr_, GLint& val_);
bool set_value(const QString &valStr_, GLfloat& val_);
void generateUniformRands(const QVector<GLfloat> &args_, QVector<GLfloat> &valuesArray_);
void generateUniformRands(const QVector<GLint> &args_, QVector<GLint> &valuesArray_);

#endif  // #if QT_CONFIG(opengl)


template<typename Type_>
void extractValues(const QVariantList &values_, QVector<Type_> &valuesArray_, int arraySize_)
{
    int pos = 0;
    for(const QVariant &val_ : qAsConst(values_))
    {
        QString val = val_.toString().trimmed();
        if(val.isEmpty()) { continue; }
        if(arraySize_ >= 0 && pos >= valuesArray_.size())
        {
            continue;
        }
        Type_ value = Type_();
        if(set_value(val, value))
        {
            if(arraySize_ < 0 && pos >= valuesArray_.size())
            {
                valuesArray_.resize(pos+1);
            }
            valuesArray_[pos] = value;
            ++pos;
        }
    }
}

template<typename Type_>
void extractValues(const QString &valuesStr_, QVector<Type_> &valuesArray_, int arraySize_)
{
    QString value = valuesStr_;
    value.replace(QString("("), QString(", "));
    value.replace(QString(")"), QString(", "));
    value.replace(QString("{"), QString(", "));
    value.replace(QString("}"), QString(", "));
    value.replace(QString("\n"), QString(", "));
    QStringList values = value.split(QString(","));
    QVariantList valuesVar;
    valuesVar.reserve(values.size());
    for(const QString &s : qAsConst(values))
    {
        valuesVar.push_back(s);
    }
    extractValues(valuesVar, valuesArray_, arraySize_);
}


template <typename EnumType_>
constexpr auto to_underlying(EnumType_ enumValue_) noexcept
{
    return static_cast<std::underlying_type_t<EnumType_>>(enumValue_);
}

static const char *g_englishLanguage = "English";
static const char *g_belarusLanguage = "Belarus";
static const char *g_shaderName = "shader";
static const char *g_argumentName = "argument";
static const char *g_storageName = "storage";
static const char *g_precisionName = "precision";
static const char *g_typeName = "type";
static const char *g_nameName = "name";
static const char *g_defaultValueName = "defaultValue";
static const char *g_descriptionName = "description";
static const char *g_geometrySolidName = "geomentrySolid";
static const char *g_geometryFacedName = "geomentryFaced";
static const char *g_geometrySizeName = "geomentrySize";
static const char *g_facedGeometryCoefName = "facedGeometryCoef";
static const char *g_colorsAttributeName = "attributeColors";
static const char *g_artefactsRelPath = "artefacts";
static const char *g_imagesRelPath = "images";
static const char *g_artefactTypeVertex = "Vertex";
static const char *g_artefactTypeFragment = "Fragment";
static const char *g_artefactTypeModel = "ArtefactTypeModel";
static const char *g_artefactArgTypeModel = "ArtefactArgTypeModel";
static const char *g_artefactArgStorageModel = "ArtefactArgStorageModel";
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
static const char *g_noImageQRC = "qrc:///resources/no-image.png";
static const char *g_createDefaultEffectObjectProcedureName = "create_default_object";
static const char *g_effectIDName = "id";
static const char *g_defaultObjectInfoNameName = "name";
static const char *g_defaultObjectInfoProgrammerNameName = "programmer_name";
static const char *g_defaultObjectInfoDescriptionName = "description";
static const char *g_defaultObjectInfoCreatedName = "created";
static const char *g_defaultObjectInfoProgrammerName = "";
static const char *g_randArgumentValueName = "__rand";
static const char *g_currentSpecialwordName = "__current";


#endif // UTILS_H
