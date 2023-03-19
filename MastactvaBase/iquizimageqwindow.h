#ifndef IQUIZIMAGEQWINDOW_H
#define IQUIZIMAGEQWINDOW_H


#include <QString>
#include <vector>
#include <set>
#include <map>
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


class IQuizImageQWindowOperations
{
public:
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

private:
    std::shared_ptr<drawing_data::QuizImageObjects> m_drawingData;
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
    static IQuizImageQWindowOperations* findQuizImageWindows(int windowsId_)
    {
        const IQuizImageQWindow* inst = getInstance();
        auto fit = inst->m_quizImageWindows.find(windowsId_);
        return std::end(inst->m_quizImageWindows) == fit ? nullptr : fit->second;
    }


    static void addQuizImageWindows(IQuizImageQWindowOperations* windowsOperation_)
    {
        IQuizImageQWindow* inst = getInstance();
        if(!inst)
        {
            return;
        }
        const int id = inst->createRenderingWindowsId();
        inst->m_quizImageWindows.insert(std::map<int, IQuizImageQWindowOperations*>::value_type{id, windowsOperation_});
    }

    static void removeQuizImageWindows(IQuizImageQWindowOperations* windowsOperation_)
    {
        IQuizImageQWindow* inst = getInstance();
        auto fit = std::find_if(std::begin(inst->m_quizImageWindows), std::end(inst->m_quizImageWindows),
                     [windowsOperation_](const std::map<int, IQuizImageQWindowOperations*>::value_type &it)
        {
            return it.second == windowsOperation_;
        });
        if(std::end(inst->m_quizImageWindows) == fit)
        {
            return;
        }
        inst->removeRenderingWindowsId(fit->first);
        inst->m_quizImageWindows.erase(fit);
    }

    static bool findWindowsId(IQuizImageQWindowOperations* windowsOperation_)
    {
        const IQuizImageQWindow* inst = getInstance();
        return getWindowsId(windowsOperation_) != inst->m_lastWindowsId;
    }

    static int getWindowsId(IQuizImageQWindowOperations* windowsOperation_)
    {
        const IQuizImageQWindow* inst = getInstance();
        auto fit = std::find_if(std::begin(inst->m_quizImageWindows), std::end(inst->m_quizImageWindows),
                     [windowsOperation_](const std::map<int, IQuizImageQWindowOperations*>::value_type &it)
        {
            return it.second == windowsOperation_;
        });
        return std::end(inst->m_quizImageWindows) == fit? inst->m_lastWindowsId : fit->first;
    }

private:
    static IQuizImageQWindow * getInstance()
    {
        static IQuizImageQWindow inst;
        return &inst;
    }

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

private:
    int m_lastWindowsId = 0;
    std::set<int> m_windowsIds;
    std::map<int, IQuizImageQWindowOperations*> m_quizImageWindows;
};


#endif // IQUIZIMAGEQWINDOW_H
