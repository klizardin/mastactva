#ifndef DRAWINGDATA_UTILS_H
#define DRAWINGDATA_UTILS_H


#include <map>
#include <memory>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QImage>
#include <QJsonArray>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"


namespace drawingdata
{

namespace utils
{
    template<class DrawingDataType_, class DataType_> inline
    std::unique_ptr<DrawingDataType_> factory(DataType_ &&data_, const DrawingDataType_ *)
    {
        return std::make_unique<DrawingDataType_>(std::move(data_));
    }


    template<class DataType_, class DrawingDataType_> inline
    void rebuild(const std::shared_ptr<QVector<DataType_ *>> &data_, DrawingDataType_ *)
    {
        if(data_.operator bool())
        {
            for(DataType_ *& ptr_ : *data_)
            {
                if(nullptr == ptr_)
                {
                    continue;
                }
                auto ptr = factory<DrawingDataType_, DataType_>(std::move(*ptr_), nullptr);
                if(dynamic_cast<DataType_ *>(ptr.get()))
                {
                    ptr_ = dynamic_cast<DataType_ *>(ptr.release());
                }
                else
                {
                    ptr_ = nullptr;
                }
            }
        }
    }

    template<typename Type_> inline
    void toUniform(const QString &str_, Type_ &)
    {
        Q_UNUSED(str_);
        Q_ASSERT(false); // not implemented
    }

    template<typename Type_, typename ItemType_> inline
    void vecToUniform(const QVector<ItemType_> &vec_, Type_ &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    template<typename Type_> inline
    void toAttribute(const QString &val_, std::vector<Type_> &)
    {
        Q_UNUSED(val_);
        Q_ASSERT(false); // not implemented
    }

    template<typename Type_> inline
    void vecToAttribute(const QVector<float> &vec_, std::vector<Type_> &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    namespace details
    {

        template<typename Type_> inline
        bool getArray(const QString &str_, std::vector<Type_> &array_)
        {
            QString str(str_);
            QTextStream s(&str);
            std::size_t i = 0;
            for(i = 0; i <= array_.size() && !s.atEnd(); ++i)
            {
                if(i == array_.size())
                {
                    Type_ v{};
                    s >> v;
                    array_.push_back(v);
                }
                else
                {
                    s >> array_.at(i);
                }
            }
            return i >= array_.size();
        }

        template<std::size_t size_, typename MatrixType_> inline
        void toMatrix(const QString &str_, MatrixType_ &mat_)
        {
            std::vector<float> vec;
            vec.resize(size_);
            (void)getArray(str_, vec);
            for(std::size_t i = 0; i < vec.size() && i < size_; ++i)
            {
                mat_.data()[i] = vec.at(i);
            }
        }

        template<int size_, typename MatrixType_> inline
        void vecToMatrix(const QVector<float> &vec_, MatrixType_ &mat_)
        {
            for(int i = 0; i < vec_.size() && i < size_; ++i)
            {
                mat_.data()[i] = vec_.at(i);
            }
        }

        template<std::size_t size_, typename Type_> inline
        std::vector<Type_> getArrayOfSize(const QString &str_)
        {
            std::vector<Type_> vec;
            vec.resize(size_);
            (void)details::getArray(str_, vec);
            return vec;
        }

        template<typename Type_> inline
        std::vector<Type_> getFullArray(const QString &str_)
        {
            std::vector<Type_> vec;
            vec.resize(0);
            (void)details::getArray(str_, vec);
            return vec;
        }

        template<typename Type_, typename ItemType_> inline
        Type_ toType(const std::vector<ItemType_> &vec_, std::size_t i0_)
        {
            Q_UNUSED(vec_);
            Q_UNUSED(i0_);
            Q_ASSERT(false); // not implemented
            return Type_{};
        }

        template<> inline
        int toType(const std::vector<int> &vec_, std::size_t i0_)
        {
            return vec_.at(i0_ + 0);
        }

        template<> inline
        float toType(const std::vector<float> &vec_, std::size_t i0_)
        {
            return vec_.at(i0_ + 0);
        }

        template<> inline
        QVector2D toType(const std::vector<float> &vec_, std::size_t i0_)
        {
            return {vec_.at(i0_ + 0), vec_.at(i0_ + 1)};
        }

        template<> inline
        QVector3D toType(const std::vector<float> &vec_, std::size_t i0_)
        {
            return {vec_.at(i0_ + 0), vec_.at(i0_ + 1), vec_.at(i0_ + 2)};
        }

        template<> inline
        QVector4D toType(const std::vector<float> &vec_, std::size_t i0_)
        {
            return {vec_.at(i0_ + 0), vec_.at(i0_ + 1), vec_.at(i0_ + 2), vec_.at(i0_ + 3)};
        }

        template<std::size_t size_, typename Type_> inline
        void toAttribute(const QString &str_, std::vector<Type_> &data_)
        {
            std::vector<float> vec = details::getFullArray<float>(str_);
            for(std::size_t i = 0; i + size_ <= vec.size(); i += size_)
            {
                data_.push_back(details::toType<Type_, float>(vec, i));
            }
        }

        template<std::size_t size_, typename Type_> inline
        void vecToAttribute(const std::vector<float> &vec_, std::vector<Type_> &data_)
        {
            for(int i = 0; i + size_ <= vec_.size(); i += size_)
            {
                data_.push_back(details::toType<Type_, float>(vec_, i));
            }
        }

        template<typename Type_>
        std::vector<Type_> toStdVector(const QVector<Type_> &vec_)
        {
            std::vector<Type_> result;
            result.reserve(vec_.size());
            std::copy(std::begin(vec_), std::end(vec_),
                      std::back_inserter(result));
            return result;
        }

    }

    template<> inline
    void toUniform(const QString &str_, int &data_)
    {
        data_ = details::toType<int, int>(details::getArrayOfSize<1, int>(str_), 0);
    }

    template<> inline
    void toUniform(const QString &str_, float &data_)
    {
        data_ = details::toType<float, float>(details::getArrayOfSize<1, float>(str_), 0);
    }

    template<> inline
    void toUniform(const QString &str_, QVector2D &data_)
    {
        data_ = details::toType<QVector2D, float>(details::getArrayOfSize<2, float>(str_), 0);
    }

    template<> inline
    void toUniform(const QString &str_, QVector3D &data_)
    {
        data_ = details::toType<QVector3D, float>(details::getArrayOfSize<3, float>(str_), 0);
    }

    template<> inline
    void toUniform(const QString &str_, QVector4D &data_)
    {
        data_ = details::toType<QVector4D, float>(details::getArrayOfSize<4, float>(str_), 0);
    }

    template<> inline
    void toUniform(const QString &str_, QMatrix2x2 &mat_)
    {
        details::toMatrix<2*2, QMatrix2x2>(str_, mat_);
    }

    template<> inline
    void toUniform(const QString &str_, QMatrix3x3 &mat_)
    {
        details::toMatrix<3*3, QMatrix3x3>(str_, mat_);
    }

    template<> inline
    void toUniform(const QString &str_, QMatrix4x4 &mat_)
    {
        details::toMatrix<4*4, QMatrix4x4>(str_, mat_);
    }

    template<> inline
    void vecToUniform(const QVector<int> &vec_, int &data_)
    {
        data_ = details::toType<int, int>(details::toStdVector(vec_), 0);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, float &data_)
    {
        data_ = details::toType<float, float>(details::toStdVector(vec_), 0);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QVector2D &data_)
    {
        data_ = details::toType<QVector2D, float>(details::toStdVector(vec_), 0);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QVector3D &data_)
    {
        data_ = details::toType<QVector3D, float>(details::toStdVector(vec_), 0);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QVector4D &data_)
    {
        data_ = details::toType<QVector4D, float>(details::toStdVector(vec_), 0);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QMatrix2x2 &mat_)
    {
        details::vecToMatrix<2*2, QMatrix2x2>(vec_, mat_);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QMatrix3x3 &mat_)
    {
        details::vecToMatrix<3*3, QMatrix3x3>(vec_, mat_);
    }

    template<> inline
    void vecToUniform(const QVector<float> &vec_, QMatrix4x4 &mat_)
    {
        details::vecToMatrix<4*4, QMatrix4x4>(vec_, mat_);
    }

    template<> inline
    void toAttribute(const QString &str_, std::vector<QVector2D> &data_)
    {
        details::toAttribute<2, QVector2D>(str_, data_);
    }

    template<> inline
    void toAttribute(const QString &str_, std::vector<QVector3D> &data_)
    {
        details::toAttribute<3, QVector3D>(str_, data_);
    }

    template<> inline
    void toAttribute(const QString &str_, std::vector<QVector4D> &data_)
    {
        details::toAttribute<4, QVector4D>(str_, data_);
    }

    template<> inline
    void vecToAttribute(const QVector<float> &vec_, std::vector<QVector2D> &data_)
    {
        details::vecToAttribute<2, QVector2D>(details::toStdVector(vec_), data_);
    }

    template<> inline
    void vecToAttribute(const QVector<float> &vec_, std::vector<QVector3D> &data_)
    {
        details::vecToAttribute<3, QVector3D>(details::toStdVector(vec_), data_);
    }

    template<> inline
    void vecToAttribute(const QVector<float> &vec_, std::vector<QVector4D> &data_)
    {
        details::vecToAttribute<4, QVector4D>(details::toStdVector(vec_), data_);
    }
}

class IFileSource
{
public:
    virtual ~IFileSource() = default;
    virtual QString getText(const FileSource &filename_) const = 0;
    virtual QImage getImage(const FileSource &filename_) const = 0;
};

class IVariables
{
public:
    virtual ~IVariables() = default;
    virtual bool get(const QString &name_, QVector<int> &data_) const = 0;
    virtual bool get(const QString &name_, QVector<float> &data_) const = 0;
};

class Variables : public IVariables
{
    struct Variable
    {
    public:
        Variable() = default;
        void set(const QJsonArray &jsonArray_);
        void prepare(QVector<float> &);
        void prepare(QVector<int> &);
        void get(QVector<float> &data_) const;
        void get(QVector<int> &data_) const;

    private:
        QJsonArray m_jsonArray;
        QVector<float> m_floatData;
        QVector<int> m_intData;
    };

public:
    Variables() = default;

    bool get(const QString &name_, QVector<int> &data_) const override;
    bool get(const QString &name_, QVector<float> &data_) const override;

private:
    std::map<QString, Variable> m_variables;
};

class Details
{
public:
    Details();

    std::shared_ptr<IFileSource> filesource;
    std::shared_ptr<IVariables> variables;
};

}


#endif // DRAWINGDATA_UTILS_H
