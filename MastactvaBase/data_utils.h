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

#ifndef DATA_UTILS_H
#define DATA_UTILS_H


#include <memory>
#include <QVector>


/*
 * utilities classes to work with data vectors
 * vector of pointers of some data type has custom destructor
 * for the shared_ptr (unique_ptr?) holding which clear all
 * the items of the vector. The inplace smart pointers are not used
 * because of optimization
*/
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

    /*
     * the function that uses specific method (getDataCopy()) of elements
     * to create a copy of the vector of the data
    */
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
