/*@shader @description default vertex shader*/
attribute highp vec4 renderVertexAttribute;
attribute mediump vec4 renderTextureAttribute;

uniform mediump mat4 renderMatrix;
uniform mediump mat4 renderFromImageMatrix;
uniform mediump mat4 renderToImageMatrix;

varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    gl_Position = renderMatrix * renderVertexAttribute;
    texCoord1Var = renderFromImageMatrix * renderTextureAttribute;
    texCoord2Var = renderToImageMatrix * renderTextureAttribute;
}
