var RawDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div><%= name %></div><div id="<%= dataId %>"></div></li>',
    
    setData : function(data) {
        if (data) {
            $("#" + this.dataId).html(data);
        }
    },
});