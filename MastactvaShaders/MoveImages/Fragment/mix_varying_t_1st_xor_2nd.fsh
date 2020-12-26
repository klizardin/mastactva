/*@shader @description default fragment shader to view two images at same time
path: MastactvaShaders/MoveImages/Vertex/mix_varying_t_1st_xor_2nd.fsh */
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
    mediump float vt = clamp(v_t, 0.0, 1.0);
    if(vt <= 0.0)
    {
        gl_FragColor = s1 * opacityArg;
    }
    else if(vt >= 1.0)
    {
        gl_FragColor = s2 * opacityArg;
    }
    else
    {
        gl_FragColor = mix(s1, vec4(1.0 - s2.r, 1.0 - s2.g, 1.0 - s2.b, s2.a ), v_t) * opacityArg;
    }
}
