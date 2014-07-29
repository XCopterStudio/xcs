xdatalogger
{
    name "XDatalogger"
    init 
    {
        "XDatalogger" "\"experiments/sample3-out.txt\""   ;TODO: change file name - or file name like xinputport
        ;"XDatalogger" "\"logs/\" + Date.now().asString().replace(\" \", \"_\") + \".txt\""
    }
}
xdataplayer
{
    name "XDataplayer"
    init 
    {
        "XDataplayer" "\"experiments/sample3.txt\""   ;TODO: change file name - or file name like xinputport
    }
}
xonboard
{
    name "XOnboard"
    init 
    {
        "XOnboard" ""
    }
}
xxci
{
    name "XXci"
    init
    {
        "Xci Dodo" "\"xci_dodo\""
        "Xci Parrot" "\"xci_parrot\""
        "Xci Vrep" "\"xci_vrep\""
    }
}
xcontrol
{
    name "XControl"
    init 
    {
        "XControl" ""
    }
}
xhermit_movement
{
    name "XHermitMovement"
    init 
    {
        "XHermitMovement" ""
    }
}
xlocalization
{
    name "XLocalization"
    init 
    {
        "XLocalization" "\"xsettings/xlocalization.xs\""
    }
}
