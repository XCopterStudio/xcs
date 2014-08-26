var ConsoleView = Backbone.View.extend({
    el: 'body',
    events: {
        "click #console-start": "onClickStart",
        "click #console-stop": "onClickStop",
        "click #console-clear": "onClickClear",
        "click #console-script-delete": "onClickScriptDelete",
        "keyup #console-script-save-as-name": "onChangeScriptSaveAsName",
    },
    initialize: function() {
        /* GUI objects */
        this.$buttonStart = this.$('#console-start');
        this.$buttonStop = this.$('#console-stop');
        this.$state = this.$('#console-state');
        this.$codeElement = this.$('#console-input');
        this.$outputElement = this.$('#console-output');

        this.$scriptModified = this.$('#console-script-modified');
        this.$scriptSave = this.$('#console-script-save');
        this.$scriptSaveAs = this.$('#console-script-save-as');
        this.$scriptSaveAsName = this.$('#console-script-save-as-name');

        this.editor = CodeMirror.fromTextArea(this.$codeElement.get(0), {
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
        });


        /* Event handlers */
        var model = this.model = new ConsoleModel();

        this.listenTo(model, 'change:state', this.onChangeState);
        this.listenTo(model, 'change:error', this.onChangeError);
        this.listenTo(model, 'change:output', this.onChangeOutput);
        this.listenTo(model, 'change:scriptName', this.onChangeScriptName);
        this.listenTo(model, 'change:scriptModified', this.onChangeScriptModified);
        this.listenTo(model, 'change:allScripts', this.updateScriptsList);

        var that = this;
        this.editor.doc.on("change", function() {
            model.set('scriptModified', true);
        });

        /* Actions */
        var saveAsAction = new WaitAction("#console-script-save-as", WaitActionType.Click);
        saveAsAction.set("action", function() {
            saveAsAction.start();
            model.saveScript(that.editor.doc.getValue(), that.$scriptSaveAsName.val(), function() {
                that.$scriptSaveAsName.val('');
                that.$scriptSaveAsName.trigger('change');
                saveAsAction.stop();
            });

        });
        app.Wait.setWaitAction(saveAsAction);

        var saveAction = new WaitAction("#console-script-save", WaitActionType.Click);
        saveAction.set("action", function() {
            if (that.$scriptSave.parent().hasClass('disabled')) {
                return;
            }
            saveAction.start();
            model.saveScript(that.editor.doc.getValue(), model.get('scriptName'), function() {
                saveAction.stop();
            });
        });
        app.Wait.setWaitAction(saveAction);

        CodeMirror.commands.save = function(editor) {
            saveAction.get('action')({});
        };


        var manageAction = new WaitAction("#console-script-manage", WaitActionType.Click);
        manageAction.set("action", function() {
            manageAction.start();
            model.loadScripts(function() {
                manageAction.stop();
                app.ModalView.showModal("#modal-console-scripts");
            });
        });
        app.Wait.setWaitAction(manageAction);



        /* Initialization */
        this.onChangeState(model);
    },
    /* Execution button handlers */
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
    /* Storage button handlers */
    onClickScriptDelete: function() {
        // Confirm?
        this.editor.doc.setValue('');
    },
    onChangeScriptSaveAsName: function() {
        var scriptName = this.$scriptSaveAsName.val();
        this.$scriptSaveAs.prop('disabled', !scriptName);
    },
    /* Model handlers */
    onChangeState: function(model) {
        var START = 'Start';
        var RESUME = 'Resume';
        var PAUSE = 'Pause';

        switch (model.get('state')) {
            case ConsoleModel.State.INACTIVE:
                this.$buttonStart.html(START);
                this.$buttonStart.prop('disabled', true);
                this.$buttonStop.prop('disabled', true);
                this.editor.setOption('readOnly', false);
                break;
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
    onChangeScriptName: function(model) {
        var scriptName = model.get('scriptName');
        this.$scriptSave.find('.filename').text(scriptName);
    },
    onChangeScriptModified: function(model) {
        var modified = model.get('scriptModified');
        this.$scriptSave.parent().toggleClass('disabled', !(modified && model.get('scriptName')));
        this.$scriptModified.text(modified ? '*' : '');
    },
    /* Utility functions */
    printOutput_: function(string) {
        this.$outputElement.append(document.createTextNode(string));
        this.$outputElement.get(0).scrollTop = this.$outputElement.get(0).scrollHeight;
    },
    updateScriptsList: function() {
        var scripts = this.model.get('allScripts');
        var modalItems = $('#modal-console-scripts .modal-body table');
        if (scripts.length === 0) {
            modalItems.html('<thead><tr><th>No saved scripts.</th></tr></thead>');
            return;
        }

        modalItems.html('<thead><tr><th>Script name</th><th>Actions</th></tr></thead><tbody></tbody>');
        modalItems = modalItems.find("tbody");
        var that = this;
        scripts.sort(function(a, b) {
            return a.name.localeCompare(b.name);
        });
        for (var i in scripts) {
            var scriptName = scripts[i].name;
            var idLoad = "console-load-" + i;
            var idRemove = "console-remove-" + i;
            modalItems.append(
                    '<tr>\
                    <td>' + scriptName + '</td>\
                    <td>\
                        <button type="button" class="btn btn-link" href="#" id="' + idLoad + '">Load</button>\
                        <button type="button" class="btn btn-link" href="#" id="' + idRemove + '">Remove</button>\
                    </td>\
                </tr>');

            // set load action
            var loadAction = new WaitAction("#" + idLoad, WaitActionType.Click);
            loadAction.set("action", (function() {
                var localScriptName = scriptName;
                var localAction = loadAction;
                return function() {
                    localAction.start();
                    var script = that.model.loadScript(localScriptName);
                    that.editor.doc.setValue(script);
                    that.model.set('scriptModified', false); // NOTE: override unintentional onChange event
                    localAction.stop();
                    app.ModalView.showModal(); // TODO hide modal
                };
            })());
            app.Wait.setWaitAction(loadAction);

            // set remove action
            var removeAction = new WaitAction("#" + idRemove, WaitActionType.Click);
            removeAction.set("action", (function() {
                var localScriptName = scriptName;
                var localAction = removeAction;
                return function() {
                    localAction.start();
                    that.model.deleteScript(localScriptName, function() {
                        localAction.stop();
                    });

                };
            })());
            app.Wait.setWaitAction(removeAction);
        }
    }

});