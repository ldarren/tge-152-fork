
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX - Object Selection Management
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Object Selection

function GameConnection::onObjectSelected(%this, %obj)
{
  TargetStatusbarLabel.setName(%obj.getShapeName());

  TargetStatusbar.visible = true;
  TargetHealthStatusBar.setShape(%obj);
  TargetEnergyStatusBar.setShape(%obj);

  if (%obj.selectron == 0 && %obj != %this.getControlObject())
  {
    %selectron = startSelectron(%obj, $afxCurrentSelectronStyle);
    if (%selectron != 0)
      %selectron.addConstraint(%obj, "selected");
    %obj.selectron = %selectron;
  }  
}

function GameConnection::onObjectDeselected(%this, %obj)
{
  TargetStatusbar.visible = false;
  TargetHealthStatusBar.clearShape();
  TargetEnergyStatusBar.clearShape();

  if (%obj.selectron != 0 &&isObject(%obj.selectron))
  {
    %obj.selectron.stopSelectron();
    %obj.selectron = 0;
  }
}

function GameConnection::resetSelection(%this)
{
  %sel_obj = %this.getSelectedObj();
  if (%sel_obj != -1)
     %this.setSelectedObj(%sel_obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Selectron Management

$afxCurrentSelectronStyle = 0;

function resetSelectronStyle()
{
  $afxCurrentSelectronStyle = 0;
}

function gotoNextSelectronStyle()
{
  commandToServer('NextSelectronStyle', $afxCurrentSelectronStyle, false, true);
}

function gotoPreviousSelectronStyle()
{
  commandToServer('NextSelectronStyle', $afxCurrentSelectronStyle, true, true);
}

function clientCmdUpdateSelectronStyle(%style_name, %style_id, %display_msg)
{
  if (%display_msg)
  {
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("Now using" SPC %style_name SPC "style selectrons.");
  }

  $afxCurrentSelectronStyle = %style_id;

  ServerConnection.resetSelection();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


