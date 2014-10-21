package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.JavascriptInterface;

import static android.util.Log.*;

public class WebViewJSInterface
{
	public static boolean ResultReady = false;
	public static String Result = "[UNKNOWN]";

	// Native functions
	public native void minkoNativeOnMessage(String message);
	public native void minkoNativeOnEvent(String accessor, String eventData);
	
	@JavascriptInterface
	public void onMessage(String message)
	{
		d("MINKOJAVA", "On message: " + message);
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onEvent(String accessor, String eventData)
	{
		d("MINKOJAVA", "On event: " + eventData);
		minkoNativeOnEvent(accessor, eventData);
	}
	
	@JavascriptInterface
	public void onNativeJSResult(String jsResult)
	{
		WebViewJSInterface.Result = jsResult;
		WebViewJSInterface.ResultReady = true;
	}
}