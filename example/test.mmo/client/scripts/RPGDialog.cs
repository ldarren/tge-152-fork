//-----------------------------------------------------------------------------
// RPGDialog - Presents the player with a question and several answers to select from
// Created by Nelson A. K. Gonsalves
//-----------------------------------------------------------------------------
   
function getQuestion(%questionFile,%questionNumber)
{
	%file = new FileObject();
	if(isFile($Pref::RPGDialog::Client::QuestionPath@%questionFile@".dlq") && %file.openForRead($Pref::RPGDialog::Client::QuestionPath@%questionFile@".dlq"))
	{
		for(%i=1;%i<%questionNumber;%i++)
		{
			%file.readLine();
			if(%file.isEOF())
			{
				%file.close();
				%file.delete();
				return "<Invalid>";
			}
		}
		%question=%file.readLine();
	}
	else
	{
		%file.delete();
		return "<Invalid>";
	}
	
	%file.close();
	%file.delete();
	return %question;
}

function clientCmdCloseRPGDialog()
{
   Canvas.popDialog(RPGDialog);
   RPGDialogQuestion.settext("");
   RPGDialogAnswer.settext("");
}

function clientCmdRPGDialogMessage(%sender,%senderName,%portrait,%npcFile,%questionNumber,%playerName,%customMsg)
{
   $RPGDialog::Sender=%sender;
   $RPGDialog::questionNumber=%questionNumber;
   onRPGDialogMessage(%npcFile,%questionNumber,%senderName,%portrait,%playerName,%customMsg);
}

function onRPGDialogMessage(%npcFile,%questionNumber,%senderName,%portrait,%playerName,%customMsg)
{
    
    if(%npcFile$="")
    {
		error("onRPGDialogMessage() received an invalid question file");
		return;
	}
	
	%QuestionAnswer=GetQuestion(%npcfile,%questionNumber);
	if(%QuestionAnswer!$="<Invalid>")
	{
		%QuestionAnswer=strReplace(%QuestionAnswer,"<<Name>>",%senderName);
		%QuestionAnswer=strReplace(%QuestionAnswer,"<<PlayerName>>",%playerName);
		if (%customMsg!$="") %QuestionAnswer=strReplace(%QuestionAnswer,"<<Custom>>",%customMsg);
		%QuestionAnswer=strReplace(%QuestionAnswer,"<BR>","\n");
		
		%AnswerStart=strPos(%QuestionAnswer,"<AnswerStart>");
		%question=getSubStr(%QuestionAnswer,0,%AnswerStart);
		%answer=getSubStr(%QuestionAnswer,%AnswerStart+13,strLen(%QuestionAnswer));
	}
	else
	{
		error("ERROR::Invalid Question!!\nnpcFile = "@%npcFile@"\nquestionNumber = "@%questionNumber);
		return;
	}
    
    
    if (%question!$="")
    {
		%attrOffset = 0;
		if ((%attrOffset = setRPGDialogImage(%question, %portrait)) != -1) %question = getSubStr(%question, 0, %attrOffset);
		if ((%attrOffset = playRPGDialogSound(%question)) != -1) %question = getSubStr(%question, 0, %attrOffset);
		
		RPGDialogQuestion.settext(%question);
		ChatHud.addLine($Pref::RPGDialog::ChatHudQuestionColor@%senderName@": "@StripMLControlChars(%question));
    }

    if (%answer!$="")
    {
       %line=%answer;
       %i=1;
       while(%i<=$Pref::RPGDialog::MaxOptions) //lets number the options
       {
          %Start=strpos(%line,"<a:RPGDialog "@%i@">");

          if(%Start<0)
          {
             %i=$Pref::RPGDialog::MaxOptions+1;
          }
          else
          {
             %line=getSubStr(%line,%Start,strlen(%line));
             %End=strpos(%line,"</a>")+4;
             %line=getSubStr(%line,%End,strlen(%line));
             %answer=strReplace(%answer,"<a:RPGDialog "@%i@">","<a:RPGDialog "@%i@"> "@%i@" - ");
             %i++;
          }
       }

       
       RPGDialogAnswer.settext(%answer);
    }
    else
    {
       RPGDialogAnswer.settext("<a:RPGDialogNoAnswer>Continue...");
    }
    RPGDialogAnswer.Visible=true;

    Canvas.pushDialog(RPGDialog);
}

function RPGDialogAnswer::onURL(%this, %url)
{
	//same as RPGDialogQuestion::onURL, so just forward the call
	RPGDialogQuestion::onURL(%this, %url);
}

function RPGDialogQuestion::onURL(%this, %url)
{
   if(firstword(%url)!$="RPGDialog" && firstword(%url)!$="RPGDialogLink" && firstword(%url)!$="RPGDialogNoAnswer")
   {
      gotoWebPage( %url );
   }
   else if(firstword(%url)$="RPGDialogLink")
   {
      %Answers=%this.gettext();
      %AnswerHeaderSize=strlen("<a:RPGDialogLink "@restwords(%url)@">");
      %AnswerStart=strpos(%Answers,"<a:RPGDialogLink "@restwords(%url)@">")+%AnswerHeaderSize;
      %Answers=getSubStr(%Answers,%AnswerStart,strLen(%Answers));
      %AnswerEnd=strPos(%Answers,"</a>")+4;

      ChatHud.addLine($Pref::RPGDialog::ChatHudAnswerColor@"You: "@StripMLControlChars(getSubStr(%Answers,0,%AnswerEnd)));

      CommandToServer('RPGDialogAnswer', $RPGDialog::Sender, $RPGDialog::questionNumber, "QL"@restwords(%url));

      Canvas.popDialog(RPGDialog);
      RPGDialogQuestion.settext("");
      RPGDialogAnswer.settext("");
   }
   else if(firstword(%url)$="RPGDialogNoAnswer")
   {
      Canvas.popDialog(RPGDialog);
      RPGDialogQuestion.settext("");
      RPGDialogAnswer.settext("");
   }
   else
   {
      %Answers=%this.gettext();
      %Answers=strReplace(%Answers,restwords(%url)@" - ","");
      %AnswerHeaderSize=strlen("<a:RPGDialog "@restwords(%url)@">");
      %AnswerStart=strpos(%Answers,"<a:RPGDialog "@restwords(%url)@">")+%AnswerHeaderSize;
      %Answers=getSubStr(%Answers,%AnswerStart,strLen(%Answers));
      %AnswerEnd=strpos(%Answers,"</a>")+4;

      ChatHud.addLine($Pref::RPGDialog::ChatHudAnswerColor@"You: "@StripMLControlChars(getSubStr(%Answers,0,%AnswerEnd)));
 
      CommandToServer('RPGDialogAnswer', $RPGDialog::Sender, $RPGDialog::questionNumber, restwords(%url));

      Canvas.popDialog(RPGDialog);
      RPGDialogQuestion.settext("");
      RPGDialogAnswer.settext("");
   }
}

function setRPGDialogImage(%message, %portrait)
{
	// Search for wav tag marker.
	%imageStart = strstr(%message, "~Image:");
	if (%imageStart != -1) %portrait = getSubStr(%message, %imageStart + 7, strLen(%message));
	
	if(%portrait!$="" && isFile($Pref::RPGDialog::Client::PortraitsPath@%portrait))
	{
		RPGDialogPortrait.setbitmap($Pref::RPGDialog::Client::PortraitsPath@%portrait);
	}
	else
	{
		RPGDialogPortrait.setbitmap(expandFileName($Pref::RPGDialog::Client::PortraitsPath@"default.jpg"));
	}
	
	return %imageStart;
}

function playRPGDialogSound(%message)
{
   // Search for wav tag marker.
   %soundStart = strstr(%message, "~Sound:");
   if (%soundStart == -1) return -1;

   if(alxIsPlaying($RPGDialogSoundHandle)) alxStop($RPGDialogSoundHandle);

   %profile = getSubStr(%message, %soundStart + 7, strLen(%message));
   $RPGDialogSoundHandle = alxPlay(%profile);

   return %soundStart;
}

function SelectAnswer(%Number)
{
   if(strPos(RPGDialogAnswer.getText(),"<a:RPGDialog "@%Number@">")>=0)
      RPGDialogAnswer.onURL("RPGDialog "@%Number);
   else
      OutOfRPGDialogFunction(%Number);
}