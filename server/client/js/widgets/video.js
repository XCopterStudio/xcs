$(function() {
    var video = document.getElementById("video-parrot");
    $(video).click(function() {
        if (video.paused) {
            video.play();
        } else {
            video.pause();
        }
    });
});
