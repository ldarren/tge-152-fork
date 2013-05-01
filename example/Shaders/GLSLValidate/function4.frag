uniform int uniformInt;

void function(out int i)  
{
    i = 1; 
}

void main()
{
    function(uniformInt);  // out and inout parameters cannot be uniform since uniforms cannot be modified
}

