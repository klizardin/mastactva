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

#ifndef LAYOUT_ENUMS_H
#define LAYOUT_ENUMS_H


namespace layout
{
    enum class JsonTypesEn : int
    {
        jt_null,
        jt_bool,
        jt_int,
        jt_double,
        jt_string,
        jt_datetime,
        jt_array,
        jt_object,
        jt_undefined
    };

    enum class SpecialFieldEn : int
    {
        none,
        appId,
        modelInfo,
        objectModelInfo
    };
}


#endif // LAYOUT_ENUMS_H
