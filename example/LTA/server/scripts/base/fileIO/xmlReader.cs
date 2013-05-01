
function loadXMLtoString(%filename)
{
	%sgsysresults = "";
	%file = new FileObject();	
	%file.openforRead(%filename);	 
	while( !%file.isEOF() )	 
	{		
		%line = %file.readline();		 
		%sgsysresults = %sgsysresults @ %line;	 
	}	
	%file.close();	
	%file.delete();
	return %sgsysresults;
}

function getXMLDataCount(%string, %tag, %startChar)
{	
	%startTag = "<" @ %tag @ ">";	
	%endTag   = "</" @ %tag @ ">";	
	%startTagOffset = strpos(%string, %startTag, %startChar);	
	%startOffset = %startTagOffset + strlen(%startTag);	
	%endTagOffset = strpos(%string, %endTag, %startOffset - 1);	
	if(%endTagOffset < 0 || %endTagOffset < %startOffset)    		return "";	
	%lastOffset = %endTagOffset;
	%result = getSubStr(%string, %startOffset, %endTagOffset - %startOffset);	
	%result = strreplace(%result, "\"", "\"");	
	%result = strreplace(%result, "&",  "&");	
	return %lastOffset SPC %result;
}

function getxmldata(%string, %tag, %startChar)
{	
	%startTag = "<" @ %tag @ ">";	
	%endTag   = "</" @ %tag @ ">";	
	%startTagOffset = strpos(%string, %startTag, %startChar);	
	%startOffset = %startTagOffset + strlen(%startTag);	
	%endTagOffset = strpos(%string, %endTag, %startOffset - 1);	
	if(%endTagOffset < 0 || %endTagOffset < %startOffset)    		return "";	
	%result = getSubStr(%string, %startOffset, %endTagOffset - %startOffset);	
	%result = strreplace(%result, "\"", "\"");	
	%result = strreplace(%result, "&",  "&");	
	return %result;
}
