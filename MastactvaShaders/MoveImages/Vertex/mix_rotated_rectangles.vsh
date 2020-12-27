/*@shader @description default vertex shader to 3 state mix in 9 rectangles
path: MastactvaShaders/MoveImages/Vertex/mix_rectangles_3state.vsh
@geomentryFaced
@geomentrySize (20, 20)
@facedGeometryCoef (1e-3, 1e-3)
*/
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec4 rect2;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec4 rect3;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue rand(0.075, 0.2)
    @description k of v_t line
*/
uniform mediump float slope;


uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


const highp int RECTANGLES = 5;
const mediump float M_PI = 3.14159265359;


mediump float sqr(in mediump float a)
{
    return a * a;
}

mediump float cube(in mediump float a)
{
    return sqr(a) * a;
}


void main(void)
{
    gl_Position = matrixArg * vertexArg;

   mediump vec4 rect1 = vec4(1.0, 0.0, 0.0, 0.2);
   mediump vec4 rect4 = vec4(0.0, 1.5, 1.0, 0.2);

    mediump vec2 post = cube(1.0 - t) * rect1.xy +
            3.0 * sqr(1.0 - t) * t * rect2.xy +
            3.0 * (1.0 - t) * sqr(t) * rect3.xy +
            cube(t) * rect4.xy;
    mediump float at = cube(1.0 - t) * rect1.z +
            3.0 * sqr(1.0 - t) * t * rect2.z +
            3.0 * (1.0 - t) * sqr(t) * rect3.z +
            cube(t) * rect4.z;
    mediump float sizet = cube(1.0 - t) * rect1.w +
            3.0 * sqr(1.0 - t) * t * rect2.w * 0.25 +
            3.0 * (1.0 - t) * sqr(t) * rect3.w * 0.25 +
            cube(t) * rect4.w;

    mediump vec2 pos = vec2(post.x * rectSize.x, post.y * rectSize.y);
    mediump vec4 vertexPos = vec4(vertexArg.x/vertexArg.w - pos.x,
                               vertexArg.y/vertexArg.w - pos.y,
                               0.0, 1.0);
    mediump float a = at * M_PI * 2.0;
    mediump mat4 rotateMat = mat4(
                vec4(cos(a), -sin(a), 0.0, 0.0),
                vec4(sin(a), cos(a), 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
    mediump vec4 vertexRotated = rotateMat * vertexPos;
    mediump vec2 rectsz = vec2(sizet * rectSize.x * 0.5, sizet * rectSize.y * 0.5);
    mediump float szl = length(rectSize);
    mediump float tx = 0.0;
    mediump float ty = 0.0;
    if(pos.x < 0.0)
    {
        tx = -(vertexRotated.x - (-rectsz.x))/(szl*slope);
    }
    else
    {
        tx = (vertexRotated.x - (rectsz.x))/(szl*slope);
    }
    if(pos.y < 0.0)
    {
        ty = -(-vertexRotated.y - (-rectsz.y))/(szl*slope);
    }
    else
    {
        ty = (-vertexRotated.y - (rectsz.y))/(szl*slope);
    }
    if((vertexRotated.x <= (-rectsz.x) || vertexRotated.x >= rectsz.x) &&
            (vertexRotated.y > (-rectsz.y) && vertexRotated.y < rectsz.y))
    {
        v_t = tx;
    }
    else if((vertexRotated.y <= (-rectsz.y) || vertexRotated.y >= rectsz.y) &&
            (vertexRotated.x > (-rectsz.x) && vertexRotated.x < rectsz.x))
    {
        v_t = ty;
    }
    else
    {
        v_t = max(tx, ty);
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
