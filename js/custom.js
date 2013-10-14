/*jslint browser: true*/
/*global $, jQuery*/

$.fn.toggleClick = function () {
    'use strict';
    var methods = arguments, // store the passed arguments for future reference
        count = methods.length; // cache the number of methods 

    //use return this to maintain jQuery chainability
    return this.each(function (i, item) {
        // for each element you bind to
        var index = 0; // create a local counter for that element
        $(item).click(function () { // bind a click handler to that element
            return methods[index++ % count].apply(this,arguments); // that when called will apply the 'index'th method to that element
            // the index % count means that we constrain our iterator between 0 and (count-1)
        });
    });
};


function sampleRun() {
    'use strict';
    
    $('#flash-messsages').text('Turning motors on.')
        .delay(1000)
        .text('Taken off and holding still'); // TODO: no flash messages shown
    
    $('#data-kph')
        .text('20')
        .delay(1000)
        .text('21')
        .delay(1000)
        .text('22')
        .delay(1000)
        .text('23'); // TODO: animation not working, shows only 23
    
}

$(document).ready(function () {
    "use strict";
    
    // status led
    var ledSize = 32;
    $('#status-led').height(ledSize).width(ledSize)
        .css({ borderRadius: ledSize / 2, backgroundColor: "#5bb75b" });
    
    // initialize gridster
    $(".gridster > ul").gridster({
        widget_margins: [10, 10],
        widget_base_dimensions: [270, 150]
    });
    
    
    // sidepanels init
    //$("#left-menu").css("float", "left").css("background", "whitesmoke").css("overflow", "hidden").css("width", "50px").css("z-index", "1001");
    
    
    initSidepanels();
    
    sampleRun();
});

function initSidepanels() {
    var startWidth = 70;
    var endWidth = 300;
    
    $('.sidepanel').width(startWidth);
    $('.sidepanel li > a > h3').hide();
    
    $('.sidepanel-left-trigger').toggleClick(function () {
        $('.sidepanel-left').animate({
            width: endWidth
        }, 0);
        $('.sidepanel-left li > a > h3').show();
    }, function () {
        $(".sidepanel-left").animate({
            width: startWidth
        }, 0);
        $('.sidepanel-left li > a > h3').hide();
    });
    
    $('.sidepanel-right-trigger').toggleClick(function () {
        $('.sidepanel-right').animate({
            width: endWidth
        }, 0);
        $('.sidepanel-right li > a > h3').show();
    }, function () {
        $(".sidepanel-right").animate({
            width: startWidth
        }, 0);
        $('.sidepanel-right li > a > h3').hide();
    });
}
