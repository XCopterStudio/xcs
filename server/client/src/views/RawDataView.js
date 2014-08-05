var RawDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div><%= name %></div><div id="<%= dataId %>"></div></li>',
    
    setData : function(data) {
        if (data && typeof data == 'object') {
            var parts = [];
            for(var key in data) {
                if(key === '$sn') { // TODO must be ingored, process data differently?
                    continue;
                }
                parts.push(key + ': ' + data[key]);                
            }
            $("#" + this.dataId).html(parts.join(', '));
        } else if(data) {
            $("#" + this.dataId).html(data);
        }
    },
});