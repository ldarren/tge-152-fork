void function(in int i);  

void main()
{
    float f;
    // overloaded function not present
    function(f);  
}

void function(in int i)  
{  
   i = 3;
}
