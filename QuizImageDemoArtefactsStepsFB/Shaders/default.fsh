/*@shader @description default fragment shader. mix to images*/
uniform sampler2D renderFromImage;
uniform sampler2D renderToImage;
uniform lowp float renderOpacity;

uniform mediump float renderT;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    mediump vec4 s1 = texture2D( renderFromImage, texCoord1Var.st );
    mediump vec4 s2 = texture2D( renderToImage, texCoord2Var.st );
    gl_FragColor = mix( vec4( s1.r, s1.g, s1.b, s1.a ),
                        vec4( s2.r, s2.g, s2.b, s2.a ),
                        renderT ) * renderOpacity;
}
