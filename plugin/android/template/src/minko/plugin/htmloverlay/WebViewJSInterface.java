package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.JavascriptInterface;
import java.util.HashMap;
import static android.util.Log.*;

public class WebViewJSInterface
{
	public static HashMap<Integer, String> ReturnValues;
		
	public WebViewJSInterface()
	{
		ReturnValues = new HashMap<Integer, String>();
	}
	
	@JavascriptInterface
	public void onmessage(String message)
	{
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onNativeJSResult(int uniqueId, String jsResult)
	{
		d("MINKOJAVA", "[ID = " + uniqueId + "]Get result from JS: " + jsResult);
		
		if (EvalJSManager.UniqueIds.indexOf(uniqueId) != -1)
		{
			WebViewJSInterface.ReturnValues.put(uniqueId, jsResult);
			EvalJSManager.UniqueIds.remove(uniqueId);
		}
	}
	
	public native void minkoNativeOnMessage(String message);
	public native void minkoNativeOnJSResult(String jsResult);
}