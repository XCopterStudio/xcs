var RawDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div class="text-center"><%= name %></div><br><div id="raw_<%= widgetId %>_<%= dataId %>"></div></li>',
    
    init: function() {
        this.$raw = $("#raw_" + this.attrs.widgetId + "_" + this.attrs.dataId);
    },
    
    setData : function(data) {
        if (data && typeof data == 'object') {
            var parts = [];
            for(var key in data) {
                if(key === '$sn') { // TODO must be ingored, process data differently?
                    continue;
                }
                parts.push(key + ': ' + data[key]);                
            }
            this.$raw.html(parts.join('<br>'));
        } else if(data) {
            this.$raw.html(data);
        }
    },
});