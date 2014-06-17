xjson
{
    name "XJson"
    init ""
}
xxci
{
    name "XXci"
    init
    {
        "xci dodo" "\"xci_dodo\""
        "xci parrot" "\"xci_parrot\""
    }
}
xdatalogger
{
    name XDatalogger
    init "\"logs/test.txt\""   ;TODO: change file name - or file name like xinputport
    ;init "\"logs/\" + Date.now().asString().replace(\" \", \"_\") + \".txt\""
}
xdataplayer
{
    name XDataplayer
    init "\"logs/test.txt\""   ;TODO: change file name - or file name like xinputport
}
xonboard
{
    name "XOnboard"
    init ""
}
xsettings
{
    name "XSettings"
    init "\"./xobjects.xs\""    ;TODO: change file name
}