#ifndef TEST_UTILS_H
#define TEST_UTILS_H


#include <QString>


inline
bool equal(
        const QString &result_,
        const QString &stringsSum_
        )
{
    return result_ == stringsSum_;
}

template<typename Arg_> inline
QString sum(Arg_ arg_)
{
    return QString(arg_);
}

template<typename Arg_, typename ... Args_> inline
QString sum(Arg_ arg_, Args_ ... args_)
{
    return QString(arg_) + sum(args_ ...);
}


#endif // TEST_UTILS_H
