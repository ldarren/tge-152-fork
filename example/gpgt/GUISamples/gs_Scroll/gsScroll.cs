//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiScrollCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsScroll.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

function AutoScroll::onAdd( %theControl ) {
//	echo("AutoScroll::onAdd("@%theControl@")");
	%theControl.taskMgr = newTaskManager();

	%theControl.taskMgr.setTarget(%theControl);

	%theControl.taskMgr.setDefaultTaskDelay(1000); 

	%theControl.taskMgr.addTask( "scrollToBottom();", -1 );
	%theControl.taskMgr.addTask( "scrollToTop();", -1 );
}

function AutoScroll::onRemove( %theControl ) {
	%theControl.taskMgr.stopSelfExecution();
	%theControl.taskMgr.clearTasks();
	%theControl.taskMgr.delete();
}


function AutoScroll::onWake( %theControl ) {
//	echo("AutoScroll::onWake("@%theControl@")");
	%theControl.taskMgr.selfExecuteTasks( true );
}

function AutoScroll::onSleep( %theControl ) {
//	echo("AutoScroll::onSleep("@%theControl@")");
	%theControl.taskMgr.stopSelfExecution();
}


// This will never fire
function GuiScrollCtrl::onAction( %theControl ) {
	echo("GUIScrollCtrl::onAction("@%theControl@")");
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

exec("./gsScroll.gui");

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


function TestScrollMLText::onWake( %theControl ) {
	%theControl.setValue(""); // Clear it

	%file = new FileObject();

	%fileName = expandFileName( "./TestScrollMLTextContent.txt" );

	echo( "Attempt to open " , %fileName );

	%fileIsOpen = %file.openForRead( %fileName );

	echo( "Open for read " , (%fileIsOpen ? "succeeded" : "failed" ) );

	if( %fileIsOpen ) {
		while(!%file.isEOF()) {

			%currentLine = %file.readLine();

			echo(%currentLine);

			%theControl.addText( %currentLine, force );

		}
	}

	if( %theControl.isVisible() ) %theControl.forceReflow();

	%file.close();
	%file.delete();

}



