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
		/*
		d("MINKOJAVA", "EVAL JS CALLABLE");
		d("MINKOJAVA", "Eval string: " + evalString);
		*/
		_webView.loadUrl(evalString);
		
		//_webView.loadUrl("javascript: eval(\"test[0]\");");
		
		long startTime = System.currentTimeMillis();
		long elapsedTime = 0L;
		
		// We wait for the the result of JS eval
		while(elapsedTime < 2000) // Timeout after X milliseconds
		{
			if (WebViewJSInterface.ResultReady)
			{
				//d("MINKOJAVA", "RESULT IS READY: " + WebViewJSInterface.Result);
				WebViewJSInterface.ResultReady = false;
				
				return WebViewJSInterface.Result;
			}
			
			elapsedTime = (new Date()).getTime() - startTime;
		}
		
		return "null";
    }
}