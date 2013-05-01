vec2 func()
{
    vec2 v;
    return v;
}

void main()
{
    const vec3 v = vec3(1.0, func()); // user defined functions do not return const value
    gl_FragColor = vec4(v, v);
}
