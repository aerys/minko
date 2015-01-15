#ifdef VERTEX_SHADER
void main(void)
{
    gl_Position = vec4(1.0);
}
#endif

#ifdef FRAGMENT_SHADER
void main(void)
{
    gl_FragColor = vec4(1.0);
}
#endif
