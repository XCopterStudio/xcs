
var FlashMessagesView = Backbone.View.extend({
   
    id: 'flash-messages',
    
    initialize: function () {
	this.listenTo(app.XcopterState, "change:flash", this.flash);
	console.log(this.$el);
    },
    
    render: function () {
	
    },
    
    flash: function (model) {
	console.log('>>> Flashing!!!');
	//var elem = this.$el.append('<div class="alert alert-info">' + model.get("flash") + '</div>');
	var elem = $('<div class="alert alert-info fade in">' + model.get("flash") + '</div>').appendTo("#flash-messages");
	console.log(elem);
	window.setTimeout(function(){ elem.alert('close');}, 5000);
    }
});