#ifndef IQUIZIMAGEQWINDOW_H
#define IQUIZIMAGEQWINDOW_H


#include <QString>
#include <vector>
#include <set>
#include "../MastactvaBase/quizimagedrawingdata.h"


class TextureNames : public std::vector<QString>
{
public:
    TextureNames(const std::vector<QString> &textureNames_);

private:
    bool hasDefaultTextureName() const;
    void addDefaultTextureName();

public:
    static bool isDefaultTexcture(const QString &textureName_);
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
    int createRenderingWindowsId()
    {
        for(;;++m_lastWindowsId)
        {
            if(m_windowsIds.find(m_lastWindowsId) == std::end(m_windowsIds))
            {
                break;
            }
        }
        const int result = m_lastWindowsId;
        m_windowsIds.insert(result);
        ++m_lastWindowsId;
        return result;
    }

    void removeRenderingWindowsId(int id_)
    {
        auto fit = m_windowsIds.find(id_);
        m_windowsIds.erase(fit);
    }

    // virtual void addQuizImageWindows(IQuizImageQWindow* windows_);
    // virtual void removeQuizImageWindows(IQuizImageQWindow* windows_);
    // virtual IQuizImageQWindow* findQuizImageWindows(int windowsId_) const;
    virtual void setTextures(const TextureNames & textures) = 0;
    virtual int count() const = 0;
    virtual QString at(int index) const = 0;
    virtual bool isDefaultTexture(int index) const = 0;

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
    int m_lastWindowsId = 0;
    std::set<int> m_windowsIds;
};


#endif // IQUIZIMAGEQWINDOW_H
