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

#include "drawing_tests.h"
#include <math.h>
#include <QRandomGenerator>


const char *g_defaultVertexShader =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec3 normal;\n"
    "uniform mediump mat4 matrix;\n"
    "uniform mediump float alpha;\n"
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
    "    float angle = max(dot(normal, toLight), 0.0);\n"
    "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
    "    color = vec4(col * 0.2 + col * 0.8 * angle, alpha);\n"
    "    color = clamp(color, 0.0, 1.0);\n"
    "    gl_Position = matrix * vertex;\n"
    "}\n";

const char *g_defaultFragmentShader =
    "varying mediump vec4 color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = color;\n"
    "}\n";

void drawing_data::TestMinimalDrawQTLogoQuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    object->vertexShader = g_defaultVertexShader;
    object->fragmentShader = g_defaultFragmentShader;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    test::createQTLogoGeometry(*vertices.get(), *normals.get());

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    "alpha",
                    std::make_shared<GLfloat>(1.0)
                    )
                );

    QRandomGenerator gen;
    qreal fScale = 1;
    qreal fAngle = gen.generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview(new QMatrix4x4);
    modelview->rotate(fAngle, 0.0f, 1.0f, 0.0f);
    modelview->rotate(fAngle, 1.0f, 0.0f, 0.0f);
    modelview->rotate(fAngle, 0.0f, 0.0f, 1.0f);
    modelview->scale(fScale);
    modelview->translate(0.0f, -0.2f, 0.0f);

    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview }
                ));
    data_.objects.push_back(std::move(object));
}

std::pair<const char *, const char *>  drawing_data::TestMinimalDrawQTLogoQuizImageObject::getDescription() const
{
    return {"TestMinimalDrawQTLogoQuizImageObject",
        "draw QT logo: uses default shaders; setup attributes and uniform variables."};
}


void drawing_data::TestMinimal2PassDrawQTLogoQuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object1(new QuizImageObject());
    std::unique_ptr<QuizImageObject> object2(new QuizImageObject());

    object1->vertexShader = g_defaultVertexShader;
    object1->fragmentShader = g_defaultFragmentShader;
    object2->vertexShader = g_defaultVertexShader;
    object2->fragmentShader = g_defaultFragmentShader;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    test::createQTLogoGeometry(*vertices.get(), *normals.get());

    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );

    qreal fScale = 1;
    qreal fAngle1 = QRandomGenerator::global()->generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview1(new QMatrix4x4);
    modelview1->rotate(fAngle1, 0.0f, 1.0f, 0.0f);
    modelview1->rotate(fAngle1, 1.0f, 0.0f, 0.0f);
    modelview1->rotate(fAngle1, 0.0f, 0.0f, 1.0f);
    modelview1->scale(fScale);
    modelview1->translate(0.0f, -0.2f, 0.0f);

    qreal fAngle2 = QRandomGenerator::global()->generateDouble() * 360.0;
    fScale = 0.75;
    std::shared_ptr<QMatrix4x4> modelview2(new QMatrix4x4);
    modelview2->rotate(fAngle2, 0.0f, 1.0f, 0.0f);
    modelview2->rotate(fAngle2, 1.0f, 0.0f, 0.0f);
    modelview2->rotate(fAngle2, 0.0f, 0.0f, 1.0f);
    modelview2->scale(fScale);
    modelview2->translate(0.0f, -0.2f, 0.0f);

    object1->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview1 }
                ));
    object1->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    "alpha",
                    std::make_shared<GLfloat>(1.0)
                    )
                );
    object2->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview2 }
                ));
    object2->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    "alpha",
                    std::make_shared<GLfloat>(1.0)
                    )
                );

    data_.objects.push_back(std::move(object1));
    data_.objects.push_back(std::move(object2));
}

std::pair<const char *, const char *>  drawing_data::TestMinimal2PassDrawQTLogoQuizImageObject::getDescription() const
{
    return {"TestMinimal2PassDrawQTLogoQuizImageObject",
        "draw two QT logos (two passes): uses default shaders; setup attributes and uniform variables."};
}


void drawing_data::Test0QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test000/default.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test000/default.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        { g_renderFromImageName, g_noImage001Name, QString{} },
        { g_renderToImageName, g_noImage002Name, QString{} },
    };

    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderVertexAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );
    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderTextureAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderFromImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderToImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderScreenRectName,
                    std::make_shared<QVector2D>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderWindowSizeName,
                    std::make_shared<QVector2D>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderTName,
                    std::make_shared<GLfloat>(0.5)
                    )
                );

    std::shared_ptr<GLfloat> renderOpacity(new GLfloat{1.0});
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderOpacityName,
                    std::make_shared<GLfloat>(1.0)
                    )
                );

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs(new QVector2D(0.0, 0.0));
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderFacedGeometryCoefsName,
                    std::make_shared<QVector2D>(0.0, 0.0)
                    )
                );

    std::shared_ptr<QVector2D> renderGeomertySize(new QVector2D(2.0, 2.0));
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderGeomertySizeName,
                    std::make_shared<QVector2D>(2.0, 2.0)
                    )
                );

    std::shared_ptr<GLint> renderIsGeomertySolid(new GLint{1});
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLint>>(
                    g_renderIsGeomertySolidName,
                    std::make_shared<GLint>(1)
                    )
                );

    data_.objects.push_back(std::move(object));
}

std::pair<const char *, const char *>  drawing_data::Test0QuizImageObject::getDescription() const
{
    return {"Test0QuizImageObject",
        "draw two images in betweeen - simple transmition from one image to another: "
        "uses default shaders for mixing of images; "
        "setup attributes and uniform variables; setup textures."};
}


void drawing_data::Test1QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test001/cube_rotation_horizontal_faced.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test001/default_view_two_images_mix_both.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        { g_renderFromImageName, g_noImage001Name, QString{} },
        { g_renderToImageName, g_noImage002Name, QString{} },
    };

    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderVertexAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );
    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderTextureAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderFromImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    std::shared_ptr<QMatrix4x4> renderToImageMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderToImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    std::shared_ptr<QVector2D> renderScreenRect = std::make_shared<QVector2D>();
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderScreenRectName,
                    std::make_shared<QVector2D>()
                    )
                );
    std::shared_ptr<QVector2D> renderWindowSize(new QVector2D());
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderWindowSizeName,
                    std::make_shared<QVector2D>()
                    )
                );

    QRandomGenerator gen;
    QVector4D faceRotateDir;
    faceRotateDir.setX(gen.generateDouble() - 0.5);
    faceRotateDir.setY(gen.generateDouble() - 0.5);
    faceRotateDir.setZ(gen.generateDouble() - 0.5);
    faceRotateDir.setW(gen.generateDouble() - 0.5);
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector4D>>(
                        "faceRotateDir",
                        std::make_shared<QVector4D>(faceRotateDir)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderTName,
                    std::make_shared<GLfloat>(0.5)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    g_renderOpacityName,
                    std::make_shared<GLfloat>(1.0)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderFacedGeometryCoefsName,
                    std::make_shared<QVector2D>(0.0, 1.0e-4)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderGeomertySizeName,
                    std::make_shared<QVector2D>(2.0, 4.0)
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLint>>(
                    g_renderIsGeomertySolidName,
                    std::make_shared<GLint>(0)
                    )
                );

    data_.objects.push_back(std::move(object));
}

std::pair<const char *, const char *>  drawing_data::Test1QuizImageObject::getDescription() const
{
    return {"Test1QuizImageObject",
        "draw two images in betweeen - cube rotation: "
        "uses shaders for mixing of images with cube rotation; "
        "setup attributes and uniform variables; setup textures."};
}


void drawing_data::Test2QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test002/mix_line_horizontal.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test002/mix_varying_t.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        { g_renderFromImageName, g_noImage001Name, QString{} },
        { g_renderToImageName, g_noImage002Name, QString{} },
    };

    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderVertexAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );
    object->attributes.push_back(
                std::make_unique<drawing_data::Attribute<QVector4D>>(
                    g_renderTextureAttributeName,
                    std::make_shared<std::vector<QVector4D>>()
                    )
                );

    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderFromImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QMatrix4x4>>(
                    g_renderToImageMatrixName,
                    std::make_shared<QMatrix4x4>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderScreenRectName,
                    std::make_shared<QVector2D>()
                    )
                );
    object->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<QVector2D>>(
                    g_renderWindowSizeName,
                    std::make_shared<QVector2D>()
                    )
                );

    QRandomGenerator gen;
    std::shared_ptr<GLfloat> slope = std::make_shared<GLfloat>(gen.generateDouble());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "slope", slope }
                ));

    std::shared_ptr<GLfloat> renderT = std::make_shared<GLfloat>(0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderTName, renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity = std::make_shared<GLfloat>(1.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderOpacityName, renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs = std::make_shared<QVector2D>(0.0, 0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderFacedGeometryCoefsName, renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize = std::make_shared<QVector2D>(20.0, 20.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderGeomertySizeName, renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid = std::make_shared<GLint>(1);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ g_renderIsGeomertySolidName, renderIsGeomertySolid }
                ));

    data_.objects.push_back(std::move(object));
}

std::pair<const char *, const char *>  drawing_data::Test2QuizImageObject::getDescription() const
{
    return {"Test2QuizImageObject",
        "draw two images in betweeen - mix line horizontally: "
        "uses shaders for mixing of images by line horizontally; "
        "setup attributes and uniform variables; setup textures."};
}


void drawing_data::Test3QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(":/Shaders/Shaders/test003/mix_direction_faced4.vsh").toUtf8();
    static QByteArray fragment = loadTextFile(":/Shaders/Shaders/test002/mix_varying_t.fsh").toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = {
        {g_renderFromImageName, ":/Images/Images/no-image-001.png", QString{}},
        {g_renderToImageName, ":/Images/Images/no-image-002.png", QString{}},
    };

    std::shared_ptr<std::vector<QVector4D>> vertices = std::make_shared<std::vector<QVector4D>>();
    std::shared_ptr<std::vector<QVector4D>> textures = std::make_shared<std::vector<QVector4D>>();

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderVertexAttributeName, vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderTextureAttributeName, textures }
                    )
                );

    std::shared_ptr<QMatrix4x4> renderMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderMatrixName, renderMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderFromImageMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderFromImageMatrixName, renderFromImageMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderToImageMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderToImageMatrixName, renderToImageMatrix }
                ));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderScreenRectName, std::make_shared<QVector2D>() }
                ));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderWindowSizeName, std::make_shared<QVector2D>() }
                ));

    QRandomGenerator gen(time(nullptr));
    std::shared_ptr<QVector2D> direction = std::make_shared<QVector2D>(gen.generateDouble(), gen.generateDouble());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ "direction", direction }
                ));

    std::shared_ptr<GLfloat> slope = std::make_shared<GLfloat>(gen.generateDouble());
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ "slope", slope }
                ));

    std::shared_ptr<QVector4D> faceDirections = std::make_shared<QVector4D>();
    faceDirections->setX(gen.generateDouble() - 0.5);
    faceDirections->setY(gen.generateDouble() - 0.5);
    faceDirections->setZ(gen.generateDouble() - 0.5);
    faceDirections->setW(gen.generateDouble() - 0.5);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector4D>{ "faceDirections", faceDirections }
                ));


    std::shared_ptr<GLfloat> renderT = std::make_shared<GLfloat>(0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderTName, renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity = std::make_shared<GLfloat>(1.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderOpacityName, renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs = std::make_shared<QVector2D>(0.0, 0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderFacedGeometryCoefsName, renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize = std::make_shared<QVector2D>(20.0, 20.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderGeomertySizeName, renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid = std::make_shared<GLint>(1);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ g_renderIsGeomertySolidName, renderIsGeomertySolid }
                ));

    data_.objects.push_back(std::move(object));
}

std::pair<const char *, const char *>  drawing_data::Test3QuizImageObject::getDescription() const
{
    return {"Test3QuizImageObject",
        "draw two images in betweeen - mix 4 facets on some direction: "
        "uses shaders for mixing of images with 4 facets; "
        "setup attributes and uniform variables; setup textures."};
}


std::unique_ptr<drawing_data::QuizImageObject> drawing_data::Test4QuizImageObject::createObjectWithgDefaultShaderAndPeriod(
        double tFrom_,
        double tTo_,
        const std::vector<Texture> &textures_,
        const QString &vertexShaderFileName_,
        const QString &fragmentShaderFileName_
        ) const
{
    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    static QByteArray vertex = loadTextFile(vertexShaderFileName_).toUtf8();
    static QByteArray fragment = loadTextFile(fragmentShaderFileName_).toUtf8();
    object->vertexShader = vertex.constData();
    object->fragmentShader = fragment.constData();

    object->textures = textures_;

    std::shared_ptr<std::vector<QVector4D>> vertices = std::make_shared<std::vector<QVector4D>>();
    std::shared_ptr<std::vector<QVector4D>> textures = std::make_shared<std::vector<QVector4D>>();

    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderVertexAttributeName, vertices }
                    )
                );
    object->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector4D>{ g_renderTextureAttributeName, textures }
                    )
                );

    std::shared_ptr<QMatrix4x4> renderMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderMatrixName, renderMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderFromImageMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderFromImageMatrixName, renderFromImageMatrix }
                ));
    std::shared_ptr<QMatrix4x4> renderToImageMatrix = std::make_shared<QMatrix4x4>();
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ g_renderToImageMatrixName, renderToImageMatrix }
                ));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderScreenRectName, std::make_shared<QVector2D>() }
                ));
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderWindowSizeName, std::make_shared<QVector2D>() }
                ));

    std::shared_ptr<GLfloat> renderT = std::make_shared<GLfloat>(0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderTName, renderT }
                ));

    std::shared_ptr<GLfloat> renderOpacity = std::make_shared<GLfloat>(1.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLfloat>{ g_renderOpacityName, renderOpacity }
                ));

    std::shared_ptr<QVector2D> renderFacedGeometryCoefs = std::make_shared<QVector2D>(0.0, 0.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderFacedGeometryCoefsName, renderFacedGeometryCoefs }
                ));

    std::shared_ptr<QVector2D> renderGeomertySize = std::make_shared<QVector2D>(20.0, 20.0);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderGeomertySizeName, renderGeomertySize }
                ));

    std::shared_ptr<GLint> renderIsGeomertySolid = std::make_shared<GLint>(1);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<GLint>{ g_renderIsGeomertySolidName, renderIsGeomertySolid }
                ));

    std::shared_ptr<QVector2D> renderPeriod = std::make_shared<QVector2D>(tFrom_, tTo_);
    object->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QVector2D>{ g_renderPeriodName, renderPeriod }
                ));

    return object;
}

class ImageMatrixDefaultCalculationConsts : public opengl_drawing::IEffectCalculation
{
public:
    ImageMatrixDefaultCalculationConsts();
    void calculate(opengl_drawing::IVariables *variables_) const override;
    void add(
            const QString &imageName_,
            const QString &fileName_,
            double tFrom_, double tTo_
            );

private:
    QMatrix4x4 getImageMatrix(
            const QString &imageName_,
            const QSize &windowSize_,
            double renderT_
            ) const;

    struct SizeInfo
    {
        QString name;
        double tFrom = 0.0;
        double tTo = 1.0;
        QSize size;
    };

private:
    QVector<SizeInfo> m_sizes;
};

ImageMatrixDefaultCalculationConsts::ImageMatrixDefaultCalculationConsts()
{
    setFilename(QString(g_imageMatrixDefaultCalculationName) + "_consts");
    setRequiredVariables({
                 g_renderWindowSizeName,
                 g_renderFromImageName,
                 g_renderToImageName
                });
}

void ImageMatrixDefaultCalculationConsts::calculate(opengl_drawing::IVariables *variables_) const
{
    opengl_drawing::Objects *objects = variables_ ? dynamic_cast<opengl_drawing::Objects *>(variables_->getRoot()) : nullptr;
    if(!objects)
    {
        return;
    }

    const QVector2D windowSizeF = objects->getUniform(g_renderWindowSizeName, QVector2D{1.0,1.0});
    const float renderT = objects->getUniform(g_renderTName, double{0.0});
    const QSize windowSize((int)windowSizeF.x(), (int)windowSizeF.y());
    objects->setUniform(
                g_renderFromImageMatrixName,
                getImageMatrix(g_renderFromImageName, windowSize, renderT)
                );
    objects->setUniform(
                g_renderToImageMatrixName,
                getImageMatrix(g_renderToImageName, windowSize, renderT)
                );
}

void ImageMatrixDefaultCalculationConsts::add(
        const QString &imageName_,
        const QString &fileName_,
        double tFrom_, double tTo_
        )
{
    QImage image;
    image.load(fileName_);
    const QSize imageSize = image.size();
    m_sizes.push_back({imageName_, tFrom_, tTo_, imageSize});
}

QMatrix4x4 ImageMatrixDefaultCalculationConsts::getImageMatrix(
        const QString &imageName_,
        const QSize &windowSize_,
        double renderT_
        ) const
{
    const auto fit = std::find_if(
                std::begin(m_sizes),
                std::end(m_sizes),
                [&imageName_,renderT_](const SizeInfo &si_)->bool
    {
        return si_.name == imageName_ && si_.tFrom <= renderT_ && renderT_ <= si_.tTo;
    });
    if(std::end(m_sizes) == fit)
    {
        return QMatrix4x4{};
    }
    return ::opengl_drawing::calculatePreserveAspectFitTextureMatrix(fit->size, windowSize_);
}

void drawing_data::Test4QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    const double eps = g_renderTeps;
    auto calc = std::make_shared<ImageMatrixDefaultCalculationConsts>();
    calc->add(g_renderFromImageName, ":/Images/Images/no-image-001.png", 0.0 - eps, 0.5);
    calc->add(g_renderToImageName, ":/Images/Images/no-image-003.png", 0.0 - eps, 0.5);
    calc->add(g_renderFromImageName, ":/Images/Images/no-image-003.png", 0.5 + eps, 1.0 + eps);
    calc->add(g_renderToImageName, ":/Images/Images/no-image-002.png", 0.5 + eps, 1.0 + eps);
    data_.calculations.push_back(std::move(calc));

    data_.objects.push_back(createObjectWithgDefaultShaderAndPeriod(
                0.0 - eps, 0.5,
                {
                    {g_renderFromImageName, ":/Images/Images/no-image-001.png", QString{}},
                    {g_renderToImageName, ":/Images/Images/no-image-003.png", QString{}},
                },
                ":/Shaders/Shaders/default.vsh",
                ":/Shaders/Shaders/test004/period.fsh"
            ));
    data_.objects.push_back(createObjectWithgDefaultShaderAndPeriod(
                0.5 + eps, 1.0 + eps,
                {
                    {g_renderFromImageName, ":/Images/Images/no-image-003.png", QString{}},
                    {g_renderToImageName, ":/Images/Images/no-image-002.png", QString{}},
                },
                ":/Shaders/Shaders/default.vsh",
                ":/Shaders/Shaders/test004/period.fsh"
            ));
}

std::pair<const char *, const char *>  drawing_data::Test4QuizImageObject::getDescription() const
{
    return {"Test4QuizImageObject",
        "draw tree images in betweeen - mix 3 images (second between first and third): "
        "uses default shaders for mixing of images; "
        "setup attributes and uniform variables; setup textures; "
        "create two objects: object 1 - first image + second image; object 2 - second image + third image"};
}


void drawing_data::TestAlphaBlendingsDrawQTLogoQuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object1(new QuizImageObject());
    std::unique_ptr<QuizImageObject> object2(new QuizImageObject());

    object1->vertexShader = g_defaultVertexShader;
    object1->fragmentShader = g_defaultFragmentShader;
    object2->vertexShader = g_defaultVertexShader;
    object2->fragmentShader = g_defaultFragmentShader;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    test::createQTLogoGeometry(*vertices.get(), *normals.get());

    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object1->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "vertex", vertices }
                    )
                );
    object2->attributes.push_back(
                std::unique_ptr<drawing_data::IAttribute>(
                    new drawing_data::Attribute<QVector3D>{ "normal", normals }
                    )
                );
    object2->objectStates.push_back(g_depthTestDisable);
    object2->objectStates.push_back(g_alphaBlendingDefault);

    qreal fScale = 1;
    qreal fAngle1 = QRandomGenerator::global()->generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview1(new QMatrix4x4);
    modelview1->rotate(fAngle1, 0.0f, 1.0f, 0.0f);
    modelview1->rotate(fAngle1, 1.0f, 0.0f, 0.0f);
    modelview1->rotate(fAngle1, 0.0f, 0.0f, 1.0f);
    modelview1->scale(fScale);
    modelview1->translate(0.0f, -0.2f, 0.0f);

    qreal fAngle2 = QRandomGenerator::global()->generateDouble() * 360.0;
    fScale = 0.75;
    std::shared_ptr<QMatrix4x4> modelview2(new QMatrix4x4);
    modelview2->rotate(fAngle2, 0.0f, 1.0f, 0.0f);
    modelview2->rotate(fAngle2, 1.0f, 0.0f, 0.0f);
    modelview2->rotate(fAngle2, 0.0f, 0.0f, 1.0f);
    modelview2->scale(fScale);
    modelview2->translate(0.0f, -0.2f, 0.0f);

    object1->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview1 }
                ));
    object1->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    "alpha",
                    std::make_shared<GLfloat>(1.0)
                    )
                );
    object2->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview2 }
                ));
    object2->uniforms.push_back(
                std::make_unique<drawing_data::Uniform<GLfloat>>(
                    "alpha",
                    std::make_shared<GLfloat>(0.5)
                    )
                );

    data_.objects.push_back(std::move(object1));
    data_.objects.push_back(std::move(object2));
}

std::pair<const char *, const char *>  drawing_data::TestAlphaBlendingsDrawQTLogoQuizImageObject::getDescription() const
{
    return {"TestAlphaBlendingsDrawQTLogoQuizImageObject",
        "draw two QT logo with alpha blending: "
        "uses default shaders for objects; "
        "setup attributes and uniform variables; setup textures; "
        "create two objects: object 1 - background drawing; object 2 - alpha blending drawing"};
}
