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

#ifndef QUIZIMAGE_H
#define QUIZIMAGE_H


#include <memory>
#include <QtQuick/QQuickFramebufferObject>
//#include "../MastactvaBase/quickframebufferobjects.h"
#include "../MastactvaBase/quizimagedrawingdata.h"
#include "drawing_tests.h"
#include "object_tests.h"
#include <vector>


/*
 * list of tests
 * TODO: to the separate file
*/
using Tests = std::tuple<
    drawing_data::TestMinimalDrawQTLogoQuizImageObject,
    drawing_data::TestMinimal2PassDrawQTLogoQuizImageObject,
    drawing_data::Test0QuizImageObject,
    drawing_data::Test1QuizImageObject,
    drawing_data::Test2QuizImageObject,
    drawing_data::Test3QuizImageObject,
    drawing_data::Test4QuizImageObject,
    drawing_data::TestAlphaBlendingsDrawQTLogoQuizImageObject,
    drawing_objects::BaseTest,
    drawing_objects::MultipleObjectsTest,
    drawing_objects::DefaultTest,
    drawing_objects::DataTestBase,
    drawing_objects::DataTestPosition,
    drawing_objects::DataTestObjectsList,
    drawing_objects::DataTest3DObjectSwift,
    drawing_objects::DataTest3DObjectCube,
    drawing_objects::DataTestAlias,
    drawing_objects::LuaScriptTestNewVariable,
    drawing_objects::LuaScriptTestSetVariable,
    drawing_objects::LuaScriptTestRuntime,
    drawing_objects::LuaScriptArgTest0,
    drawing_objects::LuaScriptArgTest1,
    drawing_objects::ArgSetBaseTest,
    drawing_objects::AlphaBlendingMultipleObjectsTest,
    drawing_objects::WalkEffectTest,
    drawing_objects::WalkEffectDrawingBufferTest
    >;


/*
 * control that wraps the opengl effect drawing
*/
class QuizImage : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(QuizImageDemo)
public:
    QuizImage();

    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString fromImage READ fromImage WRITE setFromImage NOTIFY fromImageChanged)
    Q_PROPERTY(QString toImage READ toImage WRITE setToImage NOTIFY toImageChanged)
    Q_PROPERTY(QString projectFilename READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QString compilerLog READ log WRITE setLog NOTIFY logChanged)
    Q_PROPERTY(int testIndex READ testIndex WRITE setTestIndex NOTIFY testIndexChanged)
    Q_PROPERTY(QString renderingTextureName READ renderingTextureName WRITE setRenderingTextureName NOTIFY renderingTextureNameChanged)

    Q_INVOKABLE void updateState();
    Q_INVOKABLE void updateProject();

public:
    virtual QQuickFramebufferObject::Renderer *createRenderer() const override;

public:
    qreal t() const;
    bool isImageDataUpdated() const;
    std::shared_ptr<drawing_data::QuizImageObjects> getData();
    void setDataToFree(std::unique_ptr<drawing_data::QuizImageObjects> &&old_);
    void renderBuildError(const QString &compilerLog_);
    bool isFromImageReady() const;
    bool isToImageReady() const;
    const QString &getFromImageUrl() const;
    const QString &getToImageUrl() const;
    int testIndex() const;
    void setTestIndex(const int &testIndex_);

protected:
    void setT(const qreal &t_);
    QString fromImage() const;
    void setFromImage(const QString &fromImage_);
    QString toImage() const;
    void setToImage(const QString &toImage_);
    QString project() const;
    void setProject(const QString &project_);
    QString log() const;
    void setLog(const QString &log_);
    QString renderingTextureName() const;
    void setRenderingTextureName(const QString &renderingTextureName_);

    void freeProject();
    void loadProject();
    void setProjectFromImage();
    void setProjectToImage();

private:
    void initDefaultDrawingData();

signals:
    void tChanged();
    void fromImageChanged();
    void toImageChanged();
    void projectChanged();
    void logChanged();
    void testIndexChanged();
    void renderingTextureNameChanged();

private:
    qreal m_t = 0.0;
    QString m_fromImage;
    QString m_toImage;
    QString m_project;
    std::shared_ptr<drawing_data::QuizImageObjects> m_drawingData;
    std::shared_ptr<drawing_data::QuizImageObjects> m_drawingOldData;
    QString m_compilerLog;
    int m_testIndex = -1;
    QString m_renderingTextureName;
};


class QuizImages
{
private:
    QuizImages() = default;

public:
    static QuizImages &getInstance();
    void add(QuizImage *quizImage);
    void remove(QuizImage *quizImage);

protected:
    std::vector<QuizImage *> m_quizImages;
};


#endif // QUIZIMAGE_H
