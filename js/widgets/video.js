var myVideo=document.getElementById("video-parrot"); 

function playPause()
{ 
  if (myVideo.paused) 
    myVideo.play(); 
  else 
    myVideo.pause(); 
} 
