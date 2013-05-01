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