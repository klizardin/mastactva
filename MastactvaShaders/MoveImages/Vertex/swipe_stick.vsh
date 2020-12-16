/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/swipe_stick.vsh */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultvalue (-1.0, 0.0)
  @description swipe direction to swipe to the direction
  (1.0, 0.0) -- to the right
  (-1.0, 0.0) -- to the left
  (0.0, 1.0) -- to the up
  (0.0, -1.0) -- to the down
*/
uniform mediump vec2 swipeDirection;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

mediump vec2 fixSize(mediump vec2 arg_)
{
    arg_.x = arg_.x > 0.0 ? arg_.x : 1.0;
    arg_.y = arg_.y > 0.0 ? arg_.y : 1.0;
    return arg_;
}

void main(void)
{
    gl_Position = matrixArg * vertexArg;

    mediump vec4 sd = vec4(
                swipeDirection.x > 0.0 ? -1.0 : swipeDirection.x < 0.0 ? 1.0 : 0.0 ,
                swipeDirection.y > 0.0 ? -1.0 : swipeDirection.y < 0.0 ? 1.0 : 0.0 ,
                0.0,
                0.0);

    const mediump vec4 tl = vec4(0.0, 0.0, 0.0, 1.0);
    const mediump vec4 br = vec4(1.0, 1.0, 0.0, 1.0);

    mediump vec4 sds = vec4(abs(sd.x), abs(sd.y), 0.0, 1.0);
    mediump vec4 img1tl = (texMatrix1Arg * tl) * sds;
    mediump vec4 img1br = (texMatrix1Arg * br) * sds;
    mediump vec4 img2tl = (texMatrix2Arg * tl) * sds;
    mediump vec4 img2br = (texMatrix2Arg * br) * sds;
    img1tl.xy /= img1tl.w;
    img1br.xy /= img1br.w;
    img2tl.xy /= img2tl.w;
    img2br.xy /= img2br.w;

    mediump vec2 img1sz = fixSize(img1br.xy - img1tl.xy);
    mediump vec2 img2sz = fixSize(img2br.xy - img2tl.xy);
    // shift 2nd image by this value at start
    mediump float s1 = (img2br.x > 0.0 ? (img2br.x - 1.0)/img2sz.x : 0.0 ) + ( img2br.y > 0.0 ? (img2br.y - 1.0)/img2sz.y : 0.0);
    mediump float l2 = -img2tl.x/img2sz.x + -img2tl.y/img2sz.y;

    mediump float t1 = -img1tl.x/img1sz.x + -img1tl.y/img1sz.y;                     // move second image
    mediump float t2 = 1.0/img2sz.x + 1.0/img2sz.y + l2 - t1 - 1.0;                 // move both images
    mediump float t3 = s1;                                                          // move first image
    mediump float ta = t1 + t2 + t3;

    mediump float t0 = t*ta;
    if(t < t1/ta)
    {
        texCoord1Var = texMatrix1Arg * texCoordArg;
        texCoord2Var = texMatrix2Arg * (texCoordArg + (t + s1 - 1.0) * sd);
    }
    if(t < (t1+t2)/ta)
    {
        texCoord1Var = texMatrix1Arg * (texCoordArg + t * sd);
        texCoord2Var = texMatrix2Arg * (texCoordArg + (t + s1 - 1.0) * sd);
    }
    else
    {
        texCoord1Var = texMatrix1Arg * (texCoordArg + t * sd);
        texCoord2Var = texMatrix2Arg * texCoordArg;
    }
}
