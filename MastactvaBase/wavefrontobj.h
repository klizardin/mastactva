#ifndef WAVEFRONTOBJ_H
#define WAVEFRONTOBJ_H


#include <QString>
#include <QVector>
#include <QVector4D>
#include <QVector3D>
#include <QVector2D>
//#include <QStringRef>
#include <QJsonDocument>
#include "../MastactvaBase/utils.h"


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


class WavefrontOBJSmoothing :  public WavefrontOBJItem, public Bool
{
public:
    WavefrontOBJSmoothing() = default;
};


void parseWavefrontOBJLine(const QString &line_, QVector4D &data_);
void parseWavefrontOBJLine(const QString &line_, QVector3D &data_);
void parseWavefrontOBJLine(const QString &line_, QVector<Vector3di> &data_);
void parseWavefrontOBJLine(const QString &line_, QVector<int> &data_);
void parseWavefrontOBJLine(const QString &line_, QString &data_);
void parseWavefrontOBJLine(const QString &line_, Bool &data_);


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

    bool processLine(const QString &line_, const QString &comment_, int lineNumber_);
    void correct();
    bool validate() const;
    QJsonDocument toJsonData() const;

    static QJsonDocument graphicsOBJtoJson(const QString &objData_);
    static std::unique_ptr<WavefrontOBJ> parseGraphicsOBJ(const QString &objData_);

private:
    static bool startsWith(const QString &line_, const QString &str_, QString &dataLine_);
    bool hasTextureIndicies() const;
    bool hasNormalIndicies() const;
    bool buildObject(
            int startLineNumber_, int endLineNumber_,
            const Vector3di &mask_,
            QJsonArray &vertex_,
            QJsonArray &normal_,
            QJsonArray &textures_
            ) const;
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


#endif // WAVEFRONTOBJ_H
