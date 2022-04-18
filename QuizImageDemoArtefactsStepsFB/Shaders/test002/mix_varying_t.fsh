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

/*@shader @description default fragment shader to view two images at same time
path: MastactvaShaders/MoveImages/Vertex/mix_varying_t.fsh */
uniform sampler2D renderFromImage;
uniform sampler2D renderToImage;
uniform lowp float renderOpacity;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


void main(void)
{
    mediump vec4 s1 = texture2D( renderFromImage, texCoord1Var.st );
    mediump vec4 s2 = texture2D( renderToImage, texCoord2Var.st );
    mediump float vt = clamp(v_t, 0.0, 1.0);
    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, s1.a ),
                        vec4( s2.r, s2.g, s2.b, s2.a ),
                        vt ) * renderOpacity;
}
