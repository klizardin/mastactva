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

/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/mix_line_horizontal.vsh
@geomentrySize (10,10)
*/
attribute highp vec4 renderVertexAttribute;
attribute mediump vec4 renderTextureAttribute;

uniform mediump mat4 renderMatrix;
uniform mediump mat4 renderFromImageMatrix;
uniform mediump mat4 renderToImageMatrix;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 renderScreenRect;

/*@argument
    @defaultValue 0.05
    @description k of v_t line
*/
uniform mediump float slope;

uniform mediump float renderT;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


void main(void)
{
    gl_Position = renderMatrix * renderVertexAttribute;
    mediump float szl = length(renderScreenRect);
    mediump float x0 = renderScreenRect.x * 0.5 - (szl * 0.5 + szl*slope);
    mediump float x1 = renderScreenRect.x * 0.5 + (szl * 0.5 + szl*slope);
    mediump float x = mix(x0, x1, renderT);
    v_t = -(renderVertexAttribute.x - x)/(szl*slope);
    texCoord1Var = renderFromImageMatrix * renderTextureAttribute;
    texCoord2Var = renderToImageMatrix * renderTextureAttribute;
}
