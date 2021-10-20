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

#ifndef DEFINES_H
#define DEFINES_H


// #define TRACE_MODEL_LIFETIME
// #define TRACE_MODEL_LOADING
// #define TRACE_MODEL_LOADED
// #define TRACE_LIST_LOAD_DATA
// #define TRACE_LIST_SIZE
// #define TRACE_LIST_DATA_ITEMS_CRUD
// #define TRACE_DB_CREATION
// #define TRACE_DB_REQUESTS
// #define TRACE_DB_DATA_BINDINGS
// #define TRACE_DB_USE
// #define TRACE_DB_DATA_RETURN
// #define TRACE_THREADS
// #define TRACE_THREADS_QUIZIMAGE
// #define TRACE_SERVER_FILES
// #define TRACE_EFFECT_OBJECT_POSITION
#define TRACE_MODELS_LOADING {              \
    "LocalData_Export_EffectArgModel",      \
    "LocalData_Export_EffectArgSetModel",   \
    "LocalData_Export_EffectArgValueModel"  \
    }                                       \
/* end TRACE_MODELS_LOADING */


#if defined(TRACE_THREADS) || defined(TRACE_THREADS_QUIZIMAGE)
#include <QThread>
#endif


#endif // DEFINES_H
