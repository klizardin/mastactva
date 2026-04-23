#include "logoGeomerty.h"


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

void createQTLogoGeometry(
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
