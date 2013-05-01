// ============================================================
// Project            :  VR20
// File               :  .\LTA\client\scripts\inputBoxGui.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Wednesday, July 04, 2007 2:48 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function InputBox(%msg, %callback)
{
	InputBoxOKBtn.command = "InputBoxOkPressed(\""@%callback@"\");Canvas.popDialog(InputBoxGui);";
	InputBoxDisplay.text = %msg;
	Canvas.pushDialog(InputBoxGui);
}

function InputBoxOkPressed(%callback)
{
	%newCallBack = %callback@"(\""@InputBoxInput.getText()@"\");";
	eval(%newCallBack);
}