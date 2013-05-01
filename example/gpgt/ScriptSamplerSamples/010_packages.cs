// 
// Packages
//
function demo()
{
   echo("Demo definition 0. \n\n");   
}

package MyPackage1
{
   function demo()
   {
      echo("Demo definition 1. \n\n");   
   }
};

package MyPackage2
{
   function demo()
   {
      echo("Demo definition 2. \n\n");   
   }
};

demo();

ActivatePackage( MyPackage1 );
demo();

ActivatePackage( MyPackage2 );
demo();

DeactivatePackage( MyPackage1 );
demo();




