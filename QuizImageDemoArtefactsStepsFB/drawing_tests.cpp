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


void quad(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3, qreal x4, qreal y4
        )
{
    vertices.push_back(QVector3D(x1, y1, -0.05f));
    vertices.push_back(QVector3D(x2, y2, -0.05f));
    vertices.push_back(QVector3D(x4, y4, -0.05f));

    vertices.push_back(QVector3D(x3, y3, -0.05f));
    vertices.push_back(QVector3D(x4, y4, -0.05f));
    vertices.push_back(QVector3D(x2, y2, -0.05f));

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(x4 - x1, y4 - y1, 0.0f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    vertices.push_back(QVector3D(x4, y4, 0.05f));
    vertices.push_back(QVector3D(x2, y2, 0.05f));
    vertices.push_back(QVector3D(x1, y1, 0.05f));

    vertices.push_back(QVector3D(x2, y2, 0.05f));
    vertices.push_back(QVector3D(x4, y4, 0.05f));
    vertices.push_back(QVector3D(x3, y3, 0.05f));

    n = QVector3D::normal
        (QVector3D(x2 - x4, y2 - y4, 0.0f), QVector3D(x1 - x4, y1 - y4, 0.0f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);
}

void extrude(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals,
        qreal x1, qreal y1, qreal x2, qreal y2
        )
{
    vertices.push_back(QVector3D(x1, y1, +0.05f));
    vertices.push_back(QVector3D(x2, y2, +0.05f));
    vertices.push_back(QVector3D(x1, y1, -0.05f));

    vertices.push_back(QVector3D(x2, y2, -0.05f));
    vertices.push_back(QVector3D(x1, y1, -0.05f));
    vertices.push_back(QVector3D(x2, y2, +0.05f));

    QVector3D n = QVector3D::normal
        (QVector3D(x2 - x1, y2 - y1, 0.0f), QVector3D(0.0f, 0.0f, -0.1f));

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);

    normals.push_back(n);
    normals.push_back(n);
    normals.push_back(n);
}

namespace test
{

void createGeometry(
        std::vector<QVector3D> &vertices,
        std::vector<QVector3D> &normals
        )
{
    qreal x1 = +0.06f;
    qreal y1 = -0.14f;
    qreal x2 = +0.14f;
    qreal y2 = -0.06f;
    qreal x3 = +0.08f;
    qreal y3 = +0.00f;
    qreal x4 = +0.30f;
    qreal y4 = +0.22f;

    quad(vertices, normals, x1, y1, x2, y2, y2, x2, y1, x1);
    quad(vertices, normals, x3, y3, x4, y4, y4, x4, y3, x3);

    extrude(vertices, normals, x1, y1, x2, y2);
    extrude(vertices, normals, x2, y2, y2, x2);
    extrude(vertices, normals, y2, x2, y1, x1);
    extrude(vertices, normals, y1, x1, x1, y1);
    extrude(vertices, normals, x3, y3, x4, y4);
    extrude(vertices, normals, x4, y4, y4, x4);
    extrude(vertices, normals, y4, x4, y3, x3);

    const qreal Pi = M_PI;
    const int NumSectors = 100;

    for (int i = 0; i < NumSectors; ++i)
    {
        qreal angle1 = (i * 2 * Pi) / NumSectors;
        qreal x5 = 0.30 * sin(angle1);
        qreal y5 = 0.30 * cos(angle1);
        qreal x6 = 0.20 * sin(angle1);
        qreal y6 = 0.20 * cos(angle1);

        qreal angle2 = ((i + 1) * 2 * Pi) / NumSectors;
        qreal x7 = 0.20 * sin(angle2);
        qreal y7 = 0.20 * cos(angle2);
        qreal x8 = 0.30 * sin(angle2);
        qreal y8 = 0.30 * cos(angle2);

        quad(vertices, normals, x5, y5, x6, y6, x7, y7, x8, y8);

        extrude(vertices, normals, x6, y6, x7, y7);
        extrude(vertices, normals, x8, y8, x5, y5);
    }

    for (int i = 0; i < (int)vertices.size();i++)
        vertices[i] *= 2.0f;
}

}

void drawing_data::TestMinimalDrawQuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object(new QuizImageObject());

    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";

    object->vertexShader = vsrc1;
    object->fragmentShader = fsrc1;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    test::createGeometry(*vertices.get(), *normals.get());

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


void drawing_data::TestMinimal2PassDrawQuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    std::unique_ptr<QuizImageObject> object1(new QuizImageObject());
    std::unique_ptr<QuizImageObject> object2(new QuizImageObject());

    const char *vsrc1 =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "    float angle = max(dot(normal, toLight), 0.0);\n"
        "    vec3 col = vec3(0.40, 1.0, 0.0);\n"
        "    color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
        "    color = clamp(color, 0.0, 1.0);\n"
        "    gl_Position = matrix * vertex;\n"
        "}\n";

    const char *fsrc1 =
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = color;\n"
        "}\n";

    QRandomGenerator gen;

    object1->vertexShader = vsrc1;
    object1->fragmentShader = fsrc1;
    object2->vertexShader = vsrc1;
    object2->fragmentShader = fsrc1;

    std::shared_ptr<std::vector<QVector3D>> vertices(new std::vector<QVector3D>());
    std::shared_ptr<std::vector<QVector3D>> normals(new std::vector<QVector3D>());
    test::createGeometry(*vertices.get(), *normals.get());

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
    qreal fAngle1 = gen.generateDouble() * 360.0;
    std::shared_ptr<QMatrix4x4> modelview1(new QMatrix4x4);
    modelview1->rotate(fAngle1, 0.0f, 1.0f, 0.0f);
    modelview1->rotate(fAngle1, 1.0f, 0.0f, 0.0f);
    modelview1->rotate(fAngle1, 0.0f, 0.0f, 1.0f);
    modelview1->scale(fScale);
    modelview1->translate(0.0f, -0.2f, 0.0f);

    qreal fAngle2 = gen.generateDouble() * 360.0;
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
    object2->uniforms.push_back(
                std::unique_ptr<drawing_data::IUniform>(
                   new drawing_data::Uniform<QMatrix4x4>{ "matrix", modelview2 }
                ));

    data_.objects.push_back(std::move(object1));
    data_.objects.push_back(std::move(object2));
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
        { g_renderFromImageName, g_noImage001Name },
        { g_renderToImageName, g_noImage002Name },
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
        { g_renderFromImageName, g_noImage001Name },
        { g_renderToImageName, g_noImage002Name },
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
        { g_renderFromImageName, g_noImage001Name },
        { g_renderToImageName, g_noImage002Name },
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
        {g_renderFromImageName, ":/Images/Images/no-image-001.png"},
        {g_renderToImageName, ":/Images/Images/no-image-002.png"},
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

void drawing_data::Test4QuizImageObject::initialize(
        QuizImageObjects &data_,
        int argsSetIndex_ /*= 0*/
        ) const
{
    Q_UNUSED(argsSetIndex_);

    auto object1 = createObjectWithgDefaultShaderAndPeriod(
                0.0, 0.5,
                {
                    {g_renderFromImageName, ":/Images/Images/no-image-001.png"},
                    {g_renderToImageName, ":/Images/Images/no-image-003.png"},
                },
                ":/Shaders/Shaders/default.vsh",
                ":/Shaders/Shaders/test004/period.fsh"
            );
    data_.objects.push_back(std::move(object1));
    auto object2 = createObjectWithgDefaultShaderAndPeriod(
                0.5, 0.999999,
                {
                    {g_renderFromImageName, ":/Images/Images/no-image-003.png"},
                    {g_renderToImageName, ":/Images/Images/no-image-002.png"},
                },
                ":/Shaders/Shaders/default.vsh",
                ":/Shaders/Shaders/test004/period.fsh"
            );
    data_.objects.push_back(std::move(object2));
    auto object3 = createObjectWithgDefaultShaderAndPeriod(
                0.999999, 1.000001,
                {
                    {g_renderFromImageName, ":/Images/Images/no-image-001.png"},
                    {g_renderToImageName, ":/Images/Images/no-image-002.png"},
                },
                ":/Shaders/Shaders/default.vsh",
                ":/Shaders/Shaders/default.fsh"
            );
    data_.objects.push_back(std::move(object3));
}
