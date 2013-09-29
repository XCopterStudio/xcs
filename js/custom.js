/*jslint browser: true*/
/*global $, jQuery*/

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
    //var gridster = $('.gridster > ul').gridster().data('gridster');
    /*
    $('#big-view').click(function () {
        gridster.resize_widget($('#big-view'), 3, 3);
    });
    */
    
    
    //$('#left-menu').sidr();
    
    sampleRun();
});
