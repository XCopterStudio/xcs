var abstractDataViewFreeId = -1;

var SettingType = ENUM("TEXT", "NUMBER", "BOOLEAN");

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
        this.onSettings = _.bind(this.onSettings, this);
        
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
        // init jquery objects
        var $settingsModal = $('#modal-widget-settings .modal-body form');
        var $btnOk = $('#modal-widget-settings .btn-ok');
        var $btnCancel = $('#modal-widget-settings .btn-cancel');
        
        // create settings
        for(var i = 0; i < this.settings.length; ++i) {
            var setting = this.settings[i];
            var input = "";
            switch(setting.type) {
                case SettingType.TEXT:
                    input = '<input type="text" class="form-control" placeholder="Text" id="widget-settings-input' + i + '">';
                    break;
                case SettingType.NUMBER:
                    input = '<input type="number" class="form-control" placeholder="Number" id="widget-settings-input' + i + '">';
                    break;
                case SettingType.BOOLEAN:
                    input = '<div class="checkbox"><label><input type="checkbox" id="widget-settings-input' + i + '"></label></div>';
                    break;
            }
            
            $settingsModal.append(
                '<div class="form-group group' + i + '">\
                    <label for="widget-settings-input' + i + '" class="col-sm-3 control-label">' + setting.name + '</label>\
                    <div class="col-sm-9">' +
                        input +
                        '<span class="help-block"></span>\
                    </div>\
                </div>');
        }
        
        // show modal window
        app.ModalView.showModal('#modal-widget-settings', { show: true, backdrop: false, keyboard: false });
        
        //set default
        for(var i = 0; i < this.settings.length; ++i) {
            var setting = this.settings[i];
            if(setting.default !== undefined) {
                if(setting.type == SettingType.BOOLEAN) {
                    $('#widget-settings-input' + i).prop('checked', setting.default) 
                }
                else {
                    $('#widget-settings-input' + i).val(setting.default);
                }
            }
            else {console.log("no default value");}
        }
        
        var self = this;
        
        // set ok action
        $btnOk.on('click', function(e) {
            //get settings
            var settings = [];
            for(var i = 0; i < self.settings.length; ++i) {
                var setting = self.settings[i];
                
                settings.push({ 
                    name: setting.name,
                    value: (setting.type == SettingType.BOOLEAN 
                            ? $('#widget-settings-input' + i).is(':checked') 
                            : $('#widget-settings-input' + i).val()),
                });
            }
            
            var validation = self.validateSettings(settings);
            
            if(!validation || validation.length == 0) {
                self.setSettings(settings);
                self.resetDefault(settings);
                $btnCancel.trigger('click');
            }
            else {
                console.log("settings validation error: " + JSON.stringify(validation));
                
                for(var i = 0; i < self.settings.length; ++i) {
                    var setting = self.settings[i];
                    
                    var error = _.find(validation, function(r) { return r.name == setting.name; })
                    
                    var $settingGroup = $settingsModal.find('.group' + i);
                    $settingGroup.removeClass("has-success");
                    $settingGroup.removeClass("has-error");
                    
                    var $msg = $settingGroup.find('.help-block');
                    
                    if(error) {
                        $settingGroup.addClass("has-error");
                        $msg.text(error.msg);
                    }
                    else {
                        $settingGroup.addClass("has-success");
                        $msg.text("");
                    }
                }
            }
        })
        
        // set clean actions
        $('#modal-widget-settings').one('hidden.bs.modal', function (e) {                    
            $settingsModal.html('');
            $btnOk.off('click');
        });
    },
    
    validateNumber: function(value) {
        var result = "";
        
        if(isNaN(parseFloat(value)) || !isFinite(value)) {
            result += "It is not a valid number. "
        }
        
        return result;
    },
    
    validateEmpty: function(value) {
        var result = "";
        
        if(value == "") {
            result += "Value can not be empty. "
        }
        
        return result;
    },
});