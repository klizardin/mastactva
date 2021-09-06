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

/*@shader @description fragment shader to varying t by 2 steps through color
path: MastactvaShaders/MoveImages/Vertex/mix_varying_t2_or_2steps_through_color.fsh */
uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;
uniform lowp float opacityArg;

/*@argument
    @defaultValue (0.0, 0.0, 0.0, 1.0)
    @description bacground color
*/
uniform mediump vec4 color;

uniform mediump float t;


varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump vec2 v_t;


void main(void)
{
    mediump vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );
    mediump vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );
    mediump float vt = v_t.x >= 0.0 || v_t.y >= 0.0 ? max(clamp(v_t.x, 0.0, 1.0), clamp(v_t.y, 0.0, 1.0)) : 0.0;
    if(t < 0.5)
    {
        gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, s1.a ),
                            color,
                            vt ) * opacityArg;
    }
    else
    {
        gl_FragColor = mix( color,
                            vec4( s2.r, s2.g, s2.b, s2.a ),
                            vt ) * opacityArg;
    }
}
