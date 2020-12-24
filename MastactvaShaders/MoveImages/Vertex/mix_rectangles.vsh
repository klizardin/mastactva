/*@shader @description default vertex shader to mix t in 5 rectangles
path: MastactvaShaders/MoveImages/Vertex/mix_rectangles.vsh
@geomentryFaced
@geomentrySize (10,10)
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
uniform mediump vec4 positions;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


const highp int RECTANGLES = 5;


void main(void)
{
    gl_Position = matrixArg * vertexArg;

    v_t = t;
    for(int i = 0; i < RECTANGLES; i++)
    {
        mediump float v =
                i == 0
                    ? positions.x
                    : i == 1
                        ? positions.y
                        : i == 2
                            ? positions.z
                            : i == 3
                                ? positions.w
                                : 0.90091;
        v -= floor(v);
        v *= 10.0;
        mediump float p1 = floor(v);
        v -= p1;
        v *= 10.0;
        mediump float p2 = floor(v);
        v -= p2;
        v *= 10.0;
        mediump float p3 = floor(v);
        v -= p3;
        v *= 10.0;
        mediump float p4 = floor(v);
        mediump float left = min(p1, p2) * rectSize.x * 0.1;
        mediump float right = (max(p1, p2) + 1.0) * rectSize.x * 0.1;
        mediump float top = min(p3, p4) * rectSize.y * 0.1;
        mediump float bottom = (max(p3, p4) + 1.0) * rectSize.y * 0.1;
        if(vertexArg.x >= left && vertexArg.x <= right && vertexArg.y >= top && vertexArg.y <= bottom)
        {
            mediump float t0 = float(i)/float(RECTANGLES);
            v_t = clamp((t - t0) / (1.0 - t0), 0.0, 1.0);
            break;
        }
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
