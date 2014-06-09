model = new ManualControl();

var MobileControllerView = Backbone.View.extend({
    el: 'window',

    events: {
        "deviceorientation": "handleOrientation"
    },

    initialize: function () {
        
        window.addEventListener('deviceorientation', this.handleOrientation);
    },

    render: function () {

    },

    handleOrientation: function (event) {
        var x = event.beta / 180.0;  // In degree in the range [-180,180]
        var y = event.gamma / 90.0; // In degree in the range [-90,90]
        var z = 0;
        //var z = event.alpha; // In degree in the range [0,360]

        //this.model.setRoll(x,this.model);
        model.setFly(x, y, z, 0,this.model);
    }
});