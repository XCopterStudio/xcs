var abstractDataViewFreeId = -1;

var AbstractDataView = Backbone.View.extend({

    id: 'data-from-onboard',
    
    initialize: function(dataId, attrs) {
        // initialize attributes, which can be overriden
        if(!this.template) {
            this.template = '';
        }
        if(!this.sizeX) { 
            this.sizeX = 1;
        }
        if(!this.sizeY) {
            this.sizeY = 1;
        }
        if(!this.dataId) {
            this.dataId = '';
        }
        if(!this.settings) {
            this.settings = [];
        }
        
        //bind functions
        _.bindAll(this, 'onSettings', 'validateSettings', 'setSettings', 'resetDefault');
        
        // initilize attributes, which should not be overriden
        this.attrs = {
            dataId: '',
        };
        this.widgetId = -1;
        this.widgetTypeId = "";
        this.widgetTypeName = "";
        
        this.model = app.Onboard;
        this.listenTo(app.Onboard, "change:data", this.onDataChange);
        
        //set unique widget id 
        this.widgetId = ++abstractDataViewFreeId;
        
        // set data id
        this.dataId = typeof dataId !== 'undefined' ? dataId : '';
        
        // set attributes
        this.attrs.dataId = this.dataId;  
        this.attrs.widgetId = this.widgetId;
        if(attrs) {
            for(var attr in attrs) {
                if (attrs.hasOwnProperty(attr)) {
                    this.attrs[attr] = attrs[attr];
                }
            }
        }
        
        if(this.template != '' && this.sizeX > 0 && this.sizeY > 0) {
            var gridster = $(".gridster > ul").gridster().data('gridster');
            var compiledTemplate = _.template(this.template);
            var hidden = "";
            if(this.settings.length == 0) {
                hidden = " hidden";
            }
            gridster.add_widget('\
                <div id="widget' + this.widgetId + '" class="widget">\
                    <button type="button" class="settings' + hidden + '">\
                        <i class="icon-cogs"></i>\
                        <span class="sr-only">Settings</span>\
                    </button>' +
                    compiledTemplate(this.attrs) + 
                '</div>', 
                this.sizeX, this.sizeY);
            
            // set settings
            if(this.settings.length > 0) {
                $('#widget' + this.widgetId + ' .settings').click(this.onSettings);
            }
        }
        
        // custom implementation init
        this.init();
    },
    
    init: function() {
        // prepare for override
    },
    
    validateSettings: function(settings) {
        // prepare for override
    },
    
    setSettings: function(settings) {
        // prepare for override
    },
    
    resetDefault: function(settings) {
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
        
            var set = _.find(this.settings, function(r) { return r.name == setting.name; })
            if(set) {
                set.default = setting.value;
            }
        }
    },
    
    remove: function() {
        var gridster = $(".gridster > ul").gridster().data('gridster');
        gridster.remove_widget($("#widget" + this.widgetId));
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
    
    onSettings: function(event){
        app.SettingView.show(
            new Setting(this.settings, this.setSettings, this.validateSettings, this.resetDefault)
        );
    },
});