#ifndef DRAWINGDATA_UTILS_H
#define DRAWINGDATA_UTILS_H


#include <memory>
#include <QVector>
#include <QImage>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"


namespace drawingdata
{

namespace utils
{
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
                auto ptr = std::make_unique<DrawingDataType_>(std::move(*ptr_));
                if(ptr.operator bool())
                {
                    ptr_ = ptr.release()->getData();
                }
                else
                {
                    ptr_ = nullptr;
                }
            }
        }
    }
}

class IFileSource
{
public:
    virtual ~IFileSource() = default;
    virtual QString getText(const FileSource &filename_) const;
    virtual QImage getImage(const FileSource &filename_) const;
};

}


#endif // DRAWINGDATA_UTILS_H
