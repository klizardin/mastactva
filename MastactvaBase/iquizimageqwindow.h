#ifndef IQUIZIMAGEQWINDOW_H
#define IQUIZIMAGEQWINDOW_H


#include <QString>
#include <vector>


class IQuizImageQWindow
{
public:
    virtual void add(const std::vector<QString> & textures) = 0;
    virtual int count() const = 0;
};


#endif // IQUIZIMAGEQWINDOW_H
