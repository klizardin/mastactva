#include <QCoreApplication>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QDebug>


#define uniform
#define attribute
#define varying
#define highp
#define mediump


using vec2 = QVector2D;
using vec3 = QVector3D;
using vec4 = QVector4D;
using mat4 = QMatrix4x4;


class Shader
{
public:
    attribute highp vec4 vertexArg;
    attribute mediump vec4 texCoordArg;

    uniform mediump mat4 matrixArg;
    uniform mediump mat4 texMatrix1Arg;
    uniform mediump mat4 texMatrix2Arg;

    uniform mediump vec2 swipeDirection;

    uniform mediump float t;
    varying mediump vec4 texCoord1Var;
    varying mediump vec4 texCoord2Var;

    varying highp vec4 gl_Position;

    vec2 fixSize(vec2 arg_)
    {
        arg_.setX(arg_.x() > 0 ? arg_.x() : 1.0);
        arg_.setY(arg_.y() > 0 ? arg_.y() : 1.0);
        return arg_;
    }

    void main(void)
    {
        gl_Position = matrixArg * vertexArg;

        mediump vec4 sd = vec4(
                    swipeDirection.x() > 0.0 ? -1.0 : swipeDirection.x() < 0.0 ? 1.0 : 0.0 ,
                    swipeDirection.y() > 0.0 ? -1.0 : swipeDirection.y() < 0.0 ? 1.0 : 0.0 ,
                    0.0,
                    0.0);

        const mediump vec4 tl = vec4(0.0, 0.0, 0.0, 1.0);
        const mediump vec4 br = vec4(1.0, 1.0, 0.0, 1.0);

        mediump vec4 sds = vec4(abs(sd.x()), abs(sd.y()), 0.0, 0.0);
        //qDebug() << texMatrix1Arg * tl;
        //qDebug() << texMatrix1Arg * br;
        //qDebug() << texMatrix2Arg * tl;
        //qDebug() << texMatrix2Arg * br;

        mediump vec4 img1tl = (texMatrix1Arg * tl) * sds;
        mediump vec4 img1br = (texMatrix1Arg * br) * sds;
        mediump vec4 img2tl = (texMatrix2Arg * tl) * sds;
        mediump vec4 img2br = (texMatrix2Arg * br) * sds;
        img1tl.setX(img1tl.x() / img1tl.w());
        img1tl.setY(img1tl.y() / img1tl.w());
        img1br.setX(img1br.x() / img1br.w());
        img1br.setY(img1br.y() / img1br.w());
        img2tl.setX(img2tl.x() / img2tl.w());
        img2tl.setY(img2tl.y() / img2tl.w());
        img2br.setX(img2br.x() / img2br.w());
        img2br.setY(img2br.y() / img2br.w());

        mediump vec2 img1sz = fixSize(vec2(img1br) - vec2(img1tl));
        mediump vec2 img2sz = fixSize(vec2(img2br) - vec2(img2tl));

        // shift 2nd image by this value at start
        mediump float img2BorderRight =
                (img2br.x() > 1.0 ? (img2br.x() - 1.0)/img2sz.x() : 0.0)
                + (img2br.y() > 1.0 ? (img2br.y() - 1.0)/img2sz.y() : 0.0)
                ;
        // left shift of image 2
        mediump float img2BorderLeft = -img2tl.x()/img2sz.x() + -img2tl.y()/img2sz.y();

        // left shift of image 1
        // move second image
        mediump float img1BorderLeft = -img1tl.x()/img1sz.x() + -img1tl.y()/img1sz.y();
        mediump float t1 = img1BorderLeft;
        // move both images
        mediump float img2Width = 1.0/img2sz.x() + 1.0/img2sz.y() - 1.0;
        mediump float t2 = img2Width + img2BorderLeft - img1BorderLeft;
        // move first image
        mediump float t3 = img2BorderRight;
        mediump float ta = t1 + t2 + t3;

        mediump float t1n = t1/ta;
        mediump float t2n = t2/ta;
        //mediump float t3n = t3/ta;

        if(t < t1n)
        {
            qDebug() << "1: texCoordArg = " << texCoordArg
                     << "(texCoordArg + (t + s1 - 1.0) * sd) = " << (texCoordArg + (t + img2BorderRight - 1.0) * sd);
            texCoord1Var = texMatrix1Arg * texCoordArg;
            texCoord2Var = texMatrix2Arg * (texCoordArg + (t + img2BorderRight - 1.0) * sd);
        }
        else if(t < t1n+t2n)
        {
            qDebug() << "2: (texCoordArg + (t - t1/ta) * sd) = " << (texCoordArg + (t - t1n) * sd)
                     << "(texCoordArg + (t + s1 - 1.0) * sd) = " << (texCoordArg + (t + img2BorderRight - 1.0) * sd);
            texCoord1Var = texMatrix1Arg * (texCoordArg + (t - t1n) * sd);
            texCoord2Var = texMatrix2Arg * (texCoordArg + (t + img2BorderRight - 1.0) * sd);
        }
        else // if(t <= 1.0)
        {
            qDebug() << "3: (texCoordArg + (t - t1/ta) * sd) = " << (texCoordArg + (t - t1n) * sd)
                     << "texCoordArg = " << texCoordArg;
            texCoord1Var = texMatrix1Arg * (texCoordArg + (t - t1n) * sd);
            texCoord2Var = texMatrix2Arg * texCoordArg;
        }
    }
};



class Render
{
protected:
    QSize m_rectSize;
    QSize m_image1Size;
    QSize m_image2Size;
    QMatrix4x4 m_vertexMatrix;
    QMatrix4x4 m_texture1Matrix;
    QMatrix4x4 m_texture2Matrix;
    QVector2D m_swipeDirection;

public:
    void init(const QSize &rectSize_, const QSize &image1Size_, const QSize &image2Size_, const QVector2D &swipeDirection_)
    {
        m_rectSize = rectSize_;
        m_image1Size = image1Size_;
        m_image2Size = image2Size_;
        m_swipeDirection = swipeDirection_;

        calculatePreserveAspectFitTextureMatrix(m_texture1Matrix, m_image1Size, m_rectSize);
        calculatePreserveAspectFitTextureMatrix(m_texture2Matrix, m_image2Size, m_rectSize);
    }

    void test()
    {
        Shader shader;

        const int count = 25;
        for(int i = 0; i < count; i++)
        {
            qreal t = (qreal)i/(qreal)count;
            qDebug() << "t = " << t;
            init(shader);
            shader.t = t;
            shader.vertexArg = vec4(0.0, 0.0, 0.0, 1.0);
            shader.texCoordArg = vec4(0.0, 0.0, 0.0, 1.0);
            shader.main();

            //qDebug() << "shader.texCoord1Var = " << shader.texCoord1Var;
            //qDebug() << "shader.texCoord2Var = " << shader.texCoord2Var;
            //qDebug() << "shader.gl_Position = " << shader.gl_Position;

            vec2 img1tl = vec2(shader.texCoord1Var);
            vec2 img2tl = vec2(shader.texCoord2Var);

            init(shader);
            shader.t = t;
            shader.vertexArg = vec4(m_rectSize.width(), m_rectSize.height(), 0.0, 1.0);
            shader.texCoordArg = vec4(1.0, 1.0, 0.0, 1.0);
            shader.main();

            vec2 img1br = vec2(shader.texCoord1Var);
            vec2 img2br = vec2(shader.texCoord2Var);

            vec2 img1sz = (img1br - img1tl);
            vec2 img2sz = (img2br - img2tl);
            qreal img1l = -img1tl.x()/img1sz.x();
            qreal img2l = -img2tl.x()/img2sz.x();
            qreal img1r = (1.0-img1tl.x())/img1sz.x();
            qreal img2r = (1.0-img2tl.x())/img2sz.x();

            qDebug() << "img2x = " << img2l << "img2r = " << img2r;
            qDebug() << "img1x = " << img1l << "img1r = " << img1r;

/*            //qDebug() << "shader.texCoord1Var = " << shader.texCoord1Var;
            //qDebug() << "shader.texCoord2Var = " << shader.texCoord2Var;
            //qDebug() << "shader.gl_Position = " << shader.gl_Position;

            init(shader);
            shader.t = t;
            shader.vertexArg = vec4(m_rectSize.width(), 0.0, 0.0, 1.0);
            shader.texCoordArg = vec4(1.0, 0.0, 0.0, 1.0);
            shader.main();

            //qDebug() << "shader.texCoord1Var = " << shader.texCoord1Var;
            //qDebug() << "shader.texCoord2Var = " << shader.texCoord2Var;
            //qDebug() << "shader.gl_Position = " << shader.gl_Position;

            init(shader);
            shader.t = t;
            shader.vertexArg = vec4(0.0, m_rectSize.height(), 0.0, 1.0);
            shader.texCoordArg = vec4(0.0, 1.0, 0.0, 1.0);
            shader.main();

            //qDebug() << "shader.texCoord1Var = " << shader.texCoord1Var;
            //qDebug() << "shader.texCoord2Var = " << shader.texCoord2Var;
            //qDebug() << "shader.gl_Position = " << shader.gl_Position;
*/
        }
    }

protected:
    void init(Shader &shader_)
    {
        shader_.matrixArg = m_vertexMatrix;
        shader_.texMatrix1Arg = m_texture1Matrix;
        shader_.texMatrix2Arg = m_texture2Matrix;
        shader_.swipeDirection = m_swipeDirection;
        shader_.texCoord1Var = vec4();
        shader_.texCoord2Var = vec4();
        shader_.gl_Position = vec4();
    }

    void calculatePreserveAspectFitTextureMatrix(QMatrix4x4 & textureMatrix_, const QSize &imageSize_, const QSize &rectSize_)
    {
        const qreal imageRate = (qreal)std::max(1, imageSize_.width())/(qreal)std::max(1, imageSize_.height());
        const qreal rectRate = (qreal)std::max(1, rectSize_.width())/(qreal)std::max(1, rectSize_.height());
        if(rectRate >= imageRate)
        {
            textureMatrix_.scale(rectRate/imageRate, 1.0);
            textureMatrix_.translate(-(rectRate - imageRate)/rectRate*0.5, 0.0);
        }
        else
        {
            textureMatrix_.scale(1.0, imageRate/rectRate);
            textureMatrix_.translate(0.0, -(imageRate - rectRate)/imageRate*0.5);
        }
    }

};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Render render;
    render.init(QSize(100, 100), QSize(200, 800), QSize(400, 800), vec2(1.0, 0.0));
    render.test();

    qDebug() << "done";

    return a.exec();
}
