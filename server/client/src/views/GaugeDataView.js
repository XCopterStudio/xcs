var GaugeDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div id="gauge_<%= widgetId %>_<%= dataId %>" class="widget-gauge"></div></li>',
    
    init: function() {
        this.gauge = new JustGage({
            id: "gauge_" + this.attrs.widgetId + "_" + this.attrs.dataId, 
            value: 0, 
            min: this.attrs.min,
            max: this.attrs.max,
            title: this.attrs.name
        });
    },
    
    setData: function(data) {
        var intData = parseInt(data);
        if(!isNaN(intData)) {            
            this.gauge.refresh(intData);
        }
    },
});