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

varying mediump vec4 texCoordFromVar;
varying mediump vec4 texCoordToVar;

void main(void)
{
    mediump vec4 sfrom = texture2D( renderFromImage, texCoordFromVar.st );
    mediump vec4 sto = texture2D( renderToImage, texCoordToVar.st );
    if(sfrom.a <= 1e-5 && sto.a <= 1e-5)
    {
        discard;
    }
    else if(sfrom.a <= 1e-5)
    {
        gl_FragColor = vec4(
                sto.rgb * renderOpacity,
                1.0);
    }
    else if(sto.a <= 1e-5)
    {
        gl_FragColor = vec4(
                sfrom.rgb * renderOpacity,
                1.0);
    }
    else
    {
        mediump float t = ((exp(10.0*renderT - 5.0) - exp(-10.0*renderT + 5.0))/(exp(10.0*renderT - 5.0) + exp(-10.0*renderT + 5.0)) + 1.0) * 0.5;
        gl_FragColor = vec4(
            mix(sfrom.rgb, sto.rgb, t) * renderOpacity,
            1.0);
    }
}

