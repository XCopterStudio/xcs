var SettingType = ENUM("TEXT", "NUMBER", "BOOLEAN");

var Setting = Backbone.Model.extend({
    defaults: {
        settings: [],                   // array of objects { name: string, type: SettingType, [default: value] },
        setSettings: undefined,         // one param function: array of objects { name: string, value: value },
        validateSettings: undefined,    // optional one param function: array of objects { name: string, value: value },
        resetDefaultSettings: undefined,// optional one param function: array of objects { name: string, value: value },
    },

    initialize : function(settings, setSettings, validateSettings, resetDefaultSettings) {
        this.set("settings", settings);
        this.set("validateSettings", validateSettings);
        this.set("setSettings", setSettings);
        this.set("resetDefaultSettings", resetDefaultSettings);
    },
});