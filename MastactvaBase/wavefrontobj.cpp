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

#include "wavefrontobj.h"
#include <set>
#include <functional>
#include <tuple>
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


void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, QVector4D &data_)
{
    data_.setW(1.0);
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

void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, QVector3D &data_)
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

void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, QVector<Vector3di> &data_)
{
    QStringList values = line_.split(QChar(' '), Qt::SkipEmptyParts);
    for(const QString &val_: values)
    {
        QStringList faceItems = val_.split(QChar('/'), Qt::KeepEmptyParts);
        std::size_t j = 0;
        Vector3di val;
        for(const QString &fval_: faceItems)
        {
            bool ok = false;
            int vali = QVariant::fromValue(fval_).toInt(&ok);
            if(ok)
            {
                val[j] = vali;
            }
            j++;
            if(3 <= j)
            {
                break;
            }
        }
        data_.push_back(val);
    }
}

void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, QVector<int> &data_)
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

void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, QString &data_)
{
    data_ = line_;
}

void WavefrontOBJ::parseWavefrontOBJLine(const QString &line_, Bool &data_)
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

template<typename WavefrontOBJType_> inline
void initWavefrontOBJItem(
        const QString &dataLine_,
        const QString &comment_,
        int lineNumber_,
        QVector<WavefrontOBJType_> &vec_
        )
{
    WavefrontOBJType_ d;
    d.setComment(comment_);
    d.setLine(lineNumber_);
    WavefrontOBJ::parseWavefrontOBJLine(dataLine_, d);
    vec_.push_back(std::move(d));
}

template<typename WavefrontOBJTupleType_> inline
bool initWavefrontOBJItemList(
        const QString &line_, const QString &comment_, int lineNumber_,
        WavefrontOBJTupleType_ &&val_
        )
{
    QString dataLine;
    if(WavefrontOBJ::startsWith(line_, val_.second, dataLine))
    {
        initWavefrontOBJItem(dataLine, comment_, lineNumber_, val_.first);
        return true;
    }
    else
    {
        return false;
    }
}

template<typename WavefrontOBJTupleType_, typename ... WavefrontOBJTuplesTypes_> inline
bool initWavefrontOBJItemList(
        const QString &line_, const QString &comment_, int lineNumber_,
        WavefrontOBJTupleType_ &&val_, WavefrontOBJTuplesTypes_  &&... vals_
        )
{
    QString dataLine;
    if(WavefrontOBJ::startsWith(line_, val_.second, dataLine))
    {
        initWavefrontOBJItem(dataLine, comment_, lineNumber_, val_.first);
        return true;
    }
    else
    {
        return initWavefrontOBJItemList(line_, comment_, lineNumber_, std::move(vals_) ...);
    }
}

bool WavefrontOBJ::processLine(const QString &line_, const QString &comment_, int lineNumber_)
{
    if(initWavefrontOBJItemList(
        line_, comment_, lineNumber_,
        std::make_pair(std::ref(m_vertex), "v"),
        std::make_pair(std::ref(m_vertexTexture), "vt"),
        std::make_pair(std::ref(m_normal), "vn"),
        std::make_pair(std::ref(m_vertexParameter), "vp"),
        std::make_pair(std::ref(m_faceElements), "f"),
        std::make_pair(std::ref(m_lineElements), "l"),
        std::make_pair(std::ref(m_objectNames), "o"),
        std::make_pair(std::ref(m_groupNames), "g"),
        std::make_pair(std::ref(m_materialLibs), "mtllib"),
        std::make_pair(std::ref(m_materialNames), "usemtl"),
        std::make_pair(std::ref(m_smoothing), "s")
        ))
    {
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
                f_.setY(m_vertexTexture.size() - f_.y());
            }
            else
            {
                f_.setY(f_.y() - 1);
            }

            if(f_.z() < 0)
            {
                f_.setZ(m_normal.size() - f_.z());
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

bool WavefrontOBJ::isValid() const
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
            return (f_.x() >= 0 && f_.x() < m_vertex.size())
                    && ((f_.y() >= -1 && f_.y() < m_vertexTexture.size()))
                    && ((f_.z() >= -1 && f_.z() < m_normal.size()))
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
    if(line_.startsWith(str_ + QString(" ")))
    {
        dataLine_ = line_.mid(str_.length() + 1);
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
            return f_.z() >= 0 && f_.z() < m_normal.size();
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

inline
void buildResultArray(
        const std::vector<std::vector<std::size_t>> &triangles_,
        const QVector<QVector4D> &vec_,
        QJsonArray &result_
        )
{
    int vi = 0;
    for(const std::vector<std::size_t> &tri : triangles_)
    {
        for(const std::size_t &index_ : tri)
        {
            Q_ASSERT((int)index_ < vec_.size());
            for(std::size_t i = 0; i < 4; i++)
            {
                result_.append(QJsonValue(vec_[index_][i]));
            }
        }
        ++vi;
    }
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
        fbit = std::lower_bound(
                    std::begin(m_faceElements), std::end(m_faceElements),
                    startLineNumber_,
                    [](const WavefrontOBJFaceElement &elem_, int lineNumber_)->bool
        {
            return elem_.getLine() < lineNumber_;
        });
    }
    if(endLineNumber_ >= 0)
    {
        feit = std::upper_bound(
                    std::begin(m_faceElements), std::end(m_faceElements),
                    endLineNumber_,
                    [](int lineNumber_, const WavefrontOBJFaceElement &elem_)->bool
        {
            return lineNumber_ < elem_.getLine();
        });
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
    QVector<QVector4D> vertexTexture;
    QVector<QVector4D> vertexNormal;

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

    std::vector<std::vector<std::size_t>> triangles;
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
        triangles.push_back(ii);
    }

    vertex_ = QJsonArray{};
    textures_ = QJsonArray{};
    normal_ = QJsonArray{};

    buildResultArray(triangles, vertex, vertex_);
    if(mask_.y() >= 0)
    {
        buildResultArray(triangles, vertexTexture, textures_);
    }
    if(mask_.z() >= 0)
    {
        buildResultArray(triangles, vertexNormal, normal_);
    }

    return true;
}

QJsonDocument WavefrontOBJ::graphicsOBJtoJson(const QString &objData_)
{
    std::unique_ptr<WavefrontOBJ> obj = parseGraphicsOBJ(objData_);
    QJsonDocument resultDocument;
    if(!obj)
    {
        return resultDocument;
    }
    resultDocument = obj->toJsonData();
    return resultDocument;
}

std::unique_ptr<WavefrontOBJ> WavefrontOBJ::parseGraphicsOBJ(const QString &objData_)
{
    std::unique_ptr<WavefrontOBJ> res = std::make_unique<WavefrontOBJ>();
    const QStringList lines = objData_.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
    int lineNumber = 0;
    for(const QString &line0 : lines)
    {
        QString comment;
        const QString line = removeComment(line0, comment);
        if(res->processLine(line, comment, lineNumber)) { ++lineNumber; }
    }
    res->correct();
    if(!res->isValid())
    {
        res.reset();
    }
    return res;
}
