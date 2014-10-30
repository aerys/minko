package minko.plugin.htmloverlay;

import android.webkit.WebView;
import java.util.concurrent.Callable;
import java.util.Date;
import static android.util.Log.*;

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
		String evalString = "javascript:MinkoNativeInterface.onNativeJSResult(eval(\"" + _js + "\"));";		
		
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
		
		// The JS eval didn't return anything
		return "null";
    }
}