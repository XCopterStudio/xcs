var FlyControlView = Backbone.View.extend({
    el: '#fc-panel',
    events: {
        "click #fc-panel-take-off": "onClickTakeOff",
        "click #fc-panel-land": "onClickLand",
        "click button[data-fc-controller]": "onClickType",
    },
    controllerClasses: {
        'off': function() {
        },
        'keyboard': KeyboardControllerView,
        'mobile': MobileControllerView // NOTE: only add button and implementation to enable it
    },
    initialize: function() {
        this.model = new FlyControlModel();

        this.$buttons = $('button[data-fc-controller]');
        this.controllers = {};
        var that = this;
        this.$buttons.each(function() {
            var $el = $(this);
            var controller = $el.attr('data-fc-controller');
            if (!(controller in that.controllerClasses)) {
                return;
            }
            that.controllers[controller] = new that.controllerClasses[controller](that.model, controller);
        });

        this.listenTo(this.model, 'change:activeController', this.onChangeActiveController);

        this.onChangeActiveController(this.model);
    },
    onClickTakeOff: function() {
        this.model.takeOff();
    },
    onClickLand: function() {
        this.model.land();
    },
    onClickType: function(e) {
        var button = $(e.target);
        var controller = button.attr('data-fc-controller');

        this.model.set('activeController', controller);
    },
    onChangeActiveController: function(model) {
        this.$buttons.each(function() {
            var $el = $(this);
            var controller = $el.attr('data-fc-controller');
            if (controller === model.get('activeController')) {
                $el.addClass('btn-info');
                $el.removeClass('btn-default');
            } else {
                $el.addClass('btn-default');
                $el.removeClass('btn-info');
            }
        });
    }

});
