var AbstractDataView = Backbone.View.extend({

    id: 'data-from-onboard',
    
    template: '',
    
    sizeX: 1,
    
    sizeY: 1,
    
    dataId: '',
    
    attrs: {
        dataId: '',
    },
    
    initialize: function(dataId, attrs) {
        this.model = app.Onboard;
        this.listenTo(app.Onboard, "change:data", this.onDataChange);
        
        // set data id
        this.dataId = typeof dataId !== 'undefined' ? dataId : '';
        
        // set attributes
        this.attrs.dataId = this.dataId;        
        if(attrs) {
            for(var attr in attrs) {
                if (attrs.hasOwnProperty(attr)) {
                    this.attrs[attr] = attrs[attr];
                }
            }
        }
        
        if(this.markup != '' && this.sizeX > 0 && this.sizeY > 0) {
            var gridster = $(".gridster > ul").gridster().data('gridster');
            var compiledTemplate = _.template(this.template);
            gridster.add_widget(compiledTemplate(this.attrs), this.sizeX, this.siezY);
        }
    },

    onDataChange: function(model) {
        var data = model.get("data");

        if(this.dataId == '') {
            this.setData(data);
        }
        else if(data[this.dataId]) {
            this.setData(data[this.dataId]);
        }
    },
});