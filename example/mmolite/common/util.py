import sys
from ConfigParser import SafeConfigParser
from optparse import OptionParser

def getOptions(cfgFile, section, args):
    cfgParser = SafeConfigParser()
    cfgParser.read(cfgFile)
    argParser = OptionParser()

    masterSection=section
    masterOptions = cfgParser.options(masterSection);
    for dest in masterOptions:
        line=cfgParser.get(masterSection,dest)
        line=line.strip()
        if not line: continue
        short, long, help, meta, type, default = line.split(";")
        if default=="True":
            argParser.add_option(short, long, dest=dest, action="store_false", help=help)
        elif default=="False":
            argParser.add_option(short, long, dest=dest, action="store_true", help=help)
        else:
            argParser.add_option(short, long, dest=dest, help=help, default=default, type=type, metavar=meta)

    options, additionals = argParser.parse_args(args)
    
    # '+' args are for TGE
    for i in range(len(additionals)):
        additionals[i] = additionals[i].replace('+', '-')
    
    return options, additionals
