float intensity(vec4 color)
{
  return sqrt((color.x * color.x) + (color.y * color.y) + (color.z * color.z));
}

bool sobel(sampler2D texture, vec2 center, float step, float limit)
{
  // get samples around pixel
  float tleft = intensity(texture2D(texture, center + vec2(-step, step)));
  float left = intensity(texture2D(texture, center + vec2(-step, 0)));
  float bleft = intensity(texture2D(texture, center + vec2(-step, -step)));
  float top = intensity(texture2D(texture, center + vec2(0, step)));
  float bottom = intensity(texture2D(texture, center + vec2(0, -step)));
  float tright = intensity(texture2D(texture, center + vec2(step, step)));
  float right = intensity(texture2D(texture, center + vec2(step, 0)));
  float bright = intensity(texture2D(texture, center + vec2(step, -step)));

  // Sobel masks (to estimate gradient)
  //        1 0 -1     -1 -2 -1
  //    X = 2 0 -2  Y = 0  0  0
  //        1 0 -1      1  2  1
  float x = tleft + 2.0 * left + bleft - tright - 2.0 * right - bright;
  float y = -tleft - 2.0 * top - tright + bleft + 2.0 * bottom + bright;
  float color = sqrt((x * x) + (y * y));
  
  if (color > limit)
    return true;

  return false;
}