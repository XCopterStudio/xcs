var DataView = Backbone.View.extend({

    id: 'data-from-onboard',
    
    views: {
        raw: {
            name: "raw data",
            ctor: function(dataId) {
                //var dataName = dataId.charAt(0).toUpperCase() + dataId.slice(1);;
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    
                    return new RawDataView(dataId, { name: nodeName + " " + portName });
                 }
            }
        }
    },
    
    createdViews: {},
    
    initialize: function() {
        //set gridster param
        $(".gridster > ul").gridster({
            widget_margins : [ 10, 10 ],
            widget_base_dimensions : [ 270, 150 ]
        });
    },
      
    getViewNames: function() {
        var views = [];
        
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor) {
                views.push(this.views[view].name);
            }
        }
        
        return views;
    },
    
    addViewByName: function(viewName, dataId, attrs) {
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && this.views[view].name == viewName) {
                var createdView = this.views[view].ctor(dataId, attrs);
                if(createdView) {
                    this.createdViews[createdView.widgetId] = createdView;
                    return createdView.widgetId;
                }
            }
        }
        
        return -1;
    },
    
    addView: function(viewId, dataId, attrs) {
        console.log("ADD VIEW: " + viewId);
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && view == viewId) {
                var createdView = this.views[view].ctor(dataId, attrs);
                if(createdView) {
                    this.createdViews[createdView.widgetId] = createdView;
                    return createdView.widgetId;
                }
            }
        }
        
        return -1;
    },
    
    removeView: function(widgetId) {
        if(this.createdViews[widgetId]) {
            this.createdViews[widgetId].remove();
            delete this.createdViews[widgetId];
        }
    },
});