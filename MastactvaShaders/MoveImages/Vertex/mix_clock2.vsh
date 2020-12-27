/*@shader @description default vertex shader to mix t by 2 clocks
path: MastactvaShaders/MoveImages/Vertex/mix_clock2.vsh
@geomentrySize (10,10)
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
uniform mediump vec3 clockPos1;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec3 clockPos2;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue rand(0.01, 0.25)
    @description k of v_t line
*/
uniform mediump float slope;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump vec2 v_t;


const mediump float M_PI = 3.14159265359;
const mediump float MIN_SLOPE = 0.01;


mediump float atan2(in mediump float y, in mediump float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI*0.5 - atan(x,y), atan(y,x),  s ? 1.0 : 0.0);
}

void main(void)
{
    gl_Position = matrixArg * vertexArg;

    mediump float slopeNorm = 0.2; //slope < MIN_SLOPE ? MIN_SLOPE : slope;
    slopeNorm = slopeNorm * (0.275 - (t - 0.5)*(t - 0.5));
    highp vec4 vertex = vec4(
                vertexArg.x/vertexArg.w,
                vertexArg.y/vertexArg.w,
                0.0,
                1.0);

    highp vec2 rp1 = vertex.xy - vec2(clockPos1.x * rectSize.x, clockPos1.y * rectSize.y); // 
    mediump float l1 = length(rp1);
    if(l1 == 0.0)
    {
        v_t.x = t;
    }
    else
    {
        rp1.xy /= l1;
        mediump float a10 = clockPos1.z * M_PI * 2.0; //
        highp vec2 pt0 = vec2(cos(a10), sin(a10));
        mediump float a1t = t * M_PI * 2.0 * (1.0 + slopeNorm) + a10;  //  
        highp vec2 pt = vec2(cos(a1t), sin(a1t)); 
        mediump float a = rp1.x*pt.x + rp1.y*pt.y;
        a = acos(a <= 1.0 ? a >= -1.0 ? a : -1.0 : 1.0);
        mediump float z = rp1.x * pt.y - rp1.y * pt.x;
        a = z < 0.0 ? - a :  a;
        mediump float a0 = rp1.x*pt0.x + rp1.y*pt0.y;
        a0 = acos(a0 <= 1.0 ? a0 >= -1.0 ? a0 : -1.0 : 1.0);
        mediump float z0 = rp1.x * pt0.y - rp1.y * pt0.x;
        a0 = z0 > 0.0 ? M_PI * 2.0 - a0 :  a0;
        mediump float tpt = a0 / (M_PI * 2.0);
        mediump float vt1 = a / (M_PI * 2.0 * slopeNorm);
        v_t.x = vt1 >= -5.0 && vt1 <= 5.0
                         ? vt1
                         : tpt < t
                           ? 5.0
                           : -5.0;
        //v_t1 = clamp(vt1, -5.0, 5.0);
    }

    highp vec2 rp2 = vertex.xy - vec2(clockPos2.x * rectSize.x, clockPos2.y * rectSize.y);
    mediump float l2 = length(rp2);
    if(l2 == 0.0)
    {
        v_t.y = t;
    }
    else
    {
        rp2.xy /= l2;
        mediump float a10 = clockPos2.z * M_PI * 2.0;
        highp vec2 pt0 = vec2(cos(a10), sin(a10));
        mediump float a1t = t * M_PI * 2.0 * (1.0 + slopeNorm) + a10;  //  
        highp vec2 pt = vec2(cos(a1t), sin(a1t)); 
        mediump float a = rp2.x*pt.x + rp2.y*pt.y;
        a = acos(a <= 1.0 ? a : 1.0);
        mediump float z = rp2.x * pt.y - rp2.y * pt.x;
        a = z < 0.0 ? - a :  a;
        mediump float a0 = rp2.x*pt0.x + rp2.y*pt0.y;
        a0 = acos(a0 <= 1.0 ? a0 : 1.0);
        mediump float z0 = rp2.x * pt0.y - rp2.y * pt0.x;
        a0 = z0 > 0.0 ? M_PI * 2.0 - a0 :  a0;
        mediump float tpt = a0 / (M_PI * 2.0);
        mediump float vt2 = a / (M_PI * 2.0 * slopeNorm);
        v_t.y = vt2 >= -5.0 && vt2 <= 5.0
                         ? vt2 
                         : tpt < t
                           ? 5.0
                           : -5.0;
        //v_t2 = vt2;
    }

    //v_t2 = 1.0;

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
