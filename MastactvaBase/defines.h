#ifndef DEFINES_H
#define DEFINES_H


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


#if defined(TRACE_THREADS) || defined(TRACE_THREADS_QUIZIMAGE)
#include <QThread>
#endif


#endif // DEFINES_H
