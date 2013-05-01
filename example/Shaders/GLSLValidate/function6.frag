void function(const out int i)  
{  // out parameters cannot be const
   i = 3;
}

void main()
{
    int i;
    function(i);  
}


