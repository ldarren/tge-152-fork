//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GPGT Array Object Utilities ---------");
function rldarrayobj() {
exec("./egs_ArrayObject.cs");
}

////
//// Create a scriptObject named 'arrayObject' and use these methods 
//// to maintain a easily sortable array
////
function arrayObject::onAdd( %Obj )
{
   %Obj.curIndex=0;
}
function arrayObject::addEntry( %Obj , %entry )
{
   %Obj.entry[%Obj.curIndex] = %entry;
   %Obj.curIndex++;
   return %entry;
}
function arrayObject::getCount( %Obj )
{
   return %Obj.curIndex;
}
function arrayObject::sort( %Obj  , %Decreasing )
{
   if(!%Obj.curIndex) return;

   new GuiTextListCtrl(sortProxy);

   for(%count=0; %count < %Obj.curIndex; %count++)
   {
      sortProxy.addRow( %count , %Obj.entry[%count] );
   }
   
   %sortOrder = !(%Decreasing);

   sortProxy.sort( 0 , %sortOrder );

   for(%count=0; %count < %Obj.curIndex; %count++)
   {
      %Obj.entry[%count] = sortProxy.getRowText(%count);
   }

   sortProxy.delete();

}
function arrayObject::getEntry( %Obj ,  %index )
{
   if( %index >= %Obj.curIndex) return "";
   return %Obj.entry[%index];
}

