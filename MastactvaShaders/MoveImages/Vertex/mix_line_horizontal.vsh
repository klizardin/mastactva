/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/mix_line_horizontal.vsh
@geomentrySize (10,10)
*/
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue 0.05
    @description k of v_t line
*/
uniform mediump float slope;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


void main(void)
{
    gl_Position = matrixArg * vertexArg;
    mediump float szl = length(rectSize);
    mediump float x0 = rectSize.x * 0.5 - (szl * 0.5 + szl*slope);
    mediump float x1 = rectSize.x * 0.5 + (szl * 0.5 + szl*slope);
    mediump float x = mix(x0, x1, t);
    v_t = -(vertexArg.x - x)/(szl*slope);
    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
