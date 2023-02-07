#ifndef IQUIZIMAGEQWINDOW_H
#define IQUIZIMAGEQWINDOW_H


#include <QString>
#include <vector>


class IQuizImageQWindow
{
public:
    virtual void add(const std::vector<QString> & textures) = 0;
    virtual int count() const = 0;
    virtual QString at(int index) const = 0;

public:
    static IQuizImageQWindow * getInstance()
    {
        return nullptr;
    }
};


#endif // IQUIZIMAGEQWINDOW_H
