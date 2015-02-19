vec4 packFloat8bitRGBA(float val)
{
    vec4 pack = vec4(1.0, 255.0, 65025.0, 16581375.0) * val;
    pack = fract(pack);
    pack -= vec4(pack.yzw / 255.0, 0.0);

    return pack;
}

float unpackFloat8bitRGBA(vec4 pack)
{
    return dot(pack, vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 16581375.0));
}

vec3 packFloat8bitRGB(float val)
{
    vec3 pack = vec3(1.0, 255.0, 65025.0) * val;
    pack = fract(pack);
    pack -= vec3(pack.yz / 255.0, 0.0);

    return pack;
}

float unpackFloat8bitRGB(vec3 pack)
{
    return dot(pack, vec3(1.0, 1.0 / 255.0, 1.0 / 65025.0));
}

vec2 packFloat8bitRG(float val)
{
    vec2 pack = vec2(1.0, 255.0) * val;
    pack = fract(pack);
    pack -= vec2(pack.y / 255.0, 0.0);

    return pack;
}

float unpackFloat8bitRG(vec2 pack)
{
    return dot(pack, vec2(1.0, 1.0 / 255.0));
}
