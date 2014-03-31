var AbstractNavdataView = Backbone.View.extend({

    id : 'data-from-onboard',

    initialize : function() {
        this.model = app.XcopterState;

        this.listenTo(app.XcopterState, "change:data", this.onDataChange);

        // TODO GRIDSTER
        // initialize gridster
        var gridster = $(".gridster > ul").gridster({
            widget_margins : [ 10, 10 ],
            widget_base_dimensions : [ 270, 150 ]
        }).data('gridster');

        gridster.add_widget('<li class="widget-line"><div>Battery</div><div id="battery"></div></li>', 1, 1);
        gridster.add_widget('<li class="widget-line"><div>Altitude</div><div id="altitude"></div></li>', 1, 1);
        gridster.add_widget('<li class="widget-line"><div>Phi</div><div id="phi"></div></li>', 1, 1);
        gridster.add_widget('<li class="widget-line"><div>Psi</div><div id="psi"></div></li>', 1, 1);
        gridster.add_widget('<li class="widget-line"><div>Theta</div><div id="theta"></div></li>', 1, 1);

    },

    onDataChange : function(model) {
        var burst = model.get("data");

        if (burst.battery != undefined) {
            $("#battery").html(burst.battery);
        }
        if (burst.altitude != undefined) {
            $("#altitude").html(burst.altitude);
        }
        if (burst.phi != undefined) {
            $("#phi").html(burst.phi);
        }
        if (burst.psi != undefined) {
            $("#psi").html(burst.psi);
        }
        if (burst.theta != undefined) {
            $("#theta").html(burst.theta);
        }
    }

});
