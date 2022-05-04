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
uniform sampler2D renderToImage;
uniform lowp float renderOpacity;

uniform mediump float renderT;

varying mediump vec4 texCoordVar;

void main(void)
{
    mediump vec4 s1 = texture2D( renderToImage, texCoordVar.st );
    if(s1.a <= 1e-5)
    {
        discard;
    }
    gl_FragColor = vec4(
                s1.r * renderOpacity, // * renderT,
                s1.g * renderOpacity, // * renderT,
                s1.b * renderOpacity, // * renderT,
                0.5 // renderT // 0.5
                );
}
