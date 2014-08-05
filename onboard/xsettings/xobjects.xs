XDatalogger { ; this is configuration name
    ;prototype XDatalogger                  ; (default is configuration name)
    ;file xdatalogger.u                     ; (default lowecased name of the prototype)
    args "\"experiments/sample3-out.txt\""  ; these are init's arguments (default none)
}

XDataplayer {
    args "\"experiments/sample3.txt\""   ;TODO: change file name - or file name like xinputport
}

XOnboard { }    ; TODO remove

XciDodo {
    prototype XXci
    args "\"xci_dodo\""
}

XciParrot {
    prototype XXci
    args "\"xci_parrot\""
}

XciVrep {
    prototype XXci
    args "\"xci_vrep\""
}

XControl {
    args "xcsPaths.getSettings(\"xcontrol.xs\")"
}

XHermitMovement {
    file xhermit_movement
}

XLocalization {
    args "xcsPaths.getSettings(\"xlocalization.xs\")"
}

Executor {
    file nodes/executor.u     ; TODO to what is it relative
    args "this"               ; this is the current lobby
}
Gui {
    prototype SemanticSender
    file nodes/semantic_sender.u 
    args "adapter, XJson, 8"
    special true
}

SemanticReceiver {
    file nodes/semantic_receiver.u
    args "jsonReceiver, ["\
        "\"flyControl\" => Pair.new('xcs::FlyControl', \"FLY_CONTROL\"),"\
        "\"execUrbiscript\" => Pair.new(String, \"URBISCRIPT\"),"\
        "\"execControl\" => Pair.new(String, \"CONTROL\")"\
    "]"
}
