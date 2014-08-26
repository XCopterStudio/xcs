var SettingType = ENUM("TEXT", "NUMBER", "BOOLEAN");

var Setting = Backbone.Model.extend({
    defaults: {
        modul: "",
        settings: [],                   // array of objects { name: string, type: SettingType, [default: value] },
        setSettings: undefined,         // one param function: array of objects { name: string, value: value },
        validateSettings: undefined,    // optional one param function: array of objects { name: string, value: value },
        resetDefaultSettings: undefined,// optional one param function: array of objects { name: string, value: value },
    },

    initialize : function(modul, settings, setSettings, validateSettings, resetDefaultSettings) {
        this.set("modul", modul);
        this.set("settings", settings);
        this.set("validateSettings", validateSettings);
        this.set("setSettings", setSettings);
        this.set("resetDefaultSettings", resetDefaultSettings);
    },
    
    load: function() {
        var settings = this.get("settings");
        var validateSettings = this.get("validateSettings");
        var setSettings = this.get("setSettings");
        var resetDefaultSettings = this.get("resetDefaultSettings");
        var modul = this.get("modul");
        
        // load settings from cookies
        var loadedSettings = [];
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            
            var cookieValue = $.cookie(modul + "__" + setting.name);
            
            if(cookieValue) {
                loadedSettings.push({
                    name: setting.name,
                    value: cookieValue,
                });
            }
        }
        
        if(loadedSettings.length > 0) {
            // validate loaded settings
            var validation;
            if(validateSettings) {
                validation = validateSettings(loadedSettings);
            }
            
            // set just valid settings
            if(!validation || validation.length == 0) {
                setSettings(loadedSettings);
                
                if(resetDefaultSettings) {
                    resetDefaultSettings(loadedSettings);
                }
            }
        }
    },
    
    save: function(settings) {
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];

            // save and set 4 weeks expiration
            $.cookie(this.get("modul") + "__" + setting.name, setting.value, {expires: 365});
        }
    },
});