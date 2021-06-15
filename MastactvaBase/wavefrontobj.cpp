#include "wavefrontobj.h"
#include <set>
#include <QJsonArray>
#include <QJsonObject>
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


static QString removeComment(const QString &str_, QString &comment_)
{
    const int cs = str_.indexOf(QChar('#'));
    if(cs >= 0)
    {
        comment_ = str_.mid(cs);
        return str_.mid(0, cs);
    }
    else
    {
        return  str_;
    }
}

void WavefrontOBJItem::setLine(int line_)
{
    m_line = line_;
}

int WavefrontOBJItem::getLine() const
{
    return m_line;
}

void WavefrontOBJItem::setComment(const QString &comment_)
{
    m_comment = comment_;
}

const QString &WavefrontOBJItem::getComment() const
{
    return m_comment;
}


Vector3di::Vector3di(int x_, int y_, int z_)
    :m_x(x_), m_y(y_), m_z(z_)
{
}

int Vector3di::x() const
{
    return m_x;
}

void Vector3di::setX(int x_)
{
    m_x = x_;
}

int Vector3di::y() const
{
    return m_y;
}

void Vector3di::setY(int y_)
{
    m_y = y_;
}

int Vector3di::z() const
{
    return m_z;
}

void Vector3di::setZ(int z_)
{
    m_z = z_;
}

int Vector3di::operator[] (std::size_t index_) const
{
    switch(index_)
    {
    case 0:
        return m_x;
    case 1:
        return m_y;
    case 2:
        return m_z;
    default:
        return 0;
    }
}

int& Vector3di::operator[] (std::size_t index_)
{
    static int fish;
    switch(index_)
    {
    case 0:
        return m_x;
    case 1:
        return m_y;
    case 2:
        return m_z;
    default:
        return fish;
    }
}

void Vector3di::mask(const Vector3di &val_)
{
    for(std::size_t i = 0; i < 3; i++)
    {
        if(val_[i] < 0) { (*this)[i] = -1; }
    }
}

bool Vector3di::operator == (const Vector3di &val_) const
{
    return (*this)[0] == val_[0] &&
            (*this)[1] == val_[1] &&
            (*this)[2] == val_[2]
            ;
}

bool Vector3di::operator < (const Vector3di &val_) const
{
    if((*this)[0] < val_[0]) { return true; }
    if((*this)[0] > val_[0]) { return false; }
    if((*this)[1] < val_[1]) { return true; }
    if((*this)[1] > val_[1]) { return false; }
    return (*this)[2] < val_[2];
}

void parseWavefrontOBJLine(const QString &line_, QVector4D &data_)
{
    QStringList values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    data_.setZ(1.0f);
    std::size_t j = 0;
    for(const QString &val_: values)
    {
        bool ok = false;
        double vald = QVariant::fromValue(val_).toDouble(&ok);
        if(ok)
        {
            data_[j] = vald;
            j++;
        }
    }
}

void parseWavefrontOBJLine(const QString &line_, QVector3D &data_)
{
    QStringList values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    std::size_t j = 0;
    for(const QString &val_: values)
    {
        bool ok = false;
        double vald = QVariant::fromValue(val_).toDouble(&ok);
        if(ok)
        {
            data_[j] = vald;
            j++;
        }
    }
}

void parseWavefrontOBJLine(const QString &line_, QVector<Vector3di> &data_)
{
    QStringList values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    for(const QString &val_: values)
    {
        QStringList faceItems = val_.split(QChar('/'), Qt::SkipEmptyParts);
        std::size_t j = 0;
        Vector3di val;
        for(const QString &fval_: faceItems)
        {
            bool ok = false;
            int vali = QVariant::fromValue(fval_).toInt(&ok);
            if(ok)
            {
                val[j] = vali;
                j++;
            }
        }
        data_.push_back(val);
    }
}

void parseWavefrontOBJLine(const QString &line_, QVector<int> &data_)
{
    QStringList values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    for(const QString &val_: values)
    {
        bool ok = false;
        int vali = QVariant::fromValue(val_).toInt(&ok);
        if(ok)
        {
            data_.push_back(vali);
        }
    }
}

void parseWavefrontOBJLine(const QString &line_, QString &data_)
{
    data_ = line_;
}

void parseWavefrontOBJLine(const QString &line_, Bool &data_)
{
    bool ok = false;
    const int iv = line_.toInt(&ok);
    if(ok)
    {
        data_.set(iv != 0);
        return;
    }
    const bool fv = line_.indexOf("off", 0, Qt::CaseInsensitive) >= 0;
    if(fv)
    {
        data_.set(false);
        return;
    }
    const bool tv = line_.indexOf("on", 0, Qt::CaseInsensitive) >= 0;
    if(tv)
    {
        data_.set(true);
        return;
    }
    const bool bfv = line_.indexOf("false", 0, Qt::CaseInsensitive) >= 0;
    if(bfv)
    {
        data_.set(false);
        return;
    }
    const bool btv = line_.indexOf("true", 0, Qt::CaseInsensitive) >= 0;
    if(btv)
    {
        data_.set(true);
        return;
    }
    data_.set(false);
}


template<typename WavefrontOBJType_>
void initWavefrontOBJItem(
        WavefrontOBJType_ &d_,
        const QString &dataLine_,
        const QString &comment_,
        int lineNumber_,
        QVector<WavefrontOBJType_> &vec_
        )
{
    d_.setComment(comment_);
    d_.setLine(lineNumber_);
    parseWavefrontOBJLine(dataLine_, d_);
    vec_.push_back(d_);
}

bool WavefrontOBJ::processLine(const QString &line_, const QString &comment_, int lineNumber_)
{
    QString dataLine;
    if(startsWith(line_, "v ", dataLine))
    {
        WavefrontOBJVertex v;
        initWavefrontOBJItem(v, dataLine, comment_, lineNumber_, m_vertex);
        return true;
    }
    else if(startsWith(line_, "vt ", dataLine))
    {
        WavefrontOBJVertexTexture vt;
        initWavefrontOBJItem(vt, dataLine, comment_, lineNumber_, m_vertexTexture);
        return true;
    }
    else if(startsWith(line_, "vn ", dataLine))
    {
        WavefrontOBJVertexNormal vn;
        initWavefrontOBJItem(vn, dataLine, comment_, lineNumber_, m_normal);
        return true;
    }
    else if(startsWith(line_, "vp ", dataLine))
    {
        WavefrontOBJVertexParameter vp;
        initWavefrontOBJItem(vp, dataLine, comment_, lineNumber_, m_vertexParameter);
        return true;
    }
    else if(startsWith(line_, "f ", dataLine))
    {
        WavefrontOBJFaceElement f;
        initWavefrontOBJItem(f, dataLine, comment_, lineNumber_, m_faceElements);
        return true;
    }
    else if(startsWith(line_, "l ", dataLine))
    {
        WavefrontOBJLineElement l;
        initWavefrontOBJItem(l, dataLine, comment_, lineNumber_, m_lineElements);
        return true;
    }
    else if(startsWith(line_, "o ", dataLine))
    {
        WavefrontOBJObjectName o;
        initWavefrontOBJItem(o, dataLine, comment_, lineNumber_, m_objectNames);
        return true;
    }
    else if(startsWith(line_, "g ", dataLine))
    {
        WavefrontOBJGroupName g;
        initWavefrontOBJItem(g, dataLine, comment_, lineNumber_, m_groupNames);
        return true;
    }
    else if(startsWith(line_, "mtllib ", dataLine))
    {
        WavefrontOBJMaterialLib mtllib;
        initWavefrontOBJItem(mtllib, dataLine, comment_, lineNumber_, m_materialLibs);
        return true;
    }
    else if(startsWith(line_, "usemtl ", dataLine))
    {
        WavefrontOBJMaterialName usemtl;
        initWavefrontOBJItem(usemtl, dataLine, comment_, lineNumber_, m_materialNames);
        return true;
    }
    else if(startsWith(line_, "s ", dataLine))
    {
        WavefrontOBJSmoothing s;
        initWavefrontOBJItem(s, dataLine, comment_, lineNumber_, m_smoothing);
        return true;
    }
    else if(line_.trimmed().isEmpty() && !comment_.isEmpty())
    {
        // whole line comment
        WavefrontOBJItem c;
        c.setComment(comment_);
        c.setLine(lineNumber_);
        m_comments.push_back(c);
        return true;
    }
    return false;
}

void WavefrontOBJ::correct()
{
    for(WavefrontOBJFaceElement &fItems_ : m_faceElements)
    {
        for(Vector3di &f_: static_cast<QVector<Vector3di>&>(fItems_))
        {
            if(f_.x() < 0)
            {
                f_.setX(m_vertex.size() - f_.x());
            }
            else
            {
                f_.setX(f_.x() - 1);
            }

            if(f_.y() < 0)
            {
                f_.setY(m_vertex.size() - f_.y());
            }
            else
            {
                f_.setY(f_.y() - 1);
            }

            if(f_.z() < 0)
            {
                f_.setZ(m_vertex.size() - f_.z());
            }
            else
            {
                f_.setZ(f_.z() - 1);
            }
        }
    }
    for(WavefrontOBJLineElement &lItems_ : m_lineElements)
    {
        for(int &l_: static_cast<QVector<int>&>(lItems_))
        {
            if(l_ < 0)
            {
                l_ = m_vertex.size() - l_;
            }
            else
            {
                --l_;
            }
        }
    }
}

bool WavefrontOBJ::validate() const
{
    const bool fInRange = std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(static_cast<const QVector<Vector3di>&>(fItems_)),
                std::end(static_cast<const QVector<Vector3di>&>(fItems_)),
                [this](const Vector3di &f_)->bool
        {
            return (f_.x() >= 0 && f_.x() < m_vertex.size()) &&
                ((f_.y() >= -1 && f_.y() < m_vertexTexture.size())) &&
                ((f_.z() >= -1 && f_.z() < m_normal.size()))
            ;
        });
    });
    if(!fInRange) { return false; }

    const bool lInRange = std::all_of(
                std::begin(m_lineElements),
                std::end(m_lineElements),
                [this](const WavefrontOBJLineElement &lItem_)->bool
    {
        return std::all_of(
                std::begin(static_cast<const QVector<int>&>(lItem_)),
                std::end(static_cast<const QVector<int>&>(lItem_)),
                [this](const int &l_)->bool
        {
            return l_ >= 0 && l_ < m_vertex.size();
        });
    });
    if(!lInRange) { return false; }

    return true;
}

bool WavefrontOBJ::startsWith(const QString &line_, const QString &str_, QString &dataLine_)
{
    if(line_.startsWith(str_))
    {
        dataLine_ = line_.mid(str_.length());
        return true;
    }
    else
    {
        dataLine_.clear();
        return false;
    }
}

bool WavefrontOBJ::hasTextureIndicies() const
{
    return std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(fItems_),
                std::end(fItems_),
                [this](const Vector3di &f_)->bool
        {
            return f_.y() >= 0 && f_.y() < m_vertexTexture.size();
        });
    });
}

bool WavefrontOBJ::hasNormalIndicies() const
{
    return std::all_of(
                std::begin(m_faceElements),
                std::end(m_faceElements),
                [this](const WavefrontOBJFaceElement &fItems_)->bool
    {
        return std::all_of(
                std::begin(fItems_),
                std::end(fItems_),
                [this](const Vector3di &f_)->bool
        {
            return f_.z() >= 0 && f_.z() < m_vertexTexture.size();
        });
    });
}

QJsonDocument WavefrontOBJ::toJsonData() const
{
    const bool hasTexIndixes = hasTextureIndicies();
    const bool hasNormIndixes = hasNormalIndicies();
    Vector3di mask(0, hasTexIndixes ? 0 : -1, hasNormIndixes ? 0 : -1);

    QJsonObject variables;
    QString prevObjectName = g_defaultObjectNameName;

    const auto toObjectName = [](const QString &name_) ->QString
    {
        QString result = name_;
        for(int i = 0; i < result.size() ; ++i)
        {
            if(!isLetterNumeric(result.at(i)) && result.at(i) != '_')
            {
                result[i] = '_';
            }
        }
        return result;
    };

    const auto addObjects = [&variables, hasTexIndixes, hasNormIndixes, &toObjectName](
            const QString &prevObjectName_,
            const QJsonArray &vertext_,
            const QJsonArray &normals_,
            const QJsonArray &textures_)
    {
        QJsonObject vertexJO;
        vertexJO.insert(g_jsonDataVariableValueName, QJsonValue(vertext_));
        variables.insert(toObjectName(prevObjectName_ + g_vertexSpecialwordName), vertexJO);
        if(hasNormIndixes)
        {
            QJsonObject normalJO;
            normalJO.insert(g_jsonDataVariableValueName, QJsonValue(normals_));
            variables.insert(toObjectName(prevObjectName_ + g_vertexNormalSpecialwordName), normalJO);
        }
        if(hasTexIndixes)
        {
            QJsonObject textureJO;
            textureJO.insert(g_jsonDataVariableValueName, QJsonValue(textures_));
            variables.insert(toObjectName(prevObjectName_ + g_vertexTextureSpecialwordName), textureJO);
        }
    };

    if(m_objectNames.isEmpty())
    {
        QJsonArray vertext;
        QJsonArray normal;
        QJsonArray textures;
        if(buildObject(-1, -1, mask, vertext, normal, textures))
        {
            addObjects(prevObjectName, vertext, normal, textures);
        }
    }
    else
    {
        int prevLineNumber = 0;
        for(const WavefrontOBJObjectName &objectName: m_objectNames)
        {
            QJsonArray vertext;
            QJsonArray normal;
            QJsonArray textures;
            if(buildObject(prevLineNumber, objectName.getLine(), mask, vertext, normal, textures))
            {
                addObjects(prevObjectName, vertext, normal, textures);
            }
            prevLineNumber = objectName.getLine();
            prevObjectName = static_cast<const QString&>(objectName);
        }
        if(!m_faceElements.isEmpty())
        {
            QJsonArray vertext;
            QJsonArray normal;
            QJsonArray textures;
            if(buildObject(prevLineNumber, m_faceElements.back().getLine(), mask, vertext, normal, textures))
            {
                addObjects(prevObjectName, vertext, normal, textures);
            }
        }
    }

    return QJsonDocument(variables);
}


QVector<WavefrontOBJFaceElement>::const_iterator WavefrontOBJ::lower_bound(
        const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_)
{
    int i1 = 0, i2 = faceElements_.size();
    for(;i2 - i1 > 1; )
    {
        const int i0 = (i2 - i1) / 2;
        const int i0line = faceElements_[i0].getLine();
        if(i0line <= lineNumber_)
        {
            i1 = i0;
        }
        if(i0line >= lineNumber_)
        {
            i2 = i0;
        }
    }
    return std::begin(faceElements_) + i1;
}

QVector<WavefrontOBJFaceElement>::const_iterator WavefrontOBJ::upper_bound(
        const QVector<WavefrontOBJFaceElement> &faceElements_, int lineNumber_)
{
    int i1 = 0, i2 = faceElements_.size();
    for(;i2 - i1 > 1; )
    {
        const int i0 = (i2 - i1) / 2;
        const int i0line = faceElements_[i0].getLine();
        if(i0line <= lineNumber_)
        {
            i1 = i0;
        }
        if(i0line >= lineNumber_)
        {
            i2 = i0;
        }
    }
    return std::begin(faceElements_) + i2;
}

bool WavefrontOBJ::buildObject(
        int startLineNumber_, int endLineNumber_,
        const Vector3di &mask_,
        QJsonArray &vertex_,
        QJsonArray &normal_,
        QJsonArray &textures_
        ) const
{
    QVector<WavefrontOBJFaceElement>::const_iterator fbit = std::begin(m_faceElements);
    QVector<WavefrontOBJFaceElement>::const_iterator feit = std::end(m_faceElements);
    if(startLineNumber_ >= 0)
    {
        fbit = upper_bound(m_faceElements, startLineNumber_);
    }
    if(endLineNumber_ >= 0)
    {
        feit = lower_bound(m_faceElements, endLineNumber_);
    }
    std::set<Vector3di> unique;
    int indexesCount = 0;
    for(QVector<WavefrontOBJFaceElement>::const_iterator it = fbit; it != feit; ++it)
    {
        int fc = 0;
        for(const Vector3di &f_ : static_cast<const QVector<Vector3di>&>(*it))
        {
            Vector3di f(f_);
            f.mask(mask_);
            unique.insert(f);
            // allocate triangles for polygons (pt.count() > 3)
            ++indexesCount;
            if(fc >= 3) { indexesCount += 2; }
            ++fc;
        }
    }

    if(unique.empty()) { return false; }

    QVector<QVector4D> vertex;
    QVector<QVector3D> vertexTexture;
    QVector<QVector3D> vertexNormal;

    vertex.resize(unique.size());
    vertexTexture.resize(unique.size());
    vertexNormal.resize(unique.size());

    int j = 0;
    for(const Vector3di &f_: unique)
    {
        if(f_.x() >= 0 && f_.x() < m_vertex.size())
        {
            vertex[j] = m_vertex[f_.x()];
        }
        if(f_.y() >= 0 && f_.y() < m_vertexTexture.size())
        {
            vertexTexture[j] = m_vertexTexture[f_.y()];
        }
        if(f_.z() >= 0 && f_.z() < m_normal.size())
        {
            vertexNormal[j] = m_normal[f_.z()];
        }
        ++j;
    }

    for(QVector<WavefrontOBJFaceElement>::const_iterator it = fbit; it != feit; ++it)
    {
        int fc = 0;
        std::vector<std::size_t> ii;
        for(const Vector3di &f_ : static_cast<const QVector<Vector3di>&>(*it))
        {
            Vector3di f(f_);
            f.mask(mask_);
            std::set<Vector3di>::const_iterator fit = unique.find(f);
            std::size_t ci = 0;
            if(std::end(unique) != fit) { ci = std::distance(std::begin(unique), fit); }
            // build triangles
            if(fc >= 3 && !ii.empty())
            {
                const std::size_t prev = ii.back();
                ii.push_back(ii[0]);
                ii.push_back(prev);
            }
            ii.push_back(ci);
            ++fc;
        }
    }

    vertex_ = QJsonArray{};
    int vi = 0;
    for(const QVector4D &v_ : vertex)
    {
        for(std::size_t i = 0; i < 4; i++)
        {
            vertex_.append(QJsonValue(v_[i]));
        }
        ++vi;
    }
    if(mask_.y() >= 0)
    {
        Q_ASSERT(vertexTexture.size() == vertex.size());
        normal_ = QJsonArray{};
        int vti = 0;
        for(const QVector3D &v_ : vertexTexture)
        {
            for(std::size_t i = 0; i < 3; i++)
            {
                normal_.append(QJsonValue(v_[i]));
            }
            ++vti;
        }
    }
    if(mask_.z() >= 0)
    {
        Q_ASSERT(vertexTexture.size() == vertex.size());
        textures_ = QJsonArray{};
        int vni = 0;
        for(const QVector3D &v_ : vertexNormal)
        {
            for(std::size_t i = 0; i < 3; i++)
            {
                textures_.append(QJsonValue(v_[i]));
            }
            ++vni;
        }
    }

    return true;
}


QJsonDocument graphicsOBJtoJson(const QString &objData_)
{
    WavefrontOBJ *obj = parseGraphicsOBJ(objData_);
    QJsonDocument resultDocument;
    if(obj)
    {
        resultDocument = obj->toJsonData();
    }
    delete obj;
    obj = nullptr;
    return resultDocument;
}

WavefrontOBJ *parseGraphicsOBJ(const QString &objData_)
{
    WavefrontOBJ *res = new WavefrontOBJ();
    QStringList lines = objData_.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
    int lineNumber = 0;
    for(const QString &line0 : lines)
    {
        QString comment;
        const QString line = removeComment(line0, comment);
        if(res->processLine(line, comment, lineNumber)) { ++lineNumber; }
    }
    res->correct();
    if(!res->validate())
    {
        delete res;
        res = nullptr;
    }
    return res;
}
