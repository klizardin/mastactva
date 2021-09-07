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

/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/mix_direction.vsh
@geomentrySize (10,10)
*/
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue rand(-1.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 direction1;

/*@argument
  @defaultValue rand(-1.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 direction2;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue rand(0.02, 0.1)
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

    highp vec4 cubeVertex = vec4(
                vertexArg.x/vertexArg.w - rectSize.x * 0.5,
                vertexArg.y/vertexArg.w - rectSize.y * 0.5,
                0.0,
                1.0);

    mediump float a1 = atan2(
                -direction1.y,
                direction1.x == 0.0 && direction1.y == 0.0 ? 1.0 : direction1.x);
    mediump mat4 rotateMat1 = mat4(
                vec4(cos(a1), -sin(a1), 0.0, 0.0),
                vec4(sin(a1), cos(a1), 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
    highp vec4 cubeVertex1 = rotateMat1 * cubeVertex;

    mediump float a2 = atan2(
                -direction2.y,
                direction2.x == 0.0 && direction2.y == 0.0 ? 1.0 : direction2.x);
    a2 = a1 - a2 * 0.25 + M_PI * 0.5;
    mediump mat4 rotateMat2 = mat4(
                vec4(cos(a2), -sin(a2), 0.0, 0.0),
                vec4(sin(a2), cos(a2), 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
    highp vec4 cubeVertex2 = rotateMat2 * cubeVertex;

    mediump float slope1 = slope < MIN_SLOPE ? MIN_SLOPE : slope;
    mediump float szl = length(rectSize);
    mediump float x1 =  szl*(1.0 + slope1);
    mediump float x0 =  -x1;
    mediump float x = mix(x0, x1, t);

    v_t.x = -(cubeVertex1.x - x)/(szl*slope1);
    v_t.y = -(cubeVertex2.x - x)/(szl*slope1);

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
