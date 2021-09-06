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

#ifndef LOCALDATA_H
#define LOCALDATA_H


#include "../MastactvaBase/localdataapinocache.h"
#include "../MastactvaBase/localdataapicache.h"


#if defined(LOCALDATAAPICACHE)
using LocalDataAPI = LocalDataAPICache;
#else
using LocalDataAPI = LocalDataAPINoCache;
#endif
/*
 * NB
 *
 * please use following:
 *
 * class LocalDataAPINoCache;
 * using LocalDataAPI = LocalDataAPINoCache;
 *
 * not this:
 *
 * class LocalDataAPI;
 *
*/


#endif // LOCALDATA_H
