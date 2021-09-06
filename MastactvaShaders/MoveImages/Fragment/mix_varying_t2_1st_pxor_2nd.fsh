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

/*@shader @description default fragment shader to view two images at same time
path: MastactvaShaders/MoveImages/Vertex/mix_varying_t2_1st_pxor_2nd.fsh */
uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;
uniform lowp float opacityArg;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump vec2 v_t;


void main(void)
{
    mediump vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );
    mediump vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );
    mediump vec2 vt = clamp(v_t, 0.0, 1.0);
    if(vt.x > 0.0 && vt.x < 1.0 || vt.y > 0.0 && vt.y < 1.0)
    {
        gl_FragColor = mix(s1, vec4(1.0 - s2.r, 1.0 - s2.g, 1.0 - s2.b, s2.a ), max(vt.x, vt.y)) * opacityArg;
    }
    else if(vt.x <= 0.0 && vt.y <= 0.0)
    {
        gl_FragColor = s1 * opacityArg;
    }
    else
    {
        gl_FragColor = s2 * opacityArg;
    }
}
