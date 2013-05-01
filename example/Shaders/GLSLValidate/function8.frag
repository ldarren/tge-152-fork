void function(const inout int i)  
{  // inout parameters cannot be const
   i = 3;
}

void main()
{
    int i;
    function(i);  
}


