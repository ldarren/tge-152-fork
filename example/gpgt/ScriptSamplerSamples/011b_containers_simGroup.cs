// 
// Containers - Sim Groups
//

%S0     = new SimObject();

%Group0 = new SimGroup();

%Group1 = new SimGroup();

%Set0   = new SimSet();

%Set0.add( %S0 );

%Group0.add( %S0 );

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );

echo( "Group 0 contains ", %Group0.getCount() , " objects.\n" );

echo( "Group 1 contains ", %Group1.getCount() , " objects.\n" );

%Group1.add( %S0 );

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );

echo( "Group 0 contains ", %Group0.getCount() , " objects.\n" );

echo( "Group 1 contains ", %Group1.getCount() , " objects.\n" );



%Group1.delete(); // Self deletes, and automatically deletes %S0

echo( "Set 0 contains ", %Set0.getCount() , " objects.\n" );

%Group0.delete();
%Set0.delete();



