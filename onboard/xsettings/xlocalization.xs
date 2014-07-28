Ekf {
  ModelParameters 
  {
    c0  10.32
    c1  0
    c2  0.58
    c3  0
    c4  6.108
    c5  0.175
    c6  4.363 
    c7  0.175
    ;c7  0.000875
    c8  1.4
    c9  1.4
  }
  
  ModelVariance
  {
    roll  1
    pitch 1
    yaw   9
    gaz   1
  }
  
  ImuVariance
  {
    z           0.0004
    velocityX   0.0002
    velocityY   0.0002
    velocityZ   0.01
    phi         0.0003
    theta       0.0003
    velocityPsi 0.0003
  }

  PtamVariance
  {
    x     0.0001
    y     0.0001
    z     0.0001
    phi   0.0003
    theta 0.0003
    psi   0.0003
  }
}

Ptam {
  ; camera calibration for PTAM camera model
  CameraParameters {
    c0  0.771557
    c1  1.368560
    c2  0.552779
    c3  0.444056
    c4  1.156010
  }

  Ptam {
    ; frame buffer size of PTAM (pixels)
    frameWidth       640
    frameHeight      360

    ; display OpenGL window (debug, Linux only)
    showWindow       0

    ; minimum keyframe distance (meters)
    minKFDist        0.2
    ; min. distance between two KF (relative to mean scene depth)
    minKFWiggleDist  0.5
    
    ; min time between two KF.
    ;  => PTAM takes a new KF if (PTAMMinKFTimeDiff && (PTAMMinKFDist || 
    ;     PTAMMinKFWiggleDist)), and tracking is good etc.
    minKFTimeDist    0

    ; maximum no. of stored keyframes
    maxKF            60
  }
}
