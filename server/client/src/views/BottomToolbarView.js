var BottomToolBarView = Backbone.View.extend({
    
    id: 'bottom-toolbar',
    
    views: [],
    
    initialize: function () {
        // empty
    },
    
    addView: function (triggerId, viewId) {
        this.views.push({ trigger: triggerId, view: viewId });
        
        var $trigger = $('#' + triggerId);
        var $view = $('#' + viewId);
        
        $view.hide();
        $trigger.expanded = false;
        
        $trigger.click(function () {
            if (!this.expanded) {
                $view.slideDown('fast');
                this.expanded = true;
                $(this).children('i').removeClass('icon-double-angle-up');
                $(this).children('i').addClass('icon-double-angle-down');
            }
            else {
                $view.slideUp('fast');
                this.expanded = false;
                $(this).children('i').removeClass('icon-double-angle-down');
                $(this).children('i').addClass('icon-double-angle-up');
            }
        });
    },
});