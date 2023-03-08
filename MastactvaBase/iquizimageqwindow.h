#ifndef IQUIZIMAGEQWINDOW_H
#define IQUIZIMAGEQWINDOW_H


#include <QString>
#include <vector>
#include "../MastactvaBase/quizimagedrawingdata.h"


class TextureNames : public std::vector<QString>
{
public:
    TextureNames(const std::vector<QString> &textureNames_);

private:
    bool hasDefaultTextureName() const;
    void addDefaultTextureName();
};


/*
 * interface to add textures
 * TODO: change to IQuizImageQWindows
 * add method to return first QuizImage
 * or flag that the current QuizImage is the first
*/
class IQuizImageQWindow
{
public:
    // TODO: add base texture pointer void*
    virtual void setTextures(const TextureNames & textures) = 0;
    virtual int count() const = 0;
    virtual QString at(int index) const = 0;

    virtual void setDrawingData(std::shared_ptr<drawing_data::QuizImageObjects> data)
    {
        m_drawingData = data;
    }

    virtual std::shared_ptr<drawing_data::QuizImageObjects> getDrawingData() const
    {
        return m_drawingData;
    }

public:
    static IQuizImageQWindow * getInstance();

private:
    std::shared_ptr<drawing_data::QuizImageObjects> m_drawingData;
};


#endif // IQUIZIMAGEQWINDOW_H
