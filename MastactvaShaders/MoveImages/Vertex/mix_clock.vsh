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
  @defaultValue rand(0.49, 0.51)
  @description translate value from
*/
uniform mediump vec2 clockPos1;

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
varying mediump float v_t;


const mediump float M_PI = 3.14159265359;
const mediump float MIN_SLOPE = 0.01;
const mediump float MINA = M_PI / 120.0;
const mediump float MAXA = 2.0 * M_PI - MINA;


mediump float atan2(in mediump float y, in mediump float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI*0.5 - atan(x,y), atan(y,x),  s ? 1.0 : 0.0);
}

mediump float sqr(in mediump float a)
{
    return a* a;
}

mediump float cube(in mediump float a)
{
    return a * sqr(a);
}

void main(void)
{
    gl_Position = matrixArg * vertexArg;

    mediump float slopeNorm = slope < MIN_SLOPE ? MIN_SLOPE : slope;
    highp vec4 vertex = vec4(
                vertexArg.x/vertexArg.w,
                vertexArg.y/vertexArg.w,
                0.0,
                1.0);

    highp vec2 rp1 = vertex.xy - vec2(clockPos1.x * rectSize.x, clockPos1.y * rectSize.y);
    mediump float l1 = length(rp1);
    mediump float k = t * M_PI * 2.0;
    if(l1 == 0.0 || k >= MAXA)
    {
        v_t = t;
    }
    else
    {
        k = clamp(k, MINA, MAXA);
        mediump float a = atan2(rp1.y, rp1.x) + M_PI;
        if(a < M_PI)
        {
            a = a * k / M_PI;
        }
        else
        {
            a = k + (a / M_PI - 1.0) * (2.0 * M_PI - k);
        }
        v_t = -2.0*sin(a);
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
