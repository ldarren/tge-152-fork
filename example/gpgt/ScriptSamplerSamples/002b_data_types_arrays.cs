// 
// Data Types - Arrays
//

%vec[0] = 10;
%vec[1] = 20;

echo(   "\n\n  1D Arrays: %vec[0]   == " ,  %vec[0] );
 



%vec[0,0] = 100;

echo( "\n  2D Arrays: %vec[0,0] == " ,  %vec[0,0]);


//
// %vec[0,0] != %vec[0]
//

%result = ( %vec[0,0] == %vec[0] );

echo("\n\n %vec[0,0] == %vec[0]  ==> " , %result );



