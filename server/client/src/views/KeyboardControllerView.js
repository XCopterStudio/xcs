var KeyboardControllerView = Backbone.View.extend({
    //el: 'body', // is this correct?
    events: {
    },
    initialize: function(model, name) {
        this.model = model;
        this.name = name;

        this.keyMap = {
            65: {control: 'roll', value: -0.2}, // A
            68: {control: 'roll', value: 0.2}, // D
            83: {control: 'pitch', value: 0.2}, // S
            87: {control: 'pitch', value: -0.2}, // W
            37: {control: 'yaw', value: -1.0}, // left
            39: {control: 'yaw', value: 1.0}, // right
            38: {control: 'gaz', value: 0.2}, // up
            40: {control: 'gaz', value: -0.2} // down
        };
        this.down = {};

        // Workaround for event on body, something native Backbone events don't support when
        // view is on body's subelement (and nested in another view).
        $('body').on('keydown', this, this.keydown);
        $('body').on('keyup', this, this.keyup);
    },
    keydown: function(e) {
        var that = e.data;
        if (that.model.get('activeController') !== 'keyboard' || that.keyMap[e.which] === undefined) {
            return;
        }

        //disable scrolling etc. 
        e.preventDefault();

        // comment condition for repeated sending of fly params
        if (that.down[e.which]) {
            return;
        }
        that.down[e.which] = true;

        var control = that.keyMap[e.which]['control'];
        var value = that.keyMap[e.which]['value'];
        that.model.setControl(that.name, control, value);
    },
    keyup: function(e) {
        var that = e.data;
        if (that.keyMap[e.which] === undefined) {
            return;
        }
        that.down[e.which] = false;

        var control = that.keyMap[e.which]['control'];
        that.model.setControl(that.name, control, 0);
    }

});
