if (!window.Minko)
	window.Minko = {};
Minko.loaded = -1;

var canvasElement = document.getElementById('canvas');
var iframeElement = document.createElement('iframe');

iframeElement.id = 'canvasiframe';
iframeElement.className = 'emscripten';

iframeElement.style.width = canvasElement.clientWidth + 'px';
iframeElement.style.height = canvasElement.clientHeight + 'px';

iframeElement.style.backgroundColor = 'transparent';
iframeElement.allowTransparency = 'true';
iframeElement.frameBorder = '0';

iframeElement.style.position = 'relative';
canvasElement.parentNode.style.position = 'relative';

canvasElement.style.position = 'absolute';
canvasElement.style.left = '0';
canvasElement.style.right = '0';

canvasElement.parentNode.appendChild(iframeElement);

Minko.iframeElement = iframeElement;
Minko.canvasElement = canvasElement;

Minko.getOffsetTop = function(element)
{
	var result = 0;
	while(element){
		result += element.offsetTop;
		element = element.offsetParent;
	}
	return result;
};

Minko.getOffsetLeft = function(element)
{
	var result = 0;
	while(element){
		result += element.offsetLeft;
		element = element.offsetParent;
	}
	return result;
};

Minko.redispatchKeyboardEvent = function(event)
{
	var eventCopy = document.createEvent('Event');

	eventCopy.initEvent(event.type, event.bubbles, event.cancelable);

	eventCopy.type = event.type;
	eventCopy.bubbles = event.bubbles;
	eventCopy.cancelable = event.cancelable;
	eventCopy.view = event.view;
	eventCopy.ctrlKey = event.ctrlKey;
	eventCopy.altKey = event.altKey;
	eventCopy.shiftKey = event.shiftKey;
	eventCopy.metaKey = event.metaKey;
	eventCopy.keyCode = event.keyCode;
	eventCopy.charCode = event.charCode;
	eventCopy.which = event.which;
	eventCopy.key = event.key;
	eventCopy.detail = event.detail;
	eventCopy.keyIdentifier = event.keyIdentifier;

	document.dispatchEvent(eventCopy);
}


Minko.redispatchMouseEvent = function(event)
{
	if (event.type == 'mouseout' && event.target != event.currentTarget)
		return;

	if (event.type == 'mouseover' && event.target != event.currentTarget)
		return;

	var pageX = 1 + Minko.getOffsetLeft(Minko.iframeElement) + (event.pageX || event.layerX);
	var pageY = 1 + Minko.getOffsetTop(Minko.iframeElement) + (event.pageY || event.layerY);

	var screenX = pageX - document.body.scrollLeft;
	var screenY = pageY - document.body.scrollTop;

	var eventCopy = document.createEvent('MouseEvents');
	eventCopy.initMouseEvent(event.type, event.bubbles, event.cancelable, event.view, event.detail,
		pageX, pageY, screenX, screenY, 
		event.ctrlKey, event.altKey, event.shiftKey, event.metaKey, event.button, event.relatedTarget);

	if (event.type == 'mousewheel')
	{
		eventCopy.detail = event.detail;

		eventCopy.wheelDelta = event.wheelDelta;
		eventCopy.wheelDeltaX = event.wheelDeltaX;
		eventCopy.wheelDeltaY = event.wheelDeltaY;
		eventCopy.wheelDeltaZ = event.wheelDeltaZ;

		eventCopy.delta = event.delta;
		eventCopy.deltaMode = event.deltaMode;
		eventCopy.deltaX = event.deltaX;
		eventCopy.deltaY = event.deltaY;
		eventCopy.deltaZ = event.deltaZ;
		event.preventDefault();
	}

	Minko.canvasElement.dispatchEvent(eventCopy);

}

Minko.iframeLoadHandler = function(event)
{
	if(Minko.loaded == -1)
		return;

	Minko.loaded = 1;
	if (!Minko.iframeElement.contentWindow.Minko)
		Minko.iframeElement.contentWindow.Minko = {};

	Minko.iframeElement.contentWindow.document.body.oncontextmenu = function(event){ event.preventDefault(); return false;};

	if (!Minko.iframeElement.contentWindow.Minko.onmessage)
	{
		Minko.iframeElement.contentWindow.Minko.onmessage = function(message)
		{
			console.log('MINKO: ' + message);
		}
	}
	Minko.iframeElement.contentWindow.Minko.messagesToSend = [];

	Minko.iframeElement.contentWindow.Minko.sendMessage = function(message)
	{
		Minko.iframeElement.contentWindow.Minko.messagesToSend.push(message);
	}

	Minko.iframeElement.addEventListener('mouseover',					Minko.redispatchMouseEvent);
	Minko.iframeElement.addEventListener('mouseout',					Minko.redispatchMouseEvent);
	
	Minko.iframeElement.contentWindow.addEventListener('mousemove',		Minko.redispatchMouseEvent);
	Minko.iframeElement.contentWindow.addEventListener('mouseup',		Minko.redispatchMouseEvent);
	Minko.iframeElement.contentWindow.addEventListener('mousedown',		Minko.redispatchMouseEvent);
	
	Minko.iframeElement.contentWindow.addEventListener('click',			Minko.redispatchMouseEvent);
	
	Minko.iframeElement.contentWindow.addEventListener('mousewheel',	Minko.redispatchMouseEvent);
	
	Minko.iframeElement.contentWindow.addEventListener('keydown',		Minko.redispatchKeyboardEvent);
	Minko.iframeElement.contentWindow.addEventListener('keyup',			Minko.redispatchKeyboardEvent);
	Minko.iframeElement.contentWindow.addEventListener('keypress',		Minko.redispatchKeyboardEvent);
}

iframeElement.onload = Minko.iframeLoadHandler;

Minko.addListener = function(accessor, type)
{
	if(!accessor.minkoEvents)
		accessor.minkoEvents = [];
	accessor.addEventListener(type, function(event)
	{
		accessor.minkoEvents.push(event);
	});
};

Minko.getEventsCount = function(accessor)
{
	if(accessor && accessor.minkoEvents)
		return accessor.minkoEvents.length;
	else
		return 0;
};

Minko.clearEvents = function(accessor)
{
	if(accessor)
		accessor.minkoEvents = [];
};