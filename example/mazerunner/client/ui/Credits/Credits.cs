echo("\c3--------- Loading Credits   ---------");
//--------------------------------------------------------------------------
// Credits.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function CreditsMLText::onWake( %this ) {
    %this.reload();
}

//
// 1. Clear all content.
// 2. Open the file MLCreditsTextContent.txt (abort if not found)
// 3. Read the file and push the contents into this GuiMLTextCtrl
//
function CreditsMLText::reload( %this ) {
    %this.setValue(""); // Clear it

    %file = new FileObject();

    %fileName = expandFileName( "./MLCreditsTextContent.txt" );

    %fileIsOpen = %file.openForRead( %fileName );

    if( %fileIsOpen ) {
        while(!%file.isEOF()) {

            %currentLine = %file.readLine();

            %this.addText( %currentLine, true );

        }
    }

    %this.forceReflow();

    %file.close();
    %file.delete();
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./Credits.gui");



