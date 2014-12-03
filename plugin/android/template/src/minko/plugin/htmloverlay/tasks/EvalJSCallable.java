package minko.plugin.htmloverlay;

import android.webkit.WebView;
import java.util.concurrent.Callable;
import java.util.Date;
import android.util.Log;

public class EvalJSCallable implements Callable<String>
{
	private WebView _webView;
	private String _js;
	
	public EvalJSCallable(WebView webView, String js)
	{
		_webView = webView;
		_js = js;
	}
	
	@Override
    public String call() throws Exception 
	{
		String evalString = "javascript:MinkoNativeInterface.onNativeJSResult(\"" + _js + "\", eval(\"" + _js + "\"));";		
		
		_webView.loadUrl(evalString);
		
		long startTime = System.currentTimeMillis();
		long elapsedTime = 0L;
		
		// We wait for the the result of JS eval
		while(elapsedTime < 100) // Timeout after X milliseconds
		{
			if (WebViewJSInterface.ResultReady)
			{
				WebViewJSInterface.ResultReady = false;
				
				return WebViewJSInterface.Result;
			}
			
			elapsedTime = (new Date()).getTime() - startTime;
		}
		
		Log.e("minko-java", "[EvalJSCallable] For some reason, the eval of the following javascript expression didn't retrieve a result in time: " + _js);
		
		// The JS eval didn't return anything in time
		return "null";
    }
}