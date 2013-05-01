
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// eval() calls don't expand filenames so we do it with
// this function by:
//   - first we isolate the first quoted substring
//   - then we run it thru expandFilename()
//   - finally, we return the reassembled line
function expand_embedded_filename(%line)
{
  %moreline = nextToken(%line, "before", "\"");
  %after = nextToken(%moreline, "middle", "\"");
  return %before @ "\"" @ expandFilename(%middle) @ "\"" @ %after;
}

//
// Constructs a TSShapeConstructor datablock out of a template file
// and any available autoloading sequence files. The template file
// should look like a normal TSShapeConstructor script. 
//
//   example template file:
//      datablock TSShapeConstructor(OrcMageDts)
//      {
//         baseShape = "./orc_mage.dts";
//         sequence0 = "./orc_mage_root.dsq root";
//           ...
//         sequence31 = "./orc_mage_summon.dsq summon";
//      };
//
// Instead of running exec() on this file directly, we'll parse it.
// insert some additional sequences, and then create it with a
// call to eval().
//
// Autoloading sequence files are ordinary text files where each
// line lists a dsq sequence filename followed by a sequence name.
// 
//   example autoloading sequence file:
//      // Orc Mage Sequences
//      ./player_diespin.dsq tripandfall
//        ...
//      ./SP1/om_cantripcasting1.dsq cantrip1
//
// This function will search for files that match %auto_seqs_file in
// any subfolders one level below the folder containinf this script.
//

function autoloadPlayerShape(%template_file, %auto_seqs_file)
{
  %n_seqs = 0;
  %template = "";

  // load template file into %template string but leave
  // off the final closing brace.
  %template_fp = new FileObject(); 
  if (%template_fp.openForRead(%template_file))
  {
    while (!%template_fp.isEOF()) 
    {
      %line = ltrim(%template_fp.readLine());
      if (getSubStr(%line, 0, 2) $= "};")
      {
        // this is where we want to insert more
        // sequences so we stop parsing.
        break;
      }
      else if (%line $= "" || getSubStr(%line, 0, 2) $= "//")
      {
        // skip BLANKS and COMMENTS
      }
      else
      {
        if (getSubStr(%line, 0, 8) $= "sequence")
        {
          %n_seqs++; // count the sequences
          %line = expand_embedded_filename(%line);
        }
        else if (strlwr(getSubStr(%line, 0, 9)) $= "baseshape")
        {
          %line = expand_embedded_filename(%line);
        }
        %template = %template @ "\n" @ %line;
      }
    }
    %template_fp.close();
    %template_fp.delete();
  }
  else
  {
    %template_fp.delete();
    // If we're here, we are probably using TGE 1.3 where
    // openForRead() always returns false. Assume player 
    // file has been manually edited and exec() directly.
    exec(%template_file);
    return;
  }

  // parse out additional sequences from autoloading subditectories
  // and add them to the template.

  %fp = new FileObject();
  %pattern = "./*/" @ %auto_seqs_file;
  for (%file = findFirstFile(%pattern); %file !$= ""; %file = findNextFile(%pattern)) 
  {
    echo("Autoloading AFX Sequences from" SPC %file);
    if (%fp.openForRead(%file))
    {
      while (!%fp.isEOF()) 
      {
        %line = ltrim(%fp.readLine());
        // skip BLANKS and COMMENTS
        if (%line !$= "" && getSubStr(%line, 0, 2) !$= "//")
        {
          %template = %template @ "\n" @ "sequence" @ %n_seqs @ " = \"" @ expandFilename(%line) @ "\";";
          %n_seqs++;
        }
      }
      %fp.close();
    }
  }
  %fp.delete();

  // append the closing brace
  %template = %template @ "\n" @ "};";

  // eval() the template
  eval(%template);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

autoloadPlayerShape("./orc_warrior.cs", "orc_warrior_seqs.txt");
autoloadPlayerShape("./orc_mage.cs", "orc_mage_seqs.txt");

