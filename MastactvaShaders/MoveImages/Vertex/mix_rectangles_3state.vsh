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
uniform mediump vec4 positions1;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec4 positions2;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
   @defaultValue rand(2,9)
   @description speed of rising faces
*/
uniform mediump int shift;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


const highp int RECTANGLES = 9;
const mediump float WIDTH = 20.0;
const mediump float HEIGHT = 20.0;


void main(void)
{
    gl_Position = matrixArg * vertexArg;

    v_t = t;
    bool init = false;
    for(highp int i = 0; i < RECTANGLES; i++)
    {
        mediump vec4 pos = i < 4 ? positions1 : positions2;
        highp int i1 = i  - (i / 4) * 4;
        mediump float v =
            i1 == 0
                ? pos.x
                : i1 == 1
                    ? pos.y
                    : i1 == 2
                        ? pos.z
                        : pos.w;
        v -= floor(v);
        v *= WIDTH;
        mediump float x1 = floor(v);
        v -= x1;
        v *= WIDTH;
        mediump float x2 = floor(v);
        v -= x2;
        v *= HEIGHT;
        mediump float y1 = floor(v);
        v -= y1;
        v *= HEIGHT;
        mediump float y2 = floor(v);
        mediump float left = min(x1, x2) * rectSize.x / WIDTH;
        mediump float right = (max(x1, x2) + 1.0) * rectSize.x / WIDTH;
        mediump float top = min(y1, y2) * rectSize.y / HEIGHT;
        mediump float bottom = (max(y1, y2) + 1.0) * rectSize.y / HEIGHT;
        bool insideRect = i < RECTANGLES - 1;
        if((vertexArg.x > left && vertexArg.x < right &&
                vertexArg.y > top && vertexArg.y < bottom) ||
            (!insideRect)
            )
        {
            mediump float t0 = float(i)/float(RECTANGLES);
            mediump float t1 = float(min(i + shift, RECTANGLES))/float(RECTANGLES);
            if(!init)
            {
                mediump float vt = clamp((t - t0) / (t1 - t0), 0.0, 1.0);
                v_t = vt;
            }
            init = true;
        }
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}