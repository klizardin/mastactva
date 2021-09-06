/*
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

/*@shader @description default vertex shader to mix 2 eclipses in 2 steps
path: MastactvaShaders/MoveImages/Vertex/mix_elipse2_2steps.vsh
@geomentryFaced
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
uniform mediump vec2 elipsePos1;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 elipsePos2;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue rand(0.02, 0.2)
    @description k of v_t line
*/
uniform mediump float slope;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


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

    mediump float slope1 = slope < MIN_SLOPE ? MIN_SLOPE : slope;
    mediump float szl = length(rectSize);
    mediump float x1 =  szl*(1.0 + slope1);
    mediump float x0 =  -x1;
    if(t < 0.5)
    {
        highp vec4 vertexCoord = vec4(
                    vertexArg.x/vertexArg.w - elipsePos1.x * rectSize.x,
                    vertexArg.y/vertexArg.w - elipsePos1.y * rectSize.y,
                    0.0,
                    1.0);
        mediump float x = mix(x0, x1, t * 2.0);
        mediump float posLength1 = length(vertexCoord);
        v_t = -(posLength1 - x)/(szl*slope1);
    }
    else
    {
        highp vec4 vertexCoord = vec4(
                    vertexArg.x/vertexArg.w - elipsePos2.x * rectSize.x,
                    vertexArg.y/vertexArg.w - elipsePos2.y * rectSize.y,
                    0.0,
                    1.0);
        mediump float x = mix(x0, x1, (t - 0.5) * 2.0);
        mediump float posLength2 = length(vertexCoord);
        v_t = -(posLength2 - x)/(szl*slope1);
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
