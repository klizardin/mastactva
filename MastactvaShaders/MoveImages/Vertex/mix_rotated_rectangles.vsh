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
uniform mediump vec4 rect1;

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
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec4 rect4;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
   @defaultValue rand(2,5)
   @description speed of rising faces
*/
uniform mediump int shift;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


const highp int RECTANGLES = 5;
const mediump float M_PI = 3.14159265359;


void main(void)
{
    gl_Position = matrixArg * vertexArg;

    v_t = t;
    bool init = false;
    for(highp int i = 0; i < RECTANGLES; i++)
    {
        mediump vec4 rect =
              i == 0
                ? rect1
                : i == 1
                  ? rect2
                  : i == 2
                    ? rect3
                    : rect4;
        mediump vec2 pos = vec2(rect.x * rectSize.x, rect.y * rectSize.y);
        mediump vec4 vertexPos = vec4(vertexArg.x/vertexArg.w - pos.x,
                                   vertexArg.y/vertexArg.w - pos.y,
                                   0.0, 1.0);
        mediump float a = rect.z * M_PI * 2.0;
        mediump mat4 rotateMat = mat4(
                    vec4(cos(a), -sin(a), 0.0, 0.0),
                    vec4(sin(a), cos(a), 0.0, 0.0),
                    vec4(0.0, 0.0, 1.0, 0.0),
                    vec4(0.0, 0.0, 0.0, 1.0));
        mediump vec4 vertexRotated = rotateMat * vertexPos;
        mediump vec2 rectSize = vec2(rect.w * rectSize.x * 0.5, rect.w * rectSize.y * 0.5);
        bool insideRect = i < RECTANGLES - 1;
        if((vertexRotated.x >= -rectSize.x && vertexRotated.x <= rectSize.x
                && vertexRotated.y >= -rectSize.y && vertexRotated.y <= rectSize.y) ||
                !insideRect)
        {
            mediump float t0 = float(i)/float(RECTANGLES);
            mediump float t1 = float(min(i + shift, RECTANGLES))/float(RECTANGLES);
            if(!init)
            {
                mediump float vt = (t - t0) / (t1 - t0);
                v_t = vt;
            }
            init = true;
        }
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
