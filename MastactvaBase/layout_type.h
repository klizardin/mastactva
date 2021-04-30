#ifndef LAYOUT_TYPE_H
#define LAYOUT_TYPE_H


#include <QString>


namespace layout
{
    template<typename T_>
    inline int compareValues(const T_ &val1_, const T_ &val2_, bool toQML_ = true)
    {
        Q_UNUSED(toQML_);
        return val1_ < val2_ ? -1 : val2_ < val1_  ? 1 : 0;
    }
}


class JsonName : protected QString
{
public:
    JsonName() = default;
    JsonName(const QString &name_)
        : QString(name_)
    {}

    bool isEmpty() const
    {
        return static_cast<const QString &>(*this).isEmpty();
    }
    const QString &toString() const { return *this; }

    friend bool operator == (const JsonName &left_, JsonName &right_);
    friend bool operator < (const JsonName &left_, JsonName &right_);
};

inline
bool operator == (const JsonName &left_, JsonName &right_)
{
    return static_cast<const QString &>(left_) == static_cast<const QString &>(right_);
}

inline
bool operator < (const JsonName &left_, JsonName &right_)
{
    return static_cast<const QString &>(left_) < static_cast<const QString &>(right_);
}


#endif // LAYOUT_TYPE_H
