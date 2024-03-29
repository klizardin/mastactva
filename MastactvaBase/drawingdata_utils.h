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

#ifndef DRAWINGDATA_UTILS_H
#define DRAWINGDATA_UTILS_H


#include <map>
#include <memory>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/data_utils.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/addonmodules.h"


namespace drawingdata
{

namespace utils
{
    /*
     * to specify constrains of creation of a drawing data type from a data type
    */
    template<class DrawingDataType_, class DataType_> inline
    std::unique_ptr<DrawingDataType_> factory(DataType_ &&data_, const DrawingDataType_ *)
    {
        return std::make_unique<DrawingDataType_>(std::move(data_));
    }

    /*
     * rebuild items of the vector of the data types into the vector of the drawing data type
    */
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
                // note: no need to delete ptr_ because we already moved data from the *ptr_
                // just replace ptr_ with new pointer if any
                if(ptr)
                {
                    ptr_ = static_cast<DataType_ *>(ptr.release());
                }
                else
                {
                    ptr_ = nullptr;
                }
            }
        }
    }

    /*
     * version of rebuild with creation of the new vector
     * it is a copy of the original, not implace operations
    */
    template<class DataType_, class DrawingDataType_> inline
    void rebuild(
        const std::shared_ptr<QVector<DataType_ *>> &data_,
        std::shared_ptr<QVector<DrawingDataType_ *>> &result_
        )
    {
        result_ = data_object::utils::createDataVector(static_cast<const DrawingDataType_ *>(nullptr));
        if(data_.operator bool())
        {
            for(DataType_ *& ptr_ : *data_)
            {
                if(!ptr_)
                {
                    continue;
                }
                auto ptr = factory<DrawingDataType_, DataType_>(std::move(*ptr_), nullptr);
                result_->push_back(ptr.release());
                ptr_ = nullptr; // clear source vector pointers
            }
        }
    }

    /*
     * create copy of data vector
    */
    template<class DataTypeSrc_, class DataTypeDest_> inline
    void copy(
        const std::shared_ptr<QVector<DataTypeSrc_ *>> &data_,
        std::shared_ptr<QVector<DataTypeDest_ *>> &result_
        )
    {
        result_ = data_object::utils::createDataVector(static_cast<const DataTypeDest_ *>(nullptr));
        if(data_.operator bool())
        {
            for(DataTypeSrc_ *& ptr_ : *data_)
            {
                if(!ptr_)
                {
                    continue;
                }
                auto ptr = std::make_unique<DataTypeDest_>(static_cast<const DataTypeDest_ &>(*ptr_));
                result_->push_back(ptr.release());
            }
        }
    }

    /*
     * base function to convert string to uniform type
     * it is stub for unused types
    */
    template<typename Type_> inline
    void toUniform(const QString &str_, Type_ &)
    {
        Q_UNUSED(str_);
        Q_ASSERT(false); // not implemented
    }

    /*
     * base function to convert vector of items to uniform type
     * it is stub for unused types
    */
    template<typename Type_, typename ItemType_> inline
    void vecToUniform(const QVector<ItemType_> &vec_, Type_ &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    /*
     * base function to convert string to attribute type
     * it is stub for unused types
    */
    template<typename Type_> inline
    void toAttribute(const QString &val_, std::vector<Type_> &)
    {
        Q_UNUSED(val_);
        Q_ASSERT(false); // not implemented
    }

    /*
     * base function to convert vector of float to attribute type
     * it is stub for unused types
    */
    template<typename Type_> inline
    void vecToAttribute(const QVector<float> &vec_, std::vector<Type_> &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    /*
     * base function to convert vector of double to attribute type
     * it is stub for unused types
    */
    template<typename Type_> inline
    void vecToAttribute(const QVector<double> &vec_, std::vector<Type_> &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    /*
     * base function to convert vector of float to attribute type
     * it is stub for unused types
    */
    template<typename Type_> inline
    void vecToAttribute(const std::vector<float> &vec_, std::vector<Type_> &)
    {
        Q_UNUSED(vec_);
        Q_ASSERT(false); // not implemented
    }

    namespace details
    {
        /*
         * convert string to the array of type
        */
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

        template<typename Type_> inline
        std::vector<Type_> toStdVector(const QVector<Type_> &vec_)
        {
            std::vector<Type_> result;
            result.reserve(vec_.size());
            std::copy(std::begin(vec_), std::end(vec_),
                      std::back_inserter(result));
            return result;
        }

        inline
        std::vector<float> toStdVector(const QVector<double> &vec_)
        {
            std::vector<float> result;
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

    template<> inline
    void vecToAttribute(const QVector<double> &vec_, std::vector<QVector2D> &data_)
    {
        details::vecToAttribute<2, QVector2D>(details::toStdVector(vec_), data_);
    }

    template<> inline
    void vecToAttribute(const QVector<double> &vec_, std::vector<QVector3D> &data_)
    {
        details::vecToAttribute<3, QVector3D>(details::toStdVector(vec_), data_);
    }

    template<> inline
    void vecToAttribute(const QVector<double> &vec_, std::vector<QVector4D> &data_)
    {
        details::vecToAttribute<4, QVector4D>(details::toStdVector(vec_), data_);
    }

    template<> inline
    void vecToAttribute(const std::vector<float> &vec_, std::vector<QVector2D> &data_)
    {
        details::vecToAttribute<2, QVector2D>(vec_, data_);
    }

    template<> inline
    void vecToAttribute(const std::vector<float> &vec_, std::vector<QVector3D> &data_)
    {
        details::vecToAttribute<3, QVector3D>(vec_, data_);
    }

    template<> inline
    void vecToAttribute(const std::vector<float> &vec_, std::vector<QVector4D> &data_)
    {
        details::vecToAttribute<4, QVector4D>(vec_, data_);
    }

    template<typename Type_> inline
    void toVec(const QString &val_, QVector<Type_> &vec_)
    {
        std::vector<Type_> vec;
        details::getArray(val_, vec);
        vec_.clear();
        vec_.reserve(vec.size());
        for(const Type_ &val_: vec)
        {
            vec_.push_back(val_);
        }
    }

    inline
    void addStates(const QStringList &states_, std::vector<QString> &objectStates_)
    {
        objectStates_.reserve(objectStates_.size() + states_.size());
        for(const QString &state_ : states_)
        {
            objectStates_.push_back(state_);
        }
    }

    inline
    void splitTo(const QString &argument_, const QString &splitter_, QStringList &objectStates_)
    {
        objectStates_ = argument_.split(splitter_);
    }

    inline
    QStringList splitTo(const QString &argument_, const QString &splitter_, std::vector<QString> &objectStates_)
    {
        QStringList splitted = argument_.split(splitter_);
        addStates(splitted, objectStates_);
        return splitted;
    }
}


/*
 * Dependency Injection Interface of texts and images for specific file source path
*/
class IFileSource
{
public:
    virtual ~IFileSource() = default;
    virtual QString getText(const FileSource &filename_) const = 0; // return text by file name
    virtual QImage getImage(const FileSource &filename_) const = 0; // return image by file name
};


class IPosition
{
public:
    virtual ~IPosition() = default;
    virtual void setObject(const QString &name_, int objectStepIndex_, int objectNameIndex_) = 0;     // set object name
    virtual void resetArtefactStepIndex(int stepIndex_) = 0;                    // reset artefact step index
                                                                                // do not need check for current step index >= the last one
    virtual void setArtefactStepIndex(int stepIndex_) = 0;                      // set artefact step index
                                                                                // check that the current step index >= the last step index
    virtual const QString &getObjectName() const = 0;                           // return the current object name
    virtual bool hasObjectName() const { return !getObjectName().isEmpty(); };  // return true if we have the object name
    virtual int getObjectNameIndex() const = 0;                                 // return object name index
    virtual bool hasObjectNameIndex() const { return true; }                    // always true
    virtual int getObjectStepIndex() const = 0;                                 // return object step index
    virtual bool hasObjectStepIndex() const = 0;                                // return true if the object's step index is initialized
    virtual int getArtefactStepIndex() const = 0;                               // return artefact's step index
    virtual bool hasArtefactStepIndex() const = 0;                              // return true if the artefact's step index is initialized
    virtual void clear() = 0;                                                   // clear position. fields are marked as uninitialized
    virtual std::unique_ptr<IPosition> getCopyClearObjectIndex() const = 0;     // return a copy of the current position with object's step
                                                                                // index unitialized
    virtual std::unique_ptr<IPosition> getCopy() const = 0;                     // return a copy of the current position
};


class IVariables
{
public:
    virtual ~IVariables() = default;

    /*
     * get variable value
    */
    virtual bool get(const QString &name_, const IPosition *position_, QVector<int> &data_) const = 0;
    virtual bool get(const QString &name_, const IPosition *position_, QVector<float> &data_) const = 0;
    virtual bool get(const QString &name_, const IPosition *position_, QVector<double> &data_) const = 0;
    virtual bool get(const QString &name_, const IPosition *position_, QStringList &data_) const = 0;

    /*
     * add variable value
    */
    virtual void add(const QJsonDocument &data_) = 0;
    virtual bool add(const QString &name_, const IPosition *position_, const QVector<double> &data_) = 0;
    virtual bool add(const QString &name_, const IPosition *position_, QVector<double> &&data_) = 0;
    virtual bool add(const QString &name_, const IPosition *position_, const QStringList &data_) = 0;
    virtual bool add(const QString &name_, const IPosition *position_, QStringList &&data_) = 0;

    /*
     * add aliases of the variables
    */
    virtual void addAliases(const QJsonDocument &data_, const IPosition *position_) = 0;

    /*
     * process special variable objectsToRun, see g_jsonDataVariableNameObjectListName
    */
    virtual bool getObjectsList(QStringList &objects_) const = 0;

    /*
     * clear
    */
    virtual void clear() = 0;

    /*
     * helper functions to process several variables at the same time
    */
    bool add(const IPosition *position_, const std::map<QString, QVector<double>> &variables_);
    bool add(const IPosition *position_, std::map<QString, QVector<double>> &&variables_);
    bool add(const IPosition *position_, const std::map<QString, QStringList> &variables_);
    bool add(const IPosition *position_, std::map<QString, QStringList> &&variables_);

    /*
     * helper functions to process variables with explicite positions
    */
    bool add(const QString &name_, const QString &objectName_, int objectIndex, int objectNameIndex_, int artefactIndex_, const QVector<double> &data_);
    bool add(const QString &name_, const QString &objectName_, int objectIndex, int objectNameIndex_, int artefactIndex_, QVector<double> &&data_);
    bool add(const QString &name_, const QString &objectName_, int objectIndex, int objectNameIndex_, int artefactIndex_, const QStringList &data_);
    bool add(const QString &name_, const QString &objectName_, int objectIndex, int objectNameIndex_, int artefactIndex_, QStringList &&data_);

private:
    template<typename DataType_>
    bool addT(const IPosition *position_, const std::map<QString, DataType_> &variables_);
    template<typename DataType_>
    bool addT(const IPosition *position_, std::map<QString, DataType_> &&variables_);
};


template<typename DataType_> inline
bool IVariables::addT(const IPosition *position_, const std::map<QString, DataType_> &variables_)
{
    bool result = false;
    for(const auto &var_ : variables_)
    {
        result |= add(var_.first, position_, var_.second);
    }
    return result;
}

template<typename DataType_> inline
bool IVariables::addT(const IPosition *position_, std::map<QString, DataType_> &&variables_)
{
    bool result = false;
    for(auto &var_ : variables_)
    {
        result |= add(var_.first, position_, std::move(var_.second));
    }
    return result;
}

inline
bool IVariables::add(const IPosition *position_, const std::map<QString, QVector<double>> &variables_)
{
    return addT(position_, variables_);
}

inline
bool IVariables::add(const IPosition *position_, std::map<QString, QVector<double>> &&variables_)
{
    return addT(position_, std::move(variables_));
}

inline
bool IVariables::add(const IPosition *position_, const std::map<QString, QStringList> &variables_)
{
    return addT(position_, variables_);
}

inline
bool IVariables::add(const IPosition *position_, std::map<QString, QStringList> &&variables_)
{
    return addT(position_, std::move(variables_));
}


namespace details
{

/*
 * position for the variable
*/
struct VariablePosition
{
    VariablePosition() = default;

    static VariablePosition fromJson(const QJsonObject &position_);     // convert from json
    static VariablePosition fromCurrent(const IPosition *position_);    // convert from IPosition

private:
    /*
     * convert from explicite values
    */
    static VariablePosition fromInfo(const QString &objectName, int objectStepIndex_, int objectNameIndex_, int artefactStepIndex_);

private:
    std::tuple<QString, bool> objectName = std::make_tuple(QString{}, false);
    std::tuple<int, bool> objectNameIndex = std::make_tuple(0, false);
    std::tuple<int, bool> objectStepIndex = std::make_tuple(0, false);
    std::tuple<int, bool> artefactStepIndex = std::make_tuple(0, false);

    friend bool operator == (const VariablePosition &left_, const VariablePosition &right_);    // to find position
    friend class ::drawingdata::IVariables;
};


/*
 * datas for the variable
*/
class ValiableData
{
public:
    ValiableData() = default;
    void set(const QJsonArray &jsonArray_);         // set up from json data - from artefact's json data
    void set(const QVector<double> &data_);         // set up as vector of doubles
    void set(QVector<double> &&data_);              // set up as vector of doubles
    void set(const QStringList &data_);             // set up as vector of strings (for string types variables)
    void set(QStringList &&data_);                  // set up as vector of strings (for string types variables)
    void prepare(QVector<float> &);                 // prepare to get as vector of floats
    void prepare(QVector<int> &);                   // prepere to get at vector of ints
    void prepare(QVector<double> &);                // prepare to get as vector of doubles
    void prepare(QStringList &);                    // prepare to get as vector of strings
    void get(QVector<float> &data_) const;          // get as vector of floats
    void get(QVector<int> &data_) const;            // get as vector of int
    void get(QVector<double> &data_) const;         // get as vector of double
    void get(QStringList &data_) const;             // get as vector of strings

private:
    QJsonArray m_jsonArray;             // represent as a json array
    QVector<double> m_doubleData;       // as vector of doubles
    QVector<float> m_floatData;         // as vector of floats
    QVector<int> m_intData;             // as vector of ints
    QStringList m_stringsList;          // as vector of strings
};


/*
 * the variable's class
*/
struct Variable
{
public:
    Variable() = default;
    /*
     * representation of VariableData methods
     * {
    */
    void set(const QJsonArray &jsonArray_);
    void set(const QVector<double> &data_);
    void set(QVector<double> &&data_);
    void set(const QStringList &data_);
    void set(QStringList &&data_);
    void setAlias(const Variable &var_);
    void setPosition(const QJsonObject &position_);
    void setPosition(const IPosition *position_);
    void prepare(QVector<float> &) const;
    void prepare(QVector<int> &) const;
    void prepare(QVector<double> &) const;
    void prepare(QStringList &) const;
    void get(QVector<float> &data_) const;
    void get(QVector<int> &data_) const;
    void get(QVector<double> &data_) const;
    void get(QStringList &data_) const;
    // }

    bool match(const VariablePosition &pos_) const;     // check if the position of the variable matches the VariablePosition

private:
    std::shared_ptr<ValiableData> m_data = std::make_shared<ValiableData>();
    VariablePosition m_position;
};


/*
 * class of a name of variable
*/
struct VariableName
{
    VariableName(const QString &name_ = QString(),int index_ = 0, bool hasIndex_ = true);

    QString name;               // name of a variable
    int index = 0;              // is used to order variables by it's life longness
    bool hasIndex = true;

    friend bool operator == (const VariableName &left_, const VariableName &right_);
    friend bool operator < (const VariableName &left_, const VariableName &right_);
};

bool operator == (const VariableName &left_, const VariableName &right_);
bool operator < (const VariableName &left_, const VariableName &right_);

}


/*
 * implementation of IVariables interface
*/
class Variables : public IVariables
{
    using VariablesMap = std::multimap<details::VariableName, details::Variable>;

public:
    Variables() = default;

    bool get(const QString &name_, const IPosition *position_, QVector<int> &data_) const override;
    bool get(const QString &name_, const IPosition *position_, QVector<float> &data_) const override;
    bool get(const QString &name_, const IPosition *position_, QVector<double> &data_) const override;
    bool get(const QString &name_, const IPosition *position_, QStringList &data_) const override;
    void add(const QJsonDocument &data_) override;
    bool add(const QString &name_, const IPosition *position_, const QVector<double> &data_) override;
    bool add(const QString &name_, const IPosition *position_, QVector<double> &&data_) override;
    bool add(const QString &name_, const IPosition *position_, const QStringList &data_) override;
    bool add(const QString &name_, const IPosition *position_, QStringList &&data_) override;
    void addAliases(const QJsonDocument &data_, const IPosition *position_) override;
    bool getObjectsList(QStringList &objects_) const override;
    void clear() override;

private:
    bool find(const QString &name_, const IPosition *position_, VariablesMap::const_iterator &fit) const;
    void setObjectsList(const QJsonArray &array_);
    void setObjectsList(const QStringList &list_);
    void setObjectsList(QStringList &&list_);
    void addVariables(
            const QJsonObject &rootObject_,
            const std::tuple<details::Variable, bool> &dataSource_
            );
    static std::tuple<QJsonValue, bool> getJsonValue(const QJsonObject &varObject);
    void addVariable(
            const details::Variable &newVarTempl_,
            const QString &name_,
            const QJsonObject &position_
            );
    void addVariables(
            const details::Variable &newVarTempl_,
            const QString &name_,
            const QJsonArray &positions_
            );
    template<class Data_>
    bool addT(const QString &name_, const IPosition *position_, Data_ &&data_);

private:
    VariablesMap m_variables;
    int index = std::numeric_limits<decltype (index)>::min();
    QStringList m_objects;
    bool m_hasObjectsList = false;
};


/*
 * implementation of IPosition interface
*/
class Position : public IPosition
{
public:
    Position() = default;

    void setObject(const QString &name_, int objectStepIndex_, int objectNameIndex_) override;
    void resetArtefactStepIndex(int stepIndex_) override;
    void setArtefactStepIndex(int stepIndex_) override;
    const QString &getObjectName() const override;
    int getObjectNameIndex() const override;
    int getObjectStepIndex() const override;
    bool hasObjectStepIndex() const override;
    int getArtefactStepIndex() const override;
    bool hasArtefactStepIndex() const override;
    void clear() override;
    std::unique_ptr<IPosition> getCopyClearObjectIndex() const override;
    std::unique_ptr<IPosition> getCopy() const  override;
    static Position fromPosition(IPosition *pos_);
    static Position fromInfo(const QString &name_, int objectStepIndex_, int objectNameIndex_, int artefactStepIndex_);

private:
    QString objectName;
    int objectNameIndex = 0;
    int objectStepIndex = std::numeric_limits<decltype (objectStepIndex)>::max();
    int artefactStepIndex = std::numeric_limits<decltype (artefactStepIndex)>::max();
};


/*
 * drawing data detail class - data that is shared between steps
*/
class Details
{
public:
    Details();

    std::shared_ptr<IFileSource> filesource;
    std::shared_ptr<IVariables> variables;
    std::shared_ptr<IPosition> position;
    std::shared_ptr<IAddonModules> modules;
    std::shared_ptr<QStringList> textureTargets;

    void clear();
};


/*
 * interface for the textures support
*/
class ITextures
{
public:
    virtual ~ITextures() = default;
    virtual void setTexture(const QString &textureName_, const QString &newFilename_) = 0;
    virtual void setTexture(const QString &textureName_, const QString &newFilename_, const QColor &newBackgroundColor_) = 0;
    virtual void setTextureFromCurrentFrameBuffer(const QString &textureName_) = 0;
    virtual void setTextureFromCurrentFrameBuffer(const QString &textureName_, const QColor &backgroundColor_) = 0;
    virtual void setTextureFromSharedTextureId(const QString &textureName_) = 0;
    virtual void setTextureFromSharedTextureId(const QString &textureName_, const QColor &backgroundColor_) = 0;
    //virtual void setTextureFromCurrentFrameBufferSurface(const QString &textureName_) = 0;
};


}


#endif // DRAWINGDATA_UTILS_H
