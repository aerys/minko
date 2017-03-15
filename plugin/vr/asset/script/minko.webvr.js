// Retrieve VRDisplay device and store it into window.MinkoVR object
window.MinkoVR = {
  ready: false
};

function vrDeviceCallback(vrDisplays) {
  console.log('Found ' + vrDisplays.length + ' device(s).');

  for (var i = 0; i < vrDisplays.length; ++i) {
    var vrDisplay = vrDisplays[i];
    window.MinkoVR.vrDisplay = vrDisplay;

    console.log(vrDisplay.displayName)

    if (typeof(VRFrameData) != 'undefined')
        window.MinkoVR.vrFrameData = new VRFrameData();

    window.MinkoVR.ready = true;
    break;
  }
}

if (navigator.getVRDisplays !== undefined) {
    navigator.getVRDisplays().then(vrDeviceCallback);
}

// VRDisplay callbacks
window.MinkoVR.onVRRequestPresent = function() {
  var renderCanvas = document.getElementById('canvas');

  window.MinkoVR.vrLayer = {
    source: renderCanvas
  };

  window.MinkoVR.vrDisplay.requestPresent([window.MinkoVR.vrLayer]).then(
    function () {
      console.log('Success: requestPresent succeed.')
    },
    function () {
      console.log('Error: requestPresent failed.');
    }
  );
 };

window.MinkoVR.onVRExitPresent = function() {
  if (!window.MinkoVR.vrDisplay || !window.MinkoVR.vrDisplay.isPresenting)
    return;

  window.MinkoVR.vrDisplay.exitPresent().then(
    function () {
      console.log('Success: exitPresent succeed.')
    },
    function () {
      console.log('Error: exitPresent failed.');
    }
  );
}

function onVRPresentChange () {
    onResize();
}

window.MinkoVR.onResize = function() {
   console.log('onResize');
    // var renderCanvas = document.getElementById('canvas');
    // if (window.MinkoVR.vrDisplay && window.MinkoVR.vrDisplay.isPresenting) {
    //     var leftEye = window.MinkoVR.vrDisplay.getEyeParameters('left');
    //     var rightEye = window.MinkoVR.vrDisplay.getEyeParameters('right');
    //     renderCanvas.width = Math.max(leftEye.renderWidth, rightEye.renderWidth) * 2;
    //     renderCanvas.height = Math.max(leftEye.renderHeight, rightEye.renderHeight);
    // } else {
    //     renderCanvas.width = renderCanvas.offsetWidth * window.devicePixelRatio;
    //     renderCanvas.height = renderCanvas.offsetHeight * window.devicePixelRatio;
    // }
}

window.MinkoVR.bindEvents = function() {
  window.addEventListener('vrdisplayactivate', window.MinkoVR.onVRRequestPresent, false);
  window.addEventListener('vrdisplaydeactivate', window.MinkoVR.onVRExitPresent, false);
  window.addEventListener('vrdisplaypresentchange', window.MinkoVR.onVRPresentChange, false);
  window.addEventListener('resize', window.MinkoVR.onResize, false);
}

window.MinkoVR.unbindEvents = function() {
  window.removeEventListener('vrdisplayactivate', window.MinkoVR.onVRRequestPresent, false);
  window.removeEventListener('vrdisplaydeactivate', window.MinkoVR.onVRExitPresent, false);
  window.removeEventListener('vrdisplaypresentchange', window.MinkoVR.onVRPresentChange, false);
  window.removeEventListener('resize', window.MinkoVR.onResize, false);
}

window.MinkoVR.submitFrame = function() {
  if (!!window.MinkoVR.vrDisplay && window.MinkoVR.vrDisplay.isPresenting) {
    // Workaround To avoid a crash in Chrome VR
    if (!!window.MinkoVR.vrFrameData && !!window.MinkoVR.vrFrameData.pose && !!window.MinkoVR.vrFrameData.pose.orientation) {
      window.MinkoVR.vrDisplay.submitFrame();
    }
  }
}

window.MinkoVR.getOrientation = function() {
  if (!!window.MinkoVR.vrDisplay && !!window.MinkoVR.vrFrameData) {
    window.MinkoVR.vrDisplay.getFrameData(window.MinkoVR.vrFrameData);
    if (!!window.MinkoVR.vrFrameData && !!window.MinkoVR.vrFrameData.pose && !!window.MinkoVR.vrFrameData.pose.orientation) {
      return window.MinkoVR.vrFrameData.pose.orientation.join(' ');
    }
  }

  return null;
}

window.MinkoVR.getPosition = function() {
  if (!!window.MinkoVR.vrDisplay && !!window.MinkoVR.vrFrameData) {
    window.MinkoVR.vrDisplay.getFrameData(window.MinkoVR.vrFrameData);
    if (!!window.MinkoVR.vrFrameData.pose && !!window.MinkoVR.vrFrameData.pose.position) {
      return window.MinkoVR.vrFrameData.pose.position.join(' ');
    }
  }

  return null;
}

window.MinkoVR.getProjectionMatrices = function() {
  if (!!window.MinkoVR.vrDisplay && !!window.MinkoVR.vrFrameData) {
    window.MinkoVR.vrDisplay.getFrameData(window.MinkoVR.vrFrameData);
    if (!!window.MinkoVR.vrFrameData.leftProjectionMatrix && !!window.MinkoVR.vrFrameData.rightProjectionMatrix) {
      return window.MinkoVR.vrFrameData.leftProjectionMatrix.join(' ') + ' ' +
             window.MinkoVR.vrFrameData.rightProjectionMatrix.join(' ');
    }
  }

  return null;
}

window.MinkoVR.getLeftEyeFov = function() {
  if (!!window.MinkoVR.vrDisplay) {
    var eyeParameters = window.MinkoVR.vrDisplay.getEyeParameters("left");

    if (!!eyeParameters.fieldOfView &&
        !!eyeParameters.fieldOfView.upDegrees && !!eyeParameters.fieldOfView.downDegrees &&
        !!eyeParameters.fieldOfView.leftDegrees && !!eyeParameters.fieldOfView.rightDegrees)
    {
      return Math.max(
        Math.atan(eyeParameters.fieldOfView.upDegrees + eyeParameters.fieldOfView.downDegrees),
        Math.atan(eyeParameters.fieldOfView.leftDegrees + eyeParameters.fieldOfView.rightDegrees)
      );
    }
  }

  return 0.78; // 45°
}

window.MinkoVR.getRightEyeFov = function() {
  if (!!window.MinkoVR.vrDisplay) {
    var eyeParameters = window.MinkoVR.vrDisplay.getEyeParameters("right");

    if (!!eyeParameters.fieldOfView &&
        !!eyeParameters.fieldOfView.upDegrees && !!eyeParameters.fieldOfView.downDegrees &&
        !!eyeParameters.fieldOfView.leftDegrees && !!eyeParameters.fieldOfView.rightDegrees)
    {
      return Math.max(
        Math.atan(eyeParameters.fieldOfView.upDegrees + eyeParameters.fieldOfView.downDegrees),
        Math.atan(eyeParameters.fieldOfView.leftDegrees + eyeParameters.fieldOfView.rightDegrees)
      );
    }
  }

  return 0.78; // 45°
}

console.log('minko.webvr.js script successfully loaded');
