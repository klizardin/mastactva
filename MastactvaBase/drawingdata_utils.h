#ifndef DRAWINGDATA_UTILS_H
#define DRAWINGDATA_UTILS_H


#include <memory>
#include <QVector>


namespace drawingdata
{

class utils
{
public:
    template<class DataType_, class DrawingDataType_>
    static void rebuild(const std::shared_ptr<QVector<DataType_ *>> &data_, DrawingDataType_ *)
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
                ptr_ = ptr->getBase();
                (void)ptr.release();
            }
        }
    }
};

}


#endif // DRAWINGDATA_UTILS_H
