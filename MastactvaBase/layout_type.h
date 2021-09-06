/*
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
        return static_cast<const QString &>(*this).trimmed().isEmpty();
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
