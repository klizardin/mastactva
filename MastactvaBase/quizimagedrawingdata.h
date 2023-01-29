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

#ifndef QUIZIMAGEDRAWINGDATA_H
#define QUIZIMAGEDRAWINGDATA_H


#include <type_traits>
#include <set>
#include <vector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include "../MastactvaBase/opengldrawing_utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/utils.h"


#if QT_CONFIG(opengl)


namespace drawing_data
{
    enum class ItemTypesEn
    {
        none,                   // null item (special uninitialized value)
        fromItem,               // start enum item (to enumarate enum items)
        GLfloat,
        GLint,
        GLuint,
        QVector2D,
        QVector3D,
        QVector4D,
        QColor,
        QPoint,
        QPointF,
        QSize,
        QSizeF,
        QMatrix2x2,
        QMatrix2x3,
        QMatrix2x4,
        QMatrix3x2,
        QMatrix3x3,
        QMatrix3x4,
        QMatrix4x2,
        QMatrix4x3,
        QMatrix4x4,
        toItem                  // end enum item (to enumarate enum items)
    };

    namespace details
    {

    /*
     * convert to item with type from the vector of some type
     * TODO: maybe optimize there already are such functions with little bit differenet signature
    */
    template<typename ResultType_, typename ItemType_> inline
    ResultType_ fromVector(const std::vector<ItemType_> &);

    /*
     * convert from the item type to the vector of some type
     * TODO: maybe optimize
    */
    template<typename ResultType_, typename ItemType_> inline
    std::vector<ResultType_> toVector(const ItemType_ &);

    /*
     * specific implementations...
    */
    template<> inline
    GLfloat fromVector<GLfloat, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLfloat{};
    }

    template<> inline
    GLint fromVector<GLint, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? static_cast<GLint>(data_.at(0)) : GLint{};
    }

    template<> inline
    GLuint fromVector<GLuint, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return !data_.empty() ? static_cast<GLuint>(data_.at(0)) : GLuint{};
    }

    template<> inline
    GLint fromVector<GLint, GLint>(const std::vector<GLint> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLint{};
    }

    template<> inline
    GLuint fromVector<GLuint, GLuint>(const std::vector<GLuint> &data_)
    {
        return !data_.empty() ? data_.at(0) : GLuint{};
    }

    template<> inline
    QVector2D fromVector<QVector2D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2
                ? QVector2D{data_.at(0), data_.at(1)}
                : QVector2D{}
                  ;
    }

    template<> inline
    QColor fromVector<QColor, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QColor result;
        if(data_.size() >= 3)
        {
            result.setRedF(data_.at(0));
            result.setGreenF(data_.at(1));
            result.setBlueF(data_.at(2));
        }
        if(data_.size() >= 4)
        {
            result.setAlphaF(data_.at(3));
        }
        return result;
    }

    template<> inline
    QPointF fromVector<QPointF, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QPointF result;
        if(data_.size() >= 2)
        {
            result.setX(data_.at(0));
            result.setY(data_.at(1));
        }
        return result;
    }

    template<> inline
    QPoint fromVector<QPoint, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QPoint result;
        if(data_.size() >= 2)
        {
            result.setX(data_.at(0));
            result.setY(data_.at(1));
        }
        return result;
    }

    template<> inline
    QSizeF fromVector<QSizeF, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QSizeF result;
        if(data_.size() >= 2)
        {
            result.setWidth(data_.at(0));
            result.setHeight(data_.at(1));
        }
        return result;
    }

    template<> inline
    QSize fromVector<QSize, GLfloat>(const std::vector<GLfloat> &data_)
    {
        QSize result;
        if(data_.size() >= 2)
        {
            result.setWidth(data_.at(0));
            result.setHeight(data_.at(1));
        }
        return result;
    }

    template<> inline
    QVector3D fromVector<QVector3D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3
                ? QVector3D{data_.at(0), data_.at(1), data_.at(2)}
                : QVector3D{}
                  ;
    }

    template<> inline
    QVector4D fromVector<QVector4D, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4
                ? QVector4D{data_.at(0), data_.at(1), data_.at(2), data_.at(3)}
                : QVector4D{}
                  ;
    }

    template<> inline
    QMatrix2x2 fromVector<QMatrix2x2, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2*2
                ? QMatrix2x2{&data_[0]}
                : QMatrix2x2{}
                  ;
    }

    template<> inline
    QMatrix2x3 fromVector<QMatrix2x3, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2*3
                ? QMatrix2x3{&data_[0]}
                : QMatrix2x3{}
                  ;
    }

    template<> inline
    QMatrix2x4 fromVector<QMatrix2x4, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 2*4
                ? QMatrix2x4{&data_[0]}
                : QMatrix2x4{}
                  ;
    }

    template<> inline
    QMatrix3x3 fromVector<QMatrix3x3, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3*3
                ? QMatrix3x3{&data_[0]}
                : QMatrix3x3{}
                  ;
    }

    template<> inline
    QMatrix3x2 fromVector<QMatrix3x2, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3*2
                ? QMatrix3x2{&data_[0]}
                : QMatrix3x2{}
                  ;
    }

    template<> inline
    QMatrix3x4 fromVector<QMatrix3x4, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 3*4
                ? QMatrix3x4{&data_[0]}
                : QMatrix3x4{}
                  ;
    }

    template<> inline
    QMatrix4x4 fromVector<QMatrix4x4, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4*4
                ? QMatrix4x4{&data_[0]}
                : QMatrix4x4{}
                  ;
    }

    template<> inline
    QMatrix4x2 fromVector<QMatrix4x2, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4*2
                ? QMatrix4x2{&data_[0]}
                : QMatrix4x2{}
                  ;
    }

    template<> inline
    QMatrix4x3 fromVector<QMatrix4x3, GLfloat>(const std::vector<GLfloat> &data_)
    {
        return data_.size() >= 4*3
                ? QMatrix4x3{&data_[0]}
                : QMatrix4x3{}
                  ;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLfloat>(const GLfloat &val_)
    {
        std::vector<GLfloat> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLint>(const GLint &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_)};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, GLuint>(const GLuint &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_)};
        return result;
    }

    template<> inline
    std::vector<GLint> toVector<GLint, GLint>(const GLint &val_)
    {
        std::vector<GLint> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLuint> toVector<GLuint, GLuint>(const GLuint &val_)
    {
        std::vector<GLuint> result = {val_};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector2D>(const QVector2D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QColor>(const QColor &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_.redF()),
                                       static_cast<GLfloat>(val_.greenF()),
                                       static_cast<GLfloat>(val_.blueF()),
                                       static_cast<GLfloat>(val_.alphaF())
                                      };
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QPointF>(const QPointF &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_.x()),
                                       static_cast<GLfloat>(val_.y()),
                                      };
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QPoint>(const QPoint &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_.x()),
                                       static_cast<GLfloat>(val_.y()),
                                      };
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QSizeF>(const QSizeF &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_.width()),
                                       static_cast<GLfloat>(val_.height()),
                                      };
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QSize>(const QSize &val_)
    {
        std::vector<GLfloat> result = {static_cast<GLfloat>(val_.width()),
                                       static_cast<GLfloat>(val_.height()),
                                      };
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector3D>(const QVector3D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y(), val_.z()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QVector4D>(const QVector4D &val_)
    {
        std::vector<GLfloat> result = {val_.x(), val_.y(), val_.z(), val_.w()};
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix2x2>(const QMatrix2x2 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(2*2);
        std::copy(val_.constData(), val_.constData() + 2*2, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix2x3>(const QMatrix2x3 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(2*3);
        std::copy(val_.constData(), val_.constData() + 2*3, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix2x4>(const QMatrix2x4 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(2*4);
        std::copy(val_.constData(), val_.constData() + 2*4, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix3x3>(const QMatrix3x3 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(3*3);
        std::copy(val_.constData(), val_.constData() + 3*3, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix3x2>(const QMatrix3x2 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(3*2);
        std::copy(val_.constData(), val_.constData() + 3*2, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix3x4>(const QMatrix3x4 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(3*4);
        std::copy(val_.constData(), val_.constData() + 3*4, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix4x4>(const QMatrix4x4 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(4*4);
        std::copy(val_.constData(), val_.constData() + 4*4, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix4x2>(const QMatrix4x2 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(4*2);
        std::copy(val_.constData(), val_.constData() + 4*2, std::back_inserter(result));
        return result;
    }

    template<> inline
    std::vector<GLfloat> toVector<GLfloat, QMatrix4x3>(const QMatrix4x3 &val_)
    {
        std::vector<GLfloat> result;
        result.reserve(4*3);
        std::copy(val_.constData(), val_.constData() + 4*3, std::back_inserter(result));
        return result;
    }

    }


    /*
     * helper class with constatnts
    */
    class ItemTypeConvert
    {
    public:
        constexpr static int minIndex{ to_underlying(ItemTypesEn::fromItem) + 1 };      // the first enum item index, to eneumarate types
        constexpr static int maxIndex{ to_underlying(ItemTypesEn::toItem) - 1 };        // the last enum item index, to eneumarate types
                                                                                        // types should be continues
    };


    /*
     * item type traits
    */
    template<typename ItemType_>
    class ItemTypeTraits
    {
    public:
        constexpr static int tupleSize{ 0 };
        constexpr static int typeIndex{ to_underlying(ItemTypesEn::none) };
        using underlayingType = void;
    };


    /*
     * item types traits
    */
    template<std::underlying_type_t<ItemTypesEn> TypeIndex_>
    class ItemTypeIndexTraits
    {
    public:
        using type = void;
    };


    /*
     * macro to specific concrete item type traits
    */
#define DRAWING_DATA_ITEM_TYPE_TRAITS(ItemType_, tupleSize_, underlayingType_)      \
    template<>                                                                      \
    class ItemTypeTraits<ItemType_>                                                 \
    {                                                                               \
    public:                                                                         \
        constexpr static int tupleSize{tupleSize_};                                 \
        constexpr static std::underlying_type_t<ItemTypesEn> typeIndex{             \
                                        to_underlying(ItemTypesEn::ItemType_)};     \
        using underlayingType = underlayingType_;                                   \
    };                                                                              \
    template<>                                                                      \
    class ItemTypeIndexTraits<to_underlying(ItemTypesEn::ItemType_)>                \
    {                                                                               \
    public:                                                                         \
        using type = ItemType_;                                                     \
    };                                                                              \
/*end traints macro*/


    DRAWING_DATA_ITEM_TYPE_TRAITS(GLfloat,       1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(GLint,         1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(GLuint,        1,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector2D,     2,  GLfloat)       // has underlaying type
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector3D,     3,  GLfloat)       // has underlaying type
    DRAWING_DATA_ITEM_TYPE_TRAITS(QVector4D,     4,  GLfloat)       // has underlaying type
    DRAWING_DATA_ITEM_TYPE_TRAITS(QColor,        3,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QPoint,        2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QPointF,       2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QSize,         2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QSizeF,        2,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x2,    4,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x3,    6,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix2x4,    8,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x2,    6,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x3,    9,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix3x4,    12, void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x2,    8,  void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x3,    12, void)
    DRAWING_DATA_ITEM_TYPE_TRAITS(QMatrix4x4,    16, void)

#undef DRAWING_DATA_ITEM_TYPE_TRAITS


    /*
     * item type traits as virtual functions
    */
    class ITypeInfo
    {
    public:
        virtual ~ITypeInfo() = default;
        virtual int typeIndex() const = 0;
        virtual int tupleSize() const = 0;
    };


    template<typename ItemType_>
    class ITypeInfoImpl : public virtual ITypeInfo
    {
    public:
        int typeIndex() const override
        {
            // ItemTypeTraits<ItemType_>::typeIndex should be at least int
            // for compatibility with indexing in other places
            static_assert(
                sizeof(decltype(ItemTypeTraits<ItemType_>::typeIndex)) <= sizeof(int),
                "underlying type greater then int"
                );
            return ItemTypeTraits<ItemType_>::typeIndex;
        }

        int tupleSize() const override
        {
            return ItemTypeTraits<ItemType_>::tupleSize;
        }
    };


    /*
     * IAttribute interface
    */
    class IAttribute : public virtual ITypeInfo
    {
    public:
        virtual ~IAttribute() = default;
        virtual const QString &name() const = 0;        // variable name
        virtual const GLfloat *constData() const = 0;   // variable const data
        virtual int size() const = 0;                   // variable data size
    };


    /*
     * attribute variable implementation
     * add semantic to work with atttribute variables for open gl
    */
    template<class ItemType_>
    class Attribute : public IAttribute, public ITypeInfoImpl<ItemType_>
    {
    public:
        Attribute(const QString &name_, std::shared_ptr<std::vector<ItemType_>> data_)
            : m_name(name_),
              m_data(std::move(data_))
        {}

    public:
        const QString &name() const override
        {
            return m_name;
        }

        const GLfloat *constData() const override
        {
            if(!m_data.operator bool()
                    || m_data->empty())
            {
                return nullptr;
            }
            return &(const_cast<Attribute<ItemType_> *>(this)->m_data.get()->at(0)[0]);
        }

        int size() const override
        {
            return m_data.operator bool() ? m_data->size() : 0;
        }

        // for the vector of the same type
        // we should specify tuple size == 0
        // it is tuple size of the array argument
        void set(const std::vector<ItemType_> &value_, int tupleSize_)
        {
            createData();

            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            *m_data = value_;
        }

        // for the vector of the same type
        // we should specify tuple size == 0
        // it is tuple size of the array argument
        bool get(std::vector<ItemType_> &value_, int tupleSize_) const
        {
            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_ = *m_data;
            return true;
        }

        // for the vector of the different type
        template<typename ItemType2_>
        void set(const std::vector<ItemType2_> &value_, int tupleSize_)
        {
            setImpl(value_, tupleSize_,
                    // is simply convertible (can we use simple static_cast between types?)
                    typename std::integral_constant<
                        bool
                        ,std::is_convertible<ItemType2_, ItemType_>::value
                        >::type(),
                    // is underlaying type is the same type
                    typename std::integral_constant<
                        bool
                        , std::is_same<typename ItemTypeTraits<ItemType_>::underlayingType, ItemType2_>::value
                        >::type()
                    );
        }

        template<typename ItemType2_>
        bool get(std::vector<ItemType2_> &value_, int tupleSize_) const
        {
            return getImpl(value_, tupleSize_,
                    // is simply convertible (can we use simple static_cast between types?)
                    typename std::integral_constant<
                        bool
                        ,std::is_convertible<ItemType_, ItemType2_>::value
                        >::type(),
                    // is underlaying type is the same type
                    typename std::integral_constant<
                        bool
                        , std::is_same<typename ItemTypeTraits<ItemType2_>::underlayingType, ItemType_>::value
                        >::type()
                    );
        }

    private:
        void createData()
        {
            if(m_data.operator bool())
            {
                return;
            }

            m_data = std::make_shared<std::vector<ItemType_>>();
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::false_type)
        {
            createData();

            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            m_data->reserve(value_.size());
            m_data->clear();
            for(const ItemType2_ &val_ : value_)
            {
                m_data->push_back(static_cast<ItemType_>(val_));
            }
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::true_type)
        {
            createData();

            const bool cond = 0 < tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return;
            }

            m_data->resize(value_.size() / tupleSize_);
            for(std::size_t i = 0; i < m_data->size(); ++i)
            {
                for(std::size_t j = 0; j < std::size_t(tupleSize_); ++j)
                {
                    (*m_data)[i][j] = static_cast<
                            typename ItemTypeTraits<ItemType_>::underlayingType
                            >(
                                value_[i*tupleSize_ + j]
                            );
                }
            }
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &, int, std::false_type, std::false_type)
        {
            // imposible combination
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &, int, std::true_type, std::true_type)
        {
            // imposible combination
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, int tupleSize_, std::true_type, std::false_type) const
        {
            const bool cond = 0 == tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_.reserve(m_data->size());
            value_.clear();
            for(const ItemType_ &val_ : *m_data)
            {
                value_.push_back(static_cast<ItemType2_>(val_));
            }
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, int tupleSize_, std::false_type, std::true_type) const
        {
            const bool cond = 0 < tupleSize_;
            Q_ASSERT(cond);
            if(!cond)
            {
                return false;
            }

            value_.resize(m_data->size() / tupleSize_);
            for(std::size_t i = 0; i < m_data->size(); ++i)
            {
                for(std::size_t j = 0; j < std::size_t(tupleSize_); ++j)
                {
                    value_[i*tupleSize_ + j]
                        = static_cast<ItemType2_>(
                                (*m_data)[i][j]
                            );
                }
            }
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &, int, std::false_type, std::false_type) const
        {
            // imposible combination
            return false;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &, int, std::true_type, std::true_type) const
        {
            // imposible combination
            return false;
        }

    private:
        QString m_name;                                 // name of the attribute
        std::shared_ptr<std::vector<ItemType_>> m_data; // shared data with a some variable from IVariables of the effect data
                                                        // so that runtime scripts can work also with effect data
                                                        // and we do not need extra copying of maybe large data
    };


    /*
     * interface for the uniform drawing data variables
    */
    class IUniform : public virtual ITypeInfo
    {
    public:
        virtual ~IUniform() = default;
        virtual const QString &name() const = 0;        // name of the variable
        virtual void set(QOpenGLShaderProgram *program, int location_) const = 0;   // (break SOLID) set variable to the program at specific location
        virtual void getVector(std::vector<GLfloat> &values_) const = 0;            // return verctor of this data (do copy)
        // break SOLID - not a simple data
    };


    /*
     * implementation of the uniform variables for the drawing data
     * add semantic to work with uniform variables for open gl
    */
    template<typename ItemType_>
    struct Uniform : public IUniform, public ITypeInfoImpl<ItemType_>
    {
        Uniform(const QString &name_, std::shared_ptr<ItemType_> data_)
            : m_name(name_)
            , m_data(std::move(data_))
        {
        }

        virtual const QString &name() const
        {
            return m_name;
        }

        virtual void set(QOpenGLShaderProgram *program, int location_) const
        {
            if(!program
                    || location_ < 0
                    || !m_data.operator bool()
                    )
            {
                return;
            }

            // use reloadings of the QT
            program->setUniformValue(location_, *m_data);
        }

        /*
         * for the same type
        */
        void set(const ItemType_ &value_)
        {
            createData();

            *m_data = value_;
        }

        /*
         * for the different type
        */
        template<typename ItemType2_>
        void set(const ItemType2_ &value_)
        {
            return setImpl(value_,
                           // is simply convertible so that we can use static_cast
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType2_,
                                    ItemType_
                                    >::value
                                >::type());
        }

        /*
         * for different type as a vector of values
        */
        template<typename ItemType2_>
        void set(const std::vector<ItemType2_> &value_)
        {
            return setImpl(value_, std::true_type());
        }

        /*
         * for the same type
        */
        bool get(ItemType_ &value_) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = *m_data;
            return true;
        }

        /*
         * for the different type
        */
        template<typename ItemType2_>
        bool get(ItemType2_ &value_) const
        {
            return getImpl(value_,
                            // can we use simple static_cast
                            typename std::integral_constant<
                                bool,
                                std::is_convertible<
                                    ItemType_,
                                    ItemType2_
                                    >::value
                                >::type());
        }

        /*
         * for the different type as a vector of some types
        */
        template<typename ItemType2_>
        bool get(std::vector<ItemType2_> &value_) const
        {
            return getImpl(value_, std::true_type());
        }

        void getVector(std::vector<GLfloat> &values_) const override
        {
            values_ = details::toVector<GLfloat, ItemType_>(*m_data);
        }

    private:
        /*
         * helper function
        */
        void createData()
        {
            if(m_data.operator bool())
            {
                return;
            }

            // if the data was not initialized from the effect data
            m_data = std::make_shared<ItemType_>();
        }

        template<typename ItemType2_>
        bool getImpl(ItemType2_ &value_, std::true_type) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = static_cast<ItemType2_>(*m_data);
            return true;
        }

        template<typename ItemType2_>
        bool getImpl(ItemType2_ &, std::false_type) const
        {
            return false;
        }

        template<typename ItemType2_>
        bool getImpl(std::vector<ItemType2_> &value_, std::true_type) const
        {
            if(!m_data.operator bool())
            {
                return false;
            }

            value_ = details::toVector<ItemType2_, ItemType_>(*m_data);
            return true;
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &value_, std::true_type)
        {
            createData();

            *m_data = static_cast<ItemType_>(value_);
        }

        template<typename ItemType2_>
        void setImpl(const ItemType2_ &, std::false_type)
        {
        }

        template<typename ItemType2_>
        void setImpl(const std::vector<ItemType2_> &value_, std::true_type)
        {
            createData();

            *m_data = details::fromVector<ItemType_, ItemType2_>(value_);
        }

    private:
        QString m_name;                     // variable name
        std::shared_ptr<ItemType_> m_data;  // shared data
                                            // so that we can work with effect datas
    };


    namespace detail
    {
        /*
         * the helper template to dispose a specific template by the type index int value
        */
        template<int index_>
        class ItemTypeBase : public ItemTypeBase<index_ - 1>
        {
        public:
            using AttributeType = Attribute<typename ItemTypeIndexTraits<index_>::type>;    // the type to dispose for the current index int value
            using UniformType = Uniform<typename ItemTypeIndexTraits<index_>::type>;        // the type to dispose for the current index int value
            using base = ItemTypeBase<index_ - 1>;                                          // the base type

            /*
             * interface_ -- interface to dispose to the source template
             * itemIndex_ -- item type index int value
             * value_, tupleSize_ -- arguments for a set function of the disposed template
            */
            template<typename ItemType_>
            static void set(IAttribute *interface_, int itemIndex_, const std::vector<ItemType_> &value_, int tupleSize_)
            {
                if(itemIndex_ != index_)
                {
                    base::set(interface_, itemIndex_, value_, tupleSize_);
                    return;
                }

                AttributeType *attr = dynamic_cast<AttributeType *>(interface_);
                if(!attr)
                {
                    return;
                }
                attr->set(value_, tupleSize_);
            }

            /*
             * interface_ -- interface to dispose to the source template
             * itemIndex_ -- item type index int value
             * value_, tupleSize_ -- arguments for a get function of the disposed template
            */
            template<typename ItemType_>
            static bool get(const IAttribute *interface_, int itemIndex_, std::vector<ItemType_> &value_, int tupleSize_)
            {
                if(itemIndex_ != index_)
                {
                    return base::get(interface_, itemIndex_, value_, tupleSize_);
                }

                const AttributeType *attr = dynamic_cast<const AttributeType *>(interface_);
                if(!attr)
                {
                    return false;
                }
                return attr->get(value_, tupleSize_);
            }

            /*
             * interface_ -- interface to dispose to the source template
             * itemIndex_ -- item type index int value
             * value_ -- argument for a set function of the disposed template
            */
            template<typename ItemType_>
            static void set(IUniform *interface_, int itemIndex_, const ItemType_ &value_)
            {
                if(itemIndex_ != index_)
                {
                    base::set(interface_, itemIndex_, value_);
                    return;
                }

                UniformType *uniform = dynamic_cast<UniformType *>(interface_);
                if(!uniform)
                {
                    return;
                }
                uniform->set(value_);
            }

            /*
             * interface_ -- interface to dispose to the source template
             * itemIndex_ -- item type index int value
             * value_ -- argument for a get function of the disposed template
            */
            template<typename ItemType_>
            static bool get(const IUniform *interface_, int itemIndex_, ItemType_ &value_)
            {
                if(itemIndex_ != index_)
                {
                    return base::get(interface_, itemIndex_, value_);
                }

                const UniformType *uniform = dynamic_cast<const UniformType *>(interface_);
                if(!uniform)
                {
                    return false;
                }
                return uniform->get(value_);
            }
        };


        template<>
        class ItemTypeBase<ItemTypeConvert::minIndex - 1>
        {
        public:
            template<typename ItemType_>
            static void set(IAttribute *, int , const std::vector<ItemType_> &, int )
            {
            }

            template<typename ItemType_>
            static bool get(const IAttribute *, int , std::vector<ItemType_> &, int )
            {
                return false;
            }

            template<typename ItemType_>
            static void set(IUniform *, int , const ItemType_ &)
            {
            }

            template<typename ItemType_>
            static bool get(const IUniform *, int , ItemType_ &)
            {
                return false;
            }
        };
    } // namespace detail


    /*
     * the type definition for dispose of the specific template type for functions get()/set()
    */
    using ItemTypeBaseSet = detail::ItemTypeBase<ItemTypeConvert::maxIndex>;


    /*
     * the structure for the texture datas
     * (data holder)
    */
    struct Texture
    {
        QString name;       // texture name
        QString filename;   // file name associated with the texture name, maybe empty - no texture file for this texture
        QString textureName; // texture from the pipeline
    };


    namespace detail
    {
        /*
         * calculation wraper for a drawing opengl object
        */
        class Calculations
        {
        public:
            std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> calculations;  // calculations list

        public:
            Calculations(::opengl_drawing::IVariables *ivariables_);    // set up variables for this drawing object
            bool calculate(opengl_drawing::IVariables *variables_);     // do calculations, pass root variables
            void preCalculation();      // pre
            void postCalculation();     // post steps
            void init(const std::vector<QString> &effectCalculationNames_); // init calculations by the list of calculations state strings
                                                                            // state string is a function name and its arguments
            bool find(const opengl_drawing::IEffectCalculation *calculation_) const; // return true if calculation already exist
                                                                            // or calculation exists that expands the argument's calculation
            bool hasCalculations() const;                               // do we need to do any calculations

        protected:
            void setVariable(const QString &name_);                     // variable with specific name was updated
            bool isUpdated(const QSet<QString> &vars_, ::opengl_drawing::IVariables *base_) const;  // is any of variables from argument's list
                                                                        // updated

        private:
            void clearUpdated();                                        // clear updated variables list

        private:
            QSet<QString> m_updated;                                    // list of updated variable's names
            ::opengl_drawing::IVariables *m_thisIVariables = nullptr;   // this object's variables
            std::vector<std::shared_ptr<opengl_drawing::IEffectCalculation>> m_availableCalculations;   // temp data
        };
    }


    /*
     * quiz image drawing object - simple drawing step
     * can draw with one vertex and fragment shader
    */
    struct QuizImageObject : public ::opengl_drawing::IVariables, public detail::Calculations   // calculations for one drawing step
    {
    public:
        QByteArray vertexShader;        // vertex shader
        QByteArray fragmentShader;      // fragment shader

        std::vector<std::unique_ptr<IAttribute>> attributes;    // attribute variables that is required for the shaders of calculations
        std::vector<std::unique_ptr<IUniform>> uniforms;        // uniform variables that is required for the shaders of calculations
        std::vector<Texture> textures;                          // textures that is required for the shaders of calculations
        std::vector<QString> objectStates;                      // object's states that is required for the shaders of calculations
                // object state is a open gl primitives bunch that do simple drawing tuning
        std::vector<QString> objectCalculations;
                // list of object's calculations
        std::map<QString, QStringList> strVariables;
                // string variables (required only just by calculations)
        std::set<QString> textureTargets;
                // terget textures for the object
                // object will be drawen for these textures

    public:
        QuizImageObject();
        void postInitialization();

        // ::opengl_drawing::IVariables
        // {
        bool get(const QString &name_, QVector<double> &data_) const override;
        void set(const QString &name_, const QVector<double> &data_) override;
        void set(const QString &name_, QVector<double> &&data_) override;
        bool get(const QString &name_, QStringList &data_) const override;
        void set(const QString &name_, const QStringList &data_) override;
        void set(const QString &name_, QStringList &&data_) override;
        bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override;
        // }

        /*
         * is this drawing object should be drawn for the time t_
        */
        bool allowedForTime(double t_) const;
        /*
         * update time from told_ to tnew_ for the next drawing step
        */
        bool changeAllowedForTime(double told_, double tnew_) const;

        /*
         * add target texture name
        */
        void addTargetTexture(const QString &targetTextureName_);

        /*
         * check if allowed to draw this object for specific target texture name
        */
        bool allowedForTargetTexture(const QString& tartgetTextureName_) const;

        /*
         * set attribute variable value
        */
        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            setVariable(name_);

            for(std::unique_ptr<IAttribute> &attribute_ : attributes)
            {
                if(!attribute_.operator bool()
                        || attribute_->name() != name_
                        )
                {
                    continue;
                }

                ItemTypeBaseSet::set(attribute_.get(), attribute_->typeIndex(), value_, tupleSize_);
                // set all attributes with this name
            }
        }

        /*
         * get attribute variable tuple size value
        */
        int getAttributeTupleSize(const QString &name_) const;

        /*
         * set uniform variable value
        */
        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            setVariable(name_);

            for(std::unique_ptr<IUniform> &uniform_ : uniforms)
            {
                if(!uniform_.operator bool()
                        || uniform_->name() != name_
                        )
                {
                    continue;
                }

                ItemTypeBaseSet::set(uniform_.get(), uniform_->typeIndex(), value_);
                // set all uniforms with this name
                // TODO: ptimize for std::shared_ptr data use
            }
        }

        /*
         * get uniform variable value
        */
        template<typename ItemType_>
        bool getUniform(const QString &name_, ItemType_ &value_) const
        {
            for(const std::unique_ptr<IUniform> &uniform_ : uniforms)
            {
                if(!uniform_.operator bool()
                        || uniform_->name() != name_
                        )
                {
                    continue;
                }

                // return first value
                return ItemTypeBaseSet::get(uniform_.get(), uniform_->typeIndex(), value_);
            }
            return false;
        }

        /*
         * set texture to the file name
        */
        void setTexture(const QString &name_, const QString &newFilename_);

        /*
         * return list of all variables
        */
        QStringList getArgumentNames() const;

        /*
         * return value for the attribute or for the uniform variable as an array of floats
        */
        bool getArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const;

    private:
        bool m_postInitializationDone = false;
    };


    /*
     * quiz image drawing objects
     * bunch of simple drawing steps
    */
    struct QuizImageObjects : public ::opengl_drawing::IVariables, public detail::Calculations
    {
    public:
        // global variables: global clear color
        QColor clearColor{255, 255, 255};
        std::vector<std::shared_ptr<QuizImageObject>> objects;  // drawing object's list
        std::vector<QString> globalStates;  // global states
        std::vector<QString> globalCalculations;    // global calculations

    public:
        QuizImageObjects();
        void postInitialization();

        // root ::opengl_drawing::IVariables
        // {
        bool get(const QString &name_, QVector<double> &data_) const override;
        void set(const QString &name_, const QVector<double> &data_) override;
        void set(const QString &name_, QVector<double> &&data_) override;
        bool get(const QString &name_, QStringList &data_) const override;
        void set(const QString &name_, const QStringList &data_) override;
        void set(const QString &name_, QStringList &&data_) override;
        bool isUpdated(const QSet<QString> &vars_, IVariables *base_) const override;
        // }

        /*
         * set attribute variable value from root
        */
        template<typename ItemType_>
        void setAttribute(const QString &name_, const std::vector<ItemType_> &value_, int tupleSize_ = 0)
        {
            setVariable(name_);

            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->setAttribute(name_, value_, tupleSize_);
            }
        }

        /*
         * get attribute variable tuple size value from root
        */
        int getAttributeTupleSize(const QString &name_) const;

        /*
         * set uniform variable value from root
        */
        template<typename ItemType_>
        void setUniform(const QString &name_, const ItemType_ &value_)
        {
            setVariable(name_);

            for(std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                object_->setUniform(name_, value_);
            }
        }

        /*
         * get uniform variable value from root
        */
        template<typename ItemType_>
        bool getUniform(const QString &name_, ItemType_ &value_) const
        {
            bool res = false;
            for(const std::shared_ptr<QuizImageObject> &object_ : objects)
            {
                if(!object_.operator bool())
                {
                    continue;
                }

                res |= object_->getUniform(name_, value_);
                if(res)
                {
                    // return first value from list
                    break;
                }
            }
            return res;
        }

        /*
         * set texture to the file name from root
        */
        void setTexture(const QString &name_, const QString &newFilename_);

        /*
         * run all calculations for this drawing step
        */
        void calculate(opengl_drawing::IVariables *variables_);

        /*
         * return list of all variables from root
        */
        QStringList getArgumentNames() const;

        /*
         * return value for the attribute or for the uniform variable as an array of floats from root
        */
        bool getArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const;

        /*
         * return list of pseudo variables from root
        */
        QStringList getPseudoArgumentsNames() const;

        /*
         * return value for pseudo variable as an array of floats from root
        */
        bool getPseudoArgumentValue(const QString &name_, std::vector<GLfloat> &values_) const;

    private:
        bool calculateStep(opengl_drawing::IVariables *variables_);
        bool m_postInitializationDone = false;
    };
}


#endif // #if QT_CONFIG(opengl)
#endif // QUIZIMAGEDRAWINGDATA_H
