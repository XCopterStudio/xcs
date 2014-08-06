var ConsoleView = Backbone.View.extend({
    el: '#console',
    events: {
        "click #console-start": "onClickStart",
        "click #console-stop": "onClickStop"
    },
    initialize: function() {
        this.model = new ConsoleModel();

        this.listenTo(this.model, 'change:state', this.onChangeState);

        this.$buttonStart = this.$('#console-start');
        this.$buttonStop = this.$('#console-stop');
        this.$state = this.$('#console-state');
        this.$codeElement = this.$('textarea');

        this.onChangeState(this.model);
    },
    onClickStart: function() {
        switch (this.model.get('state')) {
            case ConsoleModel.State.IDLE:
                this.model.set('urbiscript', this.$codeElement.val());
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
    onChangeState: function(model) {
        var START = 'Start';
        var RESUME = 'Resume';
        var PAUSE = 'Pause';

        switch (model.get('state')) {
            case ConsoleModel.State.IDLE:
                this.$buttonStart.html(START);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', true);
                this.$codeElement.prop('disabled', false);
                break;
            case ConsoleModel.State.WAITING:
                this.$buttonStart.prop('disabled', true);
                this.$buttonStop.prop('disabled', true);
                this.$codeElement.prop('disabled', true);
                break;
            case ConsoleModel.State.RUNNING:
                this.$buttonStart.html(PAUSE);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', false);
                this.$codeElement.prop('disabled', true);
                break;
            case ConsoleModel.State.FREEZED:
                this.$buttonStart.html(RESUME);
                this.$buttonStart.prop('disabled', false);
                this.$buttonStop.prop('disabled', false);
                this.$codeElement.prop('disabled', true);
                break;
        }
        this.$state.html(model.get('state'));
    }

});