
var AbstractNavdataView = Backbone.View.extend({
    
    id: 'data-from-onboard',
    
    initialize: function () {
	this.model = app.XcopterState;
	
	this.listenTo(app.XcopterState, "change:data", this.onDataChange);
	
	// TODO GRIDSTER
	// initialize gridster
	    var gridster = $(".gridster > ul").gridster({
	        widget_margins: [10, 10],
	        widget_base_dimensions: [270, 150]
	    }).data('gridster');
    },
    
    onDataChange: function (model) {
	var burst = model.get("data");
	console.log(burst);
	if (burst.altitude) {
	    $("#altitude").html(burst.altitude);
	}
	if (burst.phi) {
	    $("#phi").html(burst.phi);
	}
	if (burst.psi) {
	    $("#psi").html(burst.psi);
	}
	if (burst.theta) {
	    $("#theta").html(burst.theta);
	}
    }
	
});