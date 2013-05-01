void function(const int i)  
{
    i = 3;  // const value cant be modified
}

void main()
{
    int i;
    function(i);
}


