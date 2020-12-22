/*@shader @description default fragment shader to view two images at same time
path: MastactvaShaders/MoveImages/Vertex/mix_varying_t.fsh */
uniform sampler2D texture1Arg;
uniform sampler2D texture2Arg;
uniform lowp float opacityArg;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


void main(void)
{
    mediump vec4 s1 = texture2D( texture1Arg, texCoord1Var.st );
    mediump vec4 s2 = texture2D( texture2Arg, texCoord2Var.st );
    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, s1.a ),
                        vec4( s2.r, s2.g, s2.b, s2.a ),
                        clamp(v_t, 0.0, 1.0)) * opacityArg;
}
