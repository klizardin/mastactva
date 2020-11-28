/*  @shader
    @description
    test 001 fragment shader
*/

uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;
uniform mediump float t;

/*  @argument
    @defaultvalue (0.1, 1.0, 0.1)
    @description color number 1 for
    texture number 1 transform color
*/

uniform mediump vec3 color1;
uniform mediump vec3 color2; /*< @argument
    @defaultvalue (0.1, 0.1, 1.0)
    @description color number 2 for
    texture number 2 transform color
*/
varying mediump vec4 texCoord;

void main(void)
{
    vec4 s1 = texture2D( texture1Arg, texCoord.st );
    vec4 s2 = texture2D( texture2Arg, texCoord.st );
    gl_FragColor = mix( vec4( s1.r*color1.r, s1.g*color1.g, s1.b*color1.b, 1.0 ),
                        vec4( s2.r*color2.r, s2.g*color2.g, s2.b*color2.b, 1.0 ),
                        t );
}
