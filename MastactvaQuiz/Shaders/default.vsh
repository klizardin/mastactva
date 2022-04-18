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

/*@shader @description default vertex shader*/
attribute highp vec4 renderVertexAttribute;
attribute mediump vec4 renderTextureAttribute;

uniform mediump mat4 renderMatrix;
uniform mediump mat4 renderFromImageMatrix;
uniform mediump mat4 renderToImageMatrix;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    gl_Position = renderMatrix * renderVertexAttribute;
    texCoord1Var = renderFromImageMatrix * renderTextureAttribute;
    texCoord2Var = renderToImageMatrix * renderTextureAttribute;
}
