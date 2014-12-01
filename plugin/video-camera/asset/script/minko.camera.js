
var minko_camera = {
    frameId : 0,
    webcamBuffer : null,
    width : 0,
    height : 0,
    bufSize : 0
}

var getUserMedia = function(t, onsuccess, onerror) {
    var result = undefined;
    if (navigator.getUserMedia) {
        result = navigator.getUserMedia(t, onsuccess, onerror);
    } else if (navigator.webkitGetUserMedia) {
        result = navigator.webkitGetUserMedia(t, onsuccess, onerror);
    } else if (navigator.mozGetUserMedia) {
        result = navigator.mozGetUserMedia(t, onsuccess, onerror);
    } else if (navigator.msGetUserMedia) {
        result = navigator.msGetUserMedia(t, onsuccess, onerror);
    } else {
        onerror(new Error("No getUserMedia implementation found."));
    }
    return result;
};

var ready = false;
var URL = window.URL || window.webkitURL;
var onGetUserMediaSuccess = function(stream) {
    try {
        video.src = URL.createObjectURL(stream);
    } catch (error) {
        video.src = stream;
    }
    setTimeout(function() {
            video.play();
            ready = true;
            if( authorizedCallback ) {
                authorizedCallback();
            }
        }, 500);
}

var authorizedCallback = undefined;
var waitForAuthorization = function(callback) {
    authorizedCallback = callback;
    if(ready) {
        callback();
    }
}

var onGetUserMediaError = function(error) {
    alert("Couldn't access webcam.");
    console.log(error);
}

var video = document.createElement('video');
video.width = 640;
video.height = 480;
video.autoplay = true;

getUserMedia(
    {'video': true},
    onGetUserMediaSuccess,
    onGetUserMediaError
);

var getDimensions = function() {
    return {
        width:video.width,
        height:video.height
    }
}

var copyToContext = function(context) {
    if (video.readyState === video.HAVE_ENOUGH_DATA) {
        context.drawImage(video, 0, 0);
    }
}

var invisibleCanvas, context, detector, view = undefined;
function initialize() {    
    invisibleCanvas = document.createElement('canvas');
    var webcamDimensions = getDimensions();
    invisibleCanvas.width = webcamDimensions.width;
    invisibleCanvas.height = webcamDimensions.height;
    context = invisibleCanvas.getContext('2d');
    context.translate(invisibleCanvas.height * (video.width/video.height),0);
    context.scale(-1,1);
    var img_size = invisibleCanvas.width * invisibleCanvas.height * 4;
    minko_camera.webcamBuffer = Module._malloc(img_size);
    minko_camera.bufSize = img_size;
    minko_camera.width = invisibleCanvas.width;
    minko_camera.height = invisibleCanvas.height;
}

function tick() {
    copyToContext(context);

    var imageData = context.getImageData(0, 0, invisibleCanvas.width, invisibleCanvas.height);
    Module.HEAPU8.set(imageData.data, minko_camera.webcamBuffer);
    minko_camera.frameId++;

    window.requestAnimationFrame(tick);
}

waitForAuthorization( function() {
    initialize();
    tick();
});



var return_array = function() {
    console.log(minko_camera);
    
}
