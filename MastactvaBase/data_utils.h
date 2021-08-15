#ifndef DATA_UTILS_H
#define DATA_UTILS_H


#include <memory>
#include <QVector>


namespace data_object
{
namespace utils
{
    template<class DataType_> inline
    std::shared_ptr<QVector<DataType_ *>> createDataVector(const DataType_ *)
    {
        return std::shared_ptr<QVector<DataType_ *>>(
             new QVector<DataType_ *>(),
             [](QVector<DataType_ *> *ptr_)->void
        {
            if(nullptr == ptr_)
            {
                return;
            }
            for(DataType_ *& p_: *ptr_)
            {
                delete p_;
                p_ = nullptr;
            }
            ptr_->clear();
            delete ptr_;
        });
    }

    template<typename DataType_> inline
    void copyDataVector(const QVector<DataType_ *> *src_, QVector<DataType_ *> *dest_)
    {
        Q_ASSERT(nullptr != dest_);
        if(nullptr == src_ || nullptr == dest_)
        {
            return;
        }
        for(const DataType_ *elem_ : *src_)
        {
            if(nullptr == elem_)
            {
                continue;
            }
            dest_->push_back(elem_->getDataCopy().release());
        }
    }

}
}


#endif // DATA_UTILS_H
