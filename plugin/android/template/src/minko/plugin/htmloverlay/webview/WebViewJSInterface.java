package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.JavascriptInterface;
import android.util.Log;

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
		Log.i("MINKO_JAVA", "[WebViewJSInterface] onMessage: " + message);
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onEvent(String accessor, String eventData)
	{
		Log.i("MINKO_JAVA", "[WebViewJSInterface] onEvent: " + eventData);
		minkoNativeOnEvent(accessor, eventData);
	}
	
	@JavascriptInterface
	public void onNativeJSResult(String jsResult)
	{
		Log.i("MINKO_JAVA", "[WebViewJSInterface] onNativeJSResult: " + jsResult);
		
		WebViewJSInterface.Result = jsResult;
		WebViewJSInterface.ResultReady = true;
	}
}