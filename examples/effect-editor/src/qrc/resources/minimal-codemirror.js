var delay;

var codeMirror = CodeMirror(
	document.body,
	{
		value: "int square(int x){ return x*x; }\n",
		mode: "text/x-c++src",
		smartIndent: true,
		indentWithTabs: true,
		tabSize: 4,
		lineWrapping: true,
		lineNumbers: true,
		highlightSelectionMatches: {showToken: /\w/}
	}
);

codeMirror.on("change", function()
{
	clearTimeout(delay);
	delay = setTimeout(updatePreview, 300);
});
      
function updatePreview()
{
	//var previewFrame = document.getElementById('preview');
	//var preview =  previewFrame.contentDocument ||  previewFrame.contentWindow.document;
	//preview.open();
	//preview.write(codeMirror.getValue());
	//preview.close();
	qMinkoEffectEditor.sourcesChangedSlot();
}
setTimeout(updatePreview, 300);
	  
