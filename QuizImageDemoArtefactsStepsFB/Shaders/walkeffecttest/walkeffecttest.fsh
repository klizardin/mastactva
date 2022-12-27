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

/*@shader @description default fragment shader. mix to images*/
uniform sampler2D renderFromImage;
uniform sampler2D renderToImage;
uniform lowp float renderOpacity;

uniform mediump float renderT;
uniform mediump float fromImage;

varying mediump vec4 texCoordVar;

void main(void)
{
    mediump vec4 s1 = fromImage >= 0.5 ?
                texture2D( renderFromImage, texCoordVar.st )
              : texture2D( renderToImage, texCoordVar.st )
              ;
    gl_FragColor = vec4( s1.r, s1.g, s1.b,
                         fromImage >= 0.5 ? 1.0 - renderT : renderT
                        ) * renderOpacity;
}
