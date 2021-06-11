#ifndef DRAWINGDATA_UTILS_H
#define DRAWINGDATA_UTILS_H


#include <memory>
#include <QVector>
#include <QImage>
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

    template<typename Type_> inline
    void toAttribute(const QString &val_, std::vector<Type_> &)
    {
        Q_UNUSED(val_);
        Q_ASSERT(false); // not implemented
    }

    template<typename Type_> inline
    bool getArray(const QString &str_, std::vector<Type_> &array_)
    {
        QString str(str_);
        QTextStream s(&str);
        std::size_t i = 0;
        for(i = 0; i < array_.size() && !s.atEnd(); ++i)
        {
            s >> array_.at(i);
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

    template<> inline
    void toUniform(const QString &str_, QMatrix2x2 &mat_)
    {
        toMatrix<2*2, QMatrix2x2>(str_, mat_);
    }

    template<> inline
    void toUniform(const QString &str_, QMatrix3x3 &mat_)
    {
        toMatrix<3*3, QMatrix3x3>(str_, mat_);
    }

    template<> inline
    void toUniform(const QString &str_, QMatrix4x4 &mat_)
    {
        toMatrix<4*4, QMatrix4x4>(str_, mat_);
    }

    template<> inline
    void toAttribute(const QString &val_, std::vector<QVector3D> &data_)
    {
        QString str(val_);
        QTextStream s(&str);
        for(std::size_t i = 0; !s.atEnd(); ++i)
        {
            std::array<float, 3> v;
            for(std::size_t j = 0; j < v.size(); ++j)
            {
                s >> v.at(j);
            }
            if(s.atEnd())
            {
                return;
            }
            QVector3D item(v.at(0), v.at(1), v.at(2));
            data_.push_back(item);
        }
    }
}

class IFileSource
{
public:
    virtual ~IFileSource() = default;
    virtual QString getText(const FileSource &filename_) const = 0;
    virtual QImage getImage(const FileSource &filename_) const = 0;
};

class Details
{
public:
    std::shared_ptr<IFileSource> filesource;
};

}


#endif // DRAWINGDATA_UTILS_H
