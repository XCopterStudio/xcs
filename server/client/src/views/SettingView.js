var SettingView = Backbone.View.extend({

    id: '#modal-settings',
    
    show: function(settingModel){
        // init jquery objects
        var $settingsModal = $('#modal-settings .modal-body form');
        var $btnOk = $('#modal-settings .btn-ok');
        var $btnCancel = $('#modal-settings .btn-cancel');
        
        var settings = settingModel.get("settings");
        var validateSettings = settingModel.get("validateSettings");
        var setSettings = settingModel.get("setSettings");
        var resetDefaultSettings = settingModel.get("resetDefaultSettings");
        
        // create settings
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
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
        app.ModalView.showModal('#modal-settings', { show: true, backdrop: false, keyboard: false });
        
        //set default
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            
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
        
        // set ok action
        $btnOk.on('click', function(e) {
            //get settings
            var settedSettings = [];
            for(var i = 0; i < settings.length; ++i) {
                var setting = settings[i];
                
                settedSettings.push({
                    name: setting.name,
                    value: (setting.type == SettingType.BOOLEAN 
                            ? $('#widget-settings-input' + i).is(':checked') 
                            : $('#widget-settings-input' + i).val()),
                });
            }
            
            var validation;
            if(validateSettings) {
                validation = validateSettings(settedSettings);
            }
            
            if(!validation || validation.length == 0) {
                setSettings(settedSettings);
                
                if(resetDefaultSettings) {
                    resetDefaultSettings(settedSettings);
                }
                
                //save to cookies
                settingModel.save(settedSettings);
                
                // close modal
                $btnCancel.trigger('click');
            }
            else {
                for(var i = 0; i < settings.length; ++i) {
                    var setting = settings[i];
                    
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
        $('#modal-settings').one('hidden.bs.modal', function (e) {                    
            $settingsModal.html('');
            $btnOk.off('click');
        });
    },
    
    //validate number
    validateNumber: function(value) {
        var result = "";
        
        if(isNaN(parseFloat(value)) || !isFinite(value)) {
            result += "It is not a valid number. "
        }
        
        return result;
    },
    
    //validate empty strings
    validateEmpty: function(value) {
        var result = "";
        
        if(value == "") {
            result += "Value can not be empty. "
        }
        
        return result;
    },
});