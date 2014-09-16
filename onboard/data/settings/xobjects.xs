; This first node configuration explains what are possible configuration options.
Datalogger { ; this is configuration name
    prototype XDatalogger                               ; (default is configuration name)
    ;file xdatalogger.u                                 ; (default lowecased name of the prototype)
    args "xcsPaths.getLog(\"default_log.txt\")"         ; these are init's arguments (default none)
    ; special true                                      ; special nodes are handled specially (default false)
    ; receiver true                                     ; node is able to receive data from GUI (default false), only last created node with this flag is receiving
}

Dataplayer {
    prototype XDataplayer
    args "xcsPaths.getLog(\"default_log.txt\")"
}

FlyControlAggregator {
    prototype XFlyControlAggregator
    file xflycontrol_aggregator
}

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

Control {
    prototype XControl
    args "xcsPaths.getSettings(\"xcontrol.xs\")"
}

CheckpointMovement {
    prototype XCheckpointMovement
    file xcheckpoint_movement
}

FlyControl {
    prototype XMultiplexerFlyControl
    file xmultiplexer
    args "2, true, \"FLY_CONTROL\""
}

PID {
    prototype XPID
}

LineFinder {
  prototype XLineFinder
  file xline_finder
}

LineKeeper {
  prototype XLineKeeper
  file xline_keeper
}

LineMaster {
  file nodes/line_master.u
}

FrontierAligner {
  file nodes/frontier_aligner.u
}

Localization {
    prototype XLocalization
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

RedDot {
    prototype XRedDot
    file xred_dot
}

SemanticReceiver {
    file nodes/semantic_receiver.u
    args "["\
        "\"flyControl\" => Pair.new('xcs::FlyControl', \"FLY_CONTROL\"),"\
        "\"command\" => Pair.new(String, \"COMMAND\"),"\
        "\"execUrbiscript\" => Pair.new(String, \"URBISCRIPT\"),"\
        "\"execControl\" => Pair.new(String, \"CONTROL\")"\
    "]"
    receiver true
}

FlyControlDual {
    prototype XMultiplexerFlyControl
    file xmultiplexer
    args "2 , false, \"FLY_CONTROL\""
}

FlyControlTriple {
    prototype XMultiplexerFlyControl
    file xmultiplexer
    args "3 , false, \"FLY_CONTROL\""
}