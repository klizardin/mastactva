/*@shader @description default fragment shader to view two images at same time, but prefere second*/
uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;
uniform lowp float opacityArg;

uniform mediump float t;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;


void main(void)
{
    vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );
    vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );
    if(s2.a > 0)
    {
        gl_FragColor = vec4( s2.r, s2.g, s2.b, s2.a ) * opacityArg;
    }
    else if(s1.a > 0)
    {
        gl_FragColor = vec4( s1.r, s1.g, s1.b, s1.a ) * opacityArg;
    }
    else
    {
        discard;
    }
}
