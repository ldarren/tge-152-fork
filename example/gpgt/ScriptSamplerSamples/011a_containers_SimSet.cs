// 
// Containers - Sim Sets
//

%S0   = new SimObject();

%Set0 = new SimSet();

%Set1 = new SimSet();

%Set0.add( %S0 );

%Set0.add( %S0 );

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );


%Set1.add( %S0 );

%Set1.add( %Set0 );

echo( "Set 1 contains ", %Set1.getCount() , " objects.\n" );


%Set1.delete(); 

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );


%S0.delete();

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );


%Set0.delete(); 




