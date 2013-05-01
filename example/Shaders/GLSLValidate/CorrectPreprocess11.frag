#if 0 
    
#define f 1.2

#elif 1
    
#define f 1.4

#endif



#if 2
    
#define q 8.9

#endif




#if 0 
these tokens will be ignored

#else tokens to be ignored
#define f 4.5

#endif



#if 1

#else

#endif



#define pine


#ifdef pine please raise a warning

#else

#endif



#ifndef pine

ignore these tokens too

#elif 1 eat these tokens
#define f 3.4

#endif



void main()

{
    
    gl_FragColor = vec4(f);

}
