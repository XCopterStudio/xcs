var ConsoleView = Backbone.View.extend({
    el: 'body',
    events: {
        "click #consoleStart": "onClickStart",
        "click #consoleStop": "onClickStop",
        "click #consoleClear": "onClickClear"
    },
    initialize: function() {
        this.model = new ConsoleModel();

        this.listenTo(this.model, 'change:state', this.onChangeState);
        this.listenTo(this.model, 'change:error', this.onChangeError);
        this.listenTo(this.model, 'change:output', this.onChangeOutput);
        

        this.$buttonStart = this.$('#consoleStart');
        this.$buttonStop = this.$('#consoleStop');
        this.$state = this.$('#consoleState');
        this.$codeElement = this.$('#consoleInput');
        this.$outputElement = this.$('#consoleOutput');

        this.editor = CodeMirror.fromTextArea(this.$codeElement.get(0), {
            //theme: "night",
            mode: "xcs",
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
        
        this.onChangeState(this.model);
    },
    onClickStart: function() {
        switch (this.model.get('state')) {
            case ConsoleModel.State.IDLE:
                var code = this.editor.doc.getValue();
                this.model.set('urbiscript', code);
                this.model.start();
                break;
            case ConsoleModel.State.FREEZED:
                this.model.resume();
                break;
            case ConsoleModel.State.RUNNING:
                this.model.pause();
                break;
        }
    },
    onClickStop: function() {
        switch (this.model.get('state')) {
            case ConsoleModel.State.RUNNING:
            case ConsoleModel.State.FREEZED:
                this.model.stop();
                break;
        }
    },
    onClickClear: function() {
        this.$outputElement.html('');
        this.$outputElement.removeClass('alert-danger');
    },
    onChangeState: function(model) {
        var START = 'Start';
        var RESUME = 'Resume';
        var PAUSE = 'Pause';

        switch (model.get('state')) {
            case ConsoleModel.State.IDLE:
                this.$buttonStart.html(START);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', true);
                this.editor.setOption('readOnly', false);
                break;
            case ConsoleModel.State.WAITING:
                this.$buttonStart.prop('disabled', true);
                this.$buttonStop.prop('disabled', true);
                this.editor.setOption('readOnly', true);
                break;
            case ConsoleModel.State.RUNNING:
                this.$buttonStart.html(PAUSE);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', false);
                this.editor.setOption('readOnly', true);
                break;
            case ConsoleModel.State.FREEZED:
                this.$buttonStart.html(RESUME);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', false);
                this.editor.setOption('readOnly', true);
                break;
        }
        this.$state.html(model.get('state'));
    },
    onChangeError: function(model) {
        var error = model.get('error');
        if (!error) {
            this.$outputElement.removeClass('alert-danger');
        } else {
            this.$outputElement.addClass('alert-danger');
            this.printOutput_(error);
        }
    },
    onChangeOutput: function(model) {
        var output = model.get('output');
        this.printOutput_(output);
    },    
    printOutput_: function(string) {
        this.$outputElement.append(document.createTextNode(string));
        this.$outputElement.get(0).scrollTop = this.$outputElement.get(0).scrollHeight;
    }

});