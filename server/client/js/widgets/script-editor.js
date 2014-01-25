var myCodeMirror = new CodeMirror(document.getElementById('script-editor-area'),
                              {
                                theme:  "night",
                                mode:   "xcs",
                                lineNumbers: true,
                                matchBrackets: true,
                                indentUnit: 2,
                                tabSize: 2,
                                smartIndent: true,  
                                electricChars: true,
                                lineWrapping: false,
                                showCursorWhenSelecting: false,
                                undoDepth: 1000,
                                autoFocus: false,
                                //onKeyEvent: function (e, s) { if (s.type == "keyup") { CodeMirror.showHint(e); console.log("test"); } }
                              });

