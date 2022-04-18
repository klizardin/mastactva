/*
    Copyright 2022

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

attribute mediump vec4 vertexAttributeFrom;
attribute mediump vec4 vertexAttributeTo;

uniform mediump mat4 renderMatrix;
uniform mediump mat4 renderFromImageMatrix;
uniform mediump mat4 renderToImageMatrix;
uniform mediump vec2 renderScreenRect;

uniform mediump float renderT;

uniform mediump float fromImage;

varying mediump vec4 texCoordVar;

void main(void)
{
    mediump vec4 vertexAttributeFromSized = vec4(
                vertexAttributeFrom.x * renderScreenRect.x,
                vertexAttributeFrom.y * renderScreenRect.y,
                vertexAttributeFrom.z,
                vertexAttributeFrom.w
                );
    mediump vec4 vertexAttributeToSized = vec4(
                vertexAttributeTo.x * renderScreenRect.x,
                vertexAttributeTo.y * renderScreenRect.y,
                vertexAttributeTo.z,
                vertexAttributeTo.w
                );
    gl_Position = renderMatrix *
            ( fromImage >= 0.5 ?
                  mix(renderVertexAttribute, vertexAttributeFromSized, renderT)
                : mix(vertexAttributeToSized, renderVertexAttribute, renderT)
             );
    texCoordVar = (fromImage >= 0.5
                   ? renderFromImageMatrix
                   : renderToImageMatrix
                   ) * renderTextureAttribute
            ;
}
