package minko.plugin.htmloverlay;

import android.webkit.WebView;
import java.util.concurrent.Callable;
import static android.util.Log.*;

public class EvalJSCallable implements Callable<String>
{
	private WebView _webView;
	private String _js;
	private int _uniqueId;
	
	public EvalJSCallable(WebView webView, String js, int uniqueId)
	{
		_webView = webView;
		_js = js;
		_uniqueId = uniqueId;
	}
	
	@Override
    public String call() throws Exception 
	{
		_webView.loadUrl("javascript:MinkoNativeInterface.onNativeJSResult(" + _uniqueId + ", eval(\"" + _js + "\"))");
		d("MINKOJAVA", "EVALJSCALLABLE");
		
		// We wait for the the result of JS eval
		// TODO: add a timeout
		while(true)
		{
			//d("MINKOJAVA", "ReturnValues size: " + WebViewJSInterface.ReturnValues.size());
			if (WebViewJSInterface.ReturnValues.containsKey(_uniqueId))
			{	
				String returnValue = WebViewJSInterface.ReturnValues.get(_uniqueId);
				WebViewJSInterface.ReturnValues.remove(_uniqueId);
				
				d("MINKOJAVA", "[ID = " + _uniqueId + "]Access to the value = " + returnValue);
				return returnValue;
			}
		}
    }
}