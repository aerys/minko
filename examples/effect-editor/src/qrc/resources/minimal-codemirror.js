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
	qMinkoEffectEditor.updateSource(qTabJSObject["tabIndex"]);
}
setTimeout(updatePreview, 300);

codeMirror.setSize("100%", "100%");
