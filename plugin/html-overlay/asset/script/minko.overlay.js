if (!window.Minko)
    var Minko = {};
else
    var Minko = window.Minko;

window.Minko = Minko;

Minko.bridge = null;
Minko.loaded = 0;
Minko.ready = false;

// Origins of the overlays allowed to send messages to the player.
Minko.overlayAllowedOrigins = [];
// Mapping between the origin of the overlay and the actual reference to the overlay.
// An entry is added only when an allowed overlay sends a message.
// Used to send messages to the overlays.
Minko.overlaySources = {};

Minko.bindJsErrors = function()
{
    Minko.window.onerror = function (message, url, linenumber)
    {
        var debugElement = Minko.document.getElementById("minkoOverlayDebug");

        if (debugElement)
            debugElement.innerHTML += message + " line " + linenumber + " " + url + "</br>";
        else
            console.error('JavaScript error: ' + message + ' on line ' + linenumber + ' for ' + url);
    }
}

Minko.loadedHandler = function(event)
{
    if (Minko.loaded == -1)
        return;

    delete Minko.window;
    delete Minko.document;
    delete Minko.onmessage;

    Minko.loaded = 1;

    if (Minko.iframe !== undefined)
    {
        Minko.window = event.currentTarget.contentWindow;
        Minko.document = event.currentTarget.contentDocument;
    }
    else
    {
        Minko.window = window;
        Minko.document = document;
    }

    if (Minko.window.Minko !== undefined)
    {
        for (var k in Minko.window.Minko)
        {
            Minko[k] = Minko.window.Minko[k];
        }
    }

    Minko.window.Minko = Minko;

    Minko.document.body.oncontextmenu = function(event)
    {
        if (event.dontPreventDefault || event.ignoreOnMinko)
            return true;

        event.preventDefault();
        return false;
    };

    Minko.onmessage = Minko.onmessage || function (message) { };

    if (Minko.platform != "androidWebView")
    {
        Minko.messagesToSend = [];

        // FIXME: Remove the old messaging logic once every platform uses `window.postMessage`.
        // See https://git.aerys.in/aerys/smartshape-engine/-/issues/278.
        Minko.sendMessage = function(message)
        {
            Minko.messagesToSend.push(message);
        }

        Minko.setWindowPostMessageListener();
    }

    Minko.bindJsErrors();

    Minko.ready = true;

    var event = document.createEvent("Event");

    event.initEvent("minkoReady", true, true);

    Minko.window.dispatchEvent(event);
}

Minko.listeners = {};

Minko.addEventListener = function(type, callback)
{
   if (!(Minko.listeners[type]))
       Minko.listeners[type] = [];

   Minko.listeners[type].push(callback);
};

Minko.removeEventListener = function(type, callback)
{
   if (!(Minko.listeners[type]))
       Minko.listeners[type] = [];

    var index = Minko.listeners[type].indexOf(callback);

    if (index != -1)
        Minko.listeners[type].splice(index, 1);
};

Minko.dispatchEvent = function(event)
{
   var callbacks = Minko.listeners[event.type];

   if (!callbacks)
       return;

   for(var i = 0; i < callbacks.length; ++i)
       callbacks[i](event);
};

Minko.dispatchMessage = function(message)
{
    // FIXME: Remove the old messaging logic once every platform uses `window.postMessage`.
    // See https://git.aerys.in/aerys/smartshape-engine/-/issues/278.
    var event = document.createEvent("Event");
    event.initEvent("message", true, true);
    event.message = message;

    Minko.dispatchEvent(event);
    Minko.onmessage(message);

    // Send the message to all the allowed overlays that have already sent a message.
    for (var origin in Minko.overlaySources)
        Minko.overlaySources[origin].postMessage(message, origin);
};

/*
** EMSCRIPTEN SPECIFIC CODE
*/

Minko.createIframe = function() //EMSCRIPTEN
{
    Minko.loaded = -1;

    var canvas = document.getElementById('canvas');
    var iframe = document.createElement('iframe');

    iframe.id = 'canvasiframe';
    iframe.className = 'emscripten';

    iframe.style.width = canvas.width + 'px';
    iframe.style.height = canvas.height + 'px';

    iframe.style.backgroundColor = 'transparent';
    iframe.allowTransparency = 'true';
    iframe.frameBorder = '0';

    iframe.style.position = 'relative';
    canvas.parentNode.style.position = 'relative';

    iframe.style.overflowX = 'hidden';
    iframe.style.overflowY = 'hidden';
    iframe.style.overflow = 'hidden';

    canvas.style.position = 'absolute';
    canvas.style.left = '0';
    canvas.style.right = '0';

    canvas.parentNode.appendChild(iframe);

    iframe.onload = Minko.loadedHandler;

    Minko.iframe = iframe;
    Minko.canvas = canvas;
}

Minko.loadUrlEmscripten = function(url) //EMSCRIPTEN
{
    Minko.iframe.src = url;
    Minko.loaded = 0;
}

/**
 * Set the origins of the overlays allowed to communicate with the player.
 */
Minko.setOverlayAllowedOrigins = function(allowedOrigins)
{
    Minko.overlayAllowedOrigins = allowedOrigins;
    Minko.overlaySources = {};
}

/**
 * Listen to messages coming from allowed overlays.
 * The overlays must use the standard `postMessage` method to send messages this way.
 */
Minko.setWindowPostMessageListener = function()
{
    if (Minko.platform == "emscripten")
    {
        window.addEventListener("message", (event) => {
            if (!Minko.overlayAllowedOrigins || !Array.isArray(Minko.overlayAllowedOrigins) || Minko.overlayAllowedOrigins.length <= 0)
                return;

            if (Minko.overlayAllowedOrigins.indexOf(event.origin) == -1)
                return;

            Minko.messagesToSend.push(event.data);

            if (!(event.origin in Minko.overlaySources))
            {
                Minko.overlaySources[event.origin] = event.source;
            }
        }, false);
    }

    // FIXME: Implement `window.postMessage` for all the supported platforms.
    // See https://git.aerys.in/aerys/smartshape-engine/-/issues/278.
}

/*
** iOS/OSX
*/
Minko.connectWebViewJavascriptBridge = function(callback) // iOS / OSX
{
    if (window.WebViewJavascriptBridge)
    {
        callback(WebViewJavascriptBridge);
    }
    else
    {
        document.addEventListener('WebViewJavascriptBridgeReady', function()
        {
            callback(WebViewJavascriptBridge);
        }, false);
    }
};

Minko.init = function(platform)
{
    Minko.platform = platform;

    if (platform == "emscripten")
    {
        Minko.createIframe();
    }
    else if (platform == "apple")
    {
        Minko.connectWebViewJavascriptBridge(function(bridge) // iOS / OSX
        {
            Minko.bridge = bridge;

            Minko.loadedHandler();

            bridge.init(function(message, responseCallback)
            {
            });

            bridge.send({type: 'ready', value:'true'});
        });

    }
    else if (platform == "androidWebView")
    {
        // FIXME: Remove the old messaging logic once every platform uses `window.postMessage`.
        Minko.sendMessage = function(message)
        {
            MinkoNativeInterface.onMessage(message);
        }

        Minko.loadedHandler();
    }
}

if (!Minko.platform)
    Minko.platform = "unknown";
