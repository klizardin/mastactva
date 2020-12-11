/*@shader @description default fragment shader. mix to images*/
uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;

uniform mediump float t;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );
    vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );
    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, s1.a ),
                        vec4( s2.r, s2.g, s2.b, s2.a ),
                        t );
}
