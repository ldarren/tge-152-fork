//-----------------------------------------------------------------------------
// Torque Game Engine
//
// Copyright (c) 2001 GarageGames.Com
// Portions Copyright (c) 2001 by Sierra Online, Inc.
//-----------------------------------------------------------------------------

function initRPGDialogEditor()
{
   exec("~/ui/MainEditorScreenGui.gui");
   exec("~/ui/EditQuestionGui.gui");
   exec("~/ui/EditAnswerGui.gui");
   exec("~/ui/NewScriptPopup.gui");
   exec("~/ui/SetPathsPopup.gui");
   exec("~/ui/EditorOpeningGui.gui");

   exec("~/defaults.cs");
   exec("~/prefs.cs");
   exec("~/editorMain.cs");
   
   PopulateActionList();
   PopulateQuestionOptionsList();
   GlobalActionMap.bind(keyboard, "f6", toggleRPGDialogEditor);
}

function openRPGDialogEditor()
{
   $GuiBeforeRPGDialogEditor=Canvas.getContent();
   if(TextScript.getvalue()$="Current Q. Script:")
      Canvas.setContent(EditorOpeningGui);
   else
      Canvas.setContent(MainEditorScreenGui);
   Canvas.setCursor("DefaultCursor");
}

function closeRPGDialogEditor()
{
   Canvas.setContent($GuiBeforeRPGDialogEditor);
   Canvas.setCursor("DefaultCursor");
}


function toggleRPGDialogEditor(%val)
{
   if (%val)
   {
      if (Canvas.getContent() == MainEditorScreenGui.getId() ||
          Canvas.getContent() == EditQuestionGui.getId() ||
          Canvas.getContent() == EditAnswerGui.getId() ||
          Canvas.getContent() == EditorOpeningGui.getId())
         closeRPGDialogEditor();
      else
         openRPGDialogEditor();
   }
}


