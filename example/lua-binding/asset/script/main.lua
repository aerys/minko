--[[
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]--

function main:start(root)
	self.canvas = getCanvas()
	local assets = 	getSceneManager().assets

	assets
		:queue('effect/Basic.effect')
		:queue('texture/box.png')
		:queue('script/camera.lua')
	
	self.assetsComplete = assets.complete:connect(function(assets)
		self.assetsComplete:disconnect()
		self.assetsComplete = nil

		self:initializeCamera(root, assets)

		self.cube = Node.create()
			:addComponent(Transform.create())
			:addComponent(Surface.create(
				CubeGeometry.create(assets.context),
				Material.create():setTexture('diffuseMap', assets:texture('texture/box.png')),
				assets:effect('effect/Basic.effect')
			))

		root:addChild(self.cube)
	end)

	-- Joystick connect event
	self.joystick = self.canvas:joystick(1)
	self.joystickButtonDown = self.joystick.joystickButtonDown:connect(function(joystick, which, button)						
		print("Button " .. button .. " pressed")
	end)

	assets:load()
end

function main:update(node)
	local keyboard	= self.canvas.keyboard

	self.cube:getTransform().matrix:prependRotationY(0.01)

	if keyboard:keyIsDown(Key.RIGHT) then
		self.cube:getTransform().matrix:appendTranslation(0.01, 0, 0)
	end
	if keyboard:keyIsDown(Key.LEFT) then
		self.cube:getTransform().matrix:appendTranslation(-0.01, 0, 0)
	end
	if keyboard:keyIsDown(Key.UP) then
		self.cube:getTransform().matrix:appendTranslation(0, 0, -0.01)
	end
	if keyboard:keyIsDown(Key.DOWN) then
		self.cube:getTransform().matrix:appendTranslation(0, 0, 0.01)
	end

	-- Test keyboard
	if keyboard:keyIsDown(Key.CANCEL) then
		print'CANCEL'
	end
	if keyboard:keyIsDown(Key.HELP) then
		print'HELP'
	end
	if keyboard:keyIsDown(Key.BACK_SPACE) then
		print'BACK_SPACE'
	end
	if keyboard:keyIsDown(Key.TAB) then
		print'TAB'
	end
	if keyboard:keyIsDown(Key.CLEAR) then
		print'CLEAR'
	end
	if keyboard:keyIsDown(Key.RETURN) then
		print'RETURN'
	end
	if keyboard:keyIsDown(Key.ENTER) then
		print'ENTER'
	end
	if keyboard:keyIsDown(Key.SHIFT) then
		print'SHIFT'
	end
	if keyboard:keyIsDown(Key.CONTROL) then
		print'CONTROL'
	end
	if keyboard:keyIsDown(Key.ALT) then
		print'ALT'
	end
	if keyboard:keyIsDown(Key.PAUSE) then
		print'PAUSE'
	end
	if keyboard:keyIsDown(Key.CAPS_LOCK) then
		print'CAPS_LOCK'
	end
	if keyboard:keyIsDown(Key.KANA) then
		print'KANA'
	end
	if keyboard:keyIsDown(Key.EISU) then
		print'EISU'
	end
	if keyboard:keyIsDown(Key.JUNJA) then
		print'JUNJA'
	end
	if keyboard:keyIsDown(Key.FINAL) then
		print'FINAL'
	end
	if keyboard:keyIsDown(Key.KANJI) then
		print'KANJI'
	end
	if keyboard:keyIsDown(Key.ESCAPE) then
		print'ESCAPE'
	end
	if keyboard:keyIsDown(Key.CONVERT) then
		print'CONVERT'
	end
	if keyboard:keyIsDown(Key.NONCONVERT) then
		print'NONCONVERT'
	end
	if keyboard:keyIsDown(Key.ACCEPT) then
		print'ACCEPT'
	end
	if keyboard:keyIsDown(Key.MODECHANGE) then
		print'MODECHANGE'
	end
	if keyboard:keyIsDown(Key.SPACE) then
		print'SPACE'
	end
	if keyboard:keyIsDown(Key.PAGE_UP) then
		print'PAGE_UP'
	end
	if keyboard:keyIsDown(Key.PAGE_DOWN) then
		print'PAGE_DOWN'
	end
	if keyboard:keyIsDown(Key.END) then
		print'END'
	end
	if keyboard:keyIsDown(Key.HOME) then
		print'HOME'
	end
	if keyboard:keyIsDown(Key.LEFT) then
		print'LEFT'
	end
	if keyboard:keyIsDown(Key.UP) then
		print'UP'
	end
	if keyboard:keyIsDown(Key.RIGHT) then
		print'RIGHT'
	end
	if keyboard:keyIsDown(Key.DOWN) then
		print'DOWN'
	end
	if keyboard:keyIsDown(Key.SELECT) then
		print'SELECT'
	end
	if keyboard:keyIsDown(Key.PRINT) then
		print'PRINT'
	end
	if keyboard:keyIsDown(Key.EXECUTE) then
		print'EXECUTE'
	end
	if keyboard:keyIsDown(Key.PRINTSCREEN) then
		print'PRINTSCREEN'
	end
	if keyboard:keyIsDown(Key.INSERT) then
		print'INSERT'
	end
	if keyboard:keyIsDown(Key.DELETE) then
		print'DELETE'
	end
	if keyboard:keyIsDown(Key._0) then
		print'_0'
	end
	if keyboard:keyIsDown(Key._1) then
		print'_1'
	end
	if keyboard:keyIsDown(Key._2) then
		print'_2'
	end
	if keyboard:keyIsDown(Key._3) then
		print'_3'
	end
	if keyboard:keyIsDown(Key._4) then
		print'_4'
	end
	if keyboard:keyIsDown(Key._5) then
		print'_5'
	end
	if keyboard:keyIsDown(Key._6) then
		print'_6'
	end
	if keyboard:keyIsDown(Key._7) then
		print'_7'
	end
	if keyboard:keyIsDown(Key._8) then
		print'_8'
	end
	if keyboard:keyIsDown(Key._9) then
		print'_9'
	end
	if keyboard:keyIsDown(Key.COLON) then
		print'COLON'
	end
	if keyboard:keyIsDown(Key.SEMICOLON) then
		print'SEMICOLON'
	end
	if keyboard:keyIsDown(Key.LESS_THAN) then
		print'LESS_THAN'
	end
	if keyboard:keyIsDown(Key.EQUALS) then
		print'EQUALS'
	end
	if keyboard:keyIsDown(Key.GREATER_THAN) then
		print'GREATER_THAN'
	end
	if keyboard:keyIsDown(Key.QUESTION_MARK) then
		print'QUESTION_MARK'
	end
	if keyboard:keyIsDown(Key.AT) then
		print'AT'
	end
	if keyboard:keyIsDown(Key.A) then
		print'A'
	end
	if keyboard:keyIsDown(Key.B) then
		print'B'
	end
	if keyboard:keyIsDown(Key.C) then
		print'C'
	end
	if keyboard:keyIsDown(Key.D) then
		print'D'
	end
	if keyboard:keyIsDown(Key.E) then
		print'E'
	end
	if keyboard:keyIsDown(Key.F) then
		print'F'
	end
	if keyboard:keyIsDown(Key.G) then
		print'G'
	end
	if keyboard:keyIsDown(Key.H) then
		print'H'
	end
	if keyboard:keyIsDown(Key.I) then
		print'I'
	end
	if keyboard:keyIsDown(Key.J) then
		print'J'
	end
	if keyboard:keyIsDown(Key.K) then
		print'K'
	end
	if keyboard:keyIsDown(Key.L) then
		print'L'
	end
	if keyboard:keyIsDown(Key.M) then
		print'M'
	end
	if keyboard:keyIsDown(Key.N) then
		print'N'
	end
	if keyboard:keyIsDown(Key.O) then
		print'O'
	end
	if keyboard:keyIsDown(Key.P) then
		print'P'
	end
	if keyboard:keyIsDown(Key.Q) then
		print'Q'
	end
	if keyboard:keyIsDown(Key.R) then
		print'R'
	end
	if keyboard:keyIsDown(Key.S) then
		print'S'
	end
	if keyboard:keyIsDown(Key.T) then
		print'T'
	end
	if keyboard:keyIsDown(Key.U) then
		print'U'
	end
	if keyboard:keyIsDown(Key.V) then
		print'V'
	end
	if keyboard:keyIsDown(Key.W) then
		print'W'
	end
	if keyboard:keyIsDown(Key.X) then
		print'X'
	end
	if keyboard:keyIsDown(Key.Y) then
		print'Y'
	end
	if keyboard:keyIsDown(Key.Z) then
		print'Z'
	end
	if keyboard:keyIsDown(Key.WIN) then
		print'WIN'
	end
	if keyboard:keyIsDown(Key.CONTEXT_MENU) then
		print'CONTEXT_MENU'
	end
	if keyboard:keyIsDown(Key.SLEEP) then
		print'SLEEP'
	end
	if keyboard:keyIsDown(Key.NUMPAD0) then
		print'NUMPAD0'
	end
	if keyboard:keyIsDown(Key.NUMPAD1) then
		print'NUMPAD1'
	end
	if keyboard:keyIsDown(Key.NUMPAD2) then
		print'NUMPAD2'
	end
	if keyboard:keyIsDown(Key.NUMPAD3) then
		print'NUMPAD3'
	end
	if keyboard:keyIsDown(Key.NUMPAD4) then
		print'NUMPAD4'
	end
	if keyboard:keyIsDown(Key.NUMPAD5) then
		print'NUMPAD5'
	end
	if keyboard:keyIsDown(Key.NUMPAD6) then
		print'NUMPAD6'
	end
	if keyboard:keyIsDown(Key.NUMPAD7) then
		print'NUMPAD7'
	end
	if keyboard:keyIsDown(Key.NUMPAD8) then
		print'NUMPAD8'
	end
	if keyboard:keyIsDown(Key.NUMPAD9) then
		print'NUMPAD9'
	end
	if keyboard:keyIsDown(Key.MULTIPLY) then
		print'MULTIPLY'
	end
	if keyboard:keyIsDown(Key.ADD) then
		print'ADD'
	end
	if keyboard:keyIsDown(Key.SEPARATOR) then
		print'SEPARATOR'
	end
	if keyboard:keyIsDown(Key.SUBTRACT) then
		print'SUBTRACT'
	end
	if keyboard:keyIsDown(Key.DECIMAL) then
		print'DECIMAL'
	end
	if keyboard:keyIsDown(Key.DIVIDE) then
		print'DIVIDE'
	end
	if keyboard:keyIsDown(Key.F1) then
		print'F1'
	end
	if keyboard:keyIsDown(Key.F2) then
		print'F2'
	end
	if keyboard:keyIsDown(Key.F3) then
		print'F3'
	end
	if keyboard:keyIsDown(Key.F4) then
		print'F4'
	end
	if keyboard:keyIsDown(Key.F5) then
		print'F5'
	end
	if keyboard:keyIsDown(Key.F6) then
		print'F6'
	end
	if keyboard:keyIsDown(Key.F7) then
		print'F7'
	end
	if keyboard:keyIsDown(Key.F8) then
		print'F8'
	end
	if keyboard:keyIsDown(Key.F9) then
		print'F9'
	end
	if keyboard:keyIsDown(Key.F10) then
		print'F10'
	end
	if keyboard:keyIsDown(Key.F11) then
		print'F11'
	end
	if keyboard:keyIsDown(Key.F12) then
		print'F12'
	end
	if keyboard:keyIsDown(Key.F13) then
		print'F13'
	end
	if keyboard:keyIsDown(Key.F14) then
		print'F14'
	end
	if keyboard:keyIsDown(Key.F15) then
		print'F15'
	end
	if keyboard:keyIsDown(Key.F16) then
		print'F16'
	end
	if keyboard:keyIsDown(Key.F17) then
		print'F17'
	end
	if keyboard:keyIsDown(Key.F18) then
		print'F18'
	end
	if keyboard:keyIsDown(Key.F19) then
		print'F19'
	end
	if keyboard:keyIsDown(Key.F20) then
		print'F20'
	end
	if keyboard:keyIsDown(Key.F21) then
		print'F21'
	end
	if keyboard:keyIsDown(Key.F22) then
		print'F22'
	end
	if keyboard:keyIsDown(Key.F23) then
		print'F23'
	end
	if keyboard:keyIsDown(Key.F24) then
		print'F24'
	end
	if keyboard:keyIsDown(Key.NUM_LOCK) then
		print'NUM_LOCK'
	end
	if keyboard:keyIsDown(Key.SCROLL_LOCK) then
		print'SCROLL_LOCK'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FJ_JISHO) then
		print'WIN_OEM_FJ_JISHO'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FJ_MASSHOU) then
		print'WIN_OEM_FJ_MASSHOU'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FJ_TOUROKU) then
		print'WIN_OEM_FJ_TOUROKU'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FJ_LOYA) then
		print'WIN_OEM_FJ_LOYA'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FJ_ROYA) then
		print'WIN_OEM_FJ_ROYA'
	end
	if keyboard:keyIsDown(Key.CIRCUMFLEX) then
		print'CIRCUMFLEX'
	end
	if keyboard:keyIsDown(Key.EXCLAMATION) then
		print'EXCLAMATION'
	end
	if keyboard:keyIsDown(Key.DOUBLE_QUOTE) then
		print'DOUBLE_QUOTE'
	end
	if keyboard:keyIsDown(Key.HASH) then
		print'HASH'
	end
	if keyboard:keyIsDown(Key.DOLLAR) then
		print'DOLLAR'
	end
	if keyboard:keyIsDown(Key.PERCENT) then
		print'PERCENT'
	end
	if keyboard:keyIsDown(Key.AMPERSAND) then
		print'AMPERSAND'
	end
	if keyboard:keyIsDown(Key.UNDERSCORE) then
		print'UNDERSCORE'
	end
	if keyboard:keyIsDown(Key.OPEN_PAREN) then
		print'OPEN_PAREN'
	end
	if keyboard:keyIsDown(Key.CLOSE_PAREN) then
		print'CLOSE_PAREN'
	end
	if keyboard:keyIsDown(Key.ASTERISK) then
		print'ASTERISK'
	end
	if keyboard:keyIsDown(Key.PLUS) then
		print'PLUS'
	end
	if keyboard:keyIsDown(Key.PIPE) then
		print'PIPE'
	end
	if keyboard:keyIsDown(Key.HYPHEN_MINUS) then
		print'HYPHEN_MINUS'
	end
	if keyboard:keyIsDown(Key.OPEN_CURLY_BRACKET) then
		print'OPEN_CURLY_BRACKET'
	end
	if keyboard:keyIsDown(Key.CLOSE_CURLY_BRACKET) then
		print'CLOSE_CURLY_BRACKET'
	end
	if keyboard:keyIsDown(Key.TILDE) then
		print'TILDE'
	end
	if keyboard:keyIsDown(Key.VOLUME_MUTE) then
		print'VOLUME_MUTE'
	end
	if keyboard:keyIsDown(Key.VOLUME_DOWN) then
		print'VOLUME_DOWN'
	end
	if keyboard:keyIsDown(Key.VOLUME_UP) then
		print'VOLUME_UP'
	end
	if keyboard:keyIsDown(Key.COMMA) then
		print'COMMA'
	end
	if keyboard:keyIsDown(Key.PERIOD) then
		print'PERIOD'
	end
	if keyboard:keyIsDown(Key.SLASH) then
		print'SLASH'
	end
	if keyboard:keyIsDown(Key.BACK_QUOTE) then
		print'BACK_QUOTE'
	end
	if keyboard:keyIsDown(Key.OPEN_BRACKET) then
		print'OPEN_BRACKET'
	end
	if keyboard:keyIsDown(Key.BACK_SLASH) then
		print'BACK_SLASH'
	end
	if keyboard:keyIsDown(Key.CLOSE_BRACKET) then
		print'CLOSE_BRACKET'
	end
	if keyboard:keyIsDown(Key.QUOTE) then
		print'QUOTE'
	end
	if keyboard:keyIsDown(Key.META) then
		print'META'
	end
	if keyboard:keyIsDown(Key.ALTGR) then
		print'ALTGR'
	end
	if keyboard:keyIsDown(Key.WIN_ICO_HELP) then
		print'WIN_ICO_HELP'
	end
	if keyboard:keyIsDown(Key.WIN_ICO_00) then
		print'WIN_ICO_00'
	end
	if keyboard:keyIsDown(Key.WIN_ICO_CLEAR) then
		print'WIN_ICO_CLEAR'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_RESET) then
		print'WIN_OEM_RESET'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_JUMP) then
		print'WIN_OEM_JUMP'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_PA1) then
		print'WIN_OEM_PA1'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_PA2) then
		print'WIN_OEM_PA2'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_PA3) then
		print'WIN_OEM_PA3'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_WSCTRL) then
		print'WIN_OEM_WSCTRL'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_CUSEL) then
		print'WIN_OEM_CUSEL'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_ATTN) then
		print'WIN_OEM_ATTN'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_FINISH) then
		print'WIN_OEM_FINISH'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_COPY) then
		print'WIN_OEM_COPY'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_AUTO) then
		print'WIN_OEM_AUTO'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_ENLW) then
		print'WIN_OEM_ENLW'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_BACKTAB) then
		print'WIN_OEM_BACKTAB'
	end
	if keyboard:keyIsDown(Key.ATTN) then
		print'ATTN'
	end
	if keyboard:keyIsDown(Key.CRSEL) then
		print'CRSEL'
	end
	if keyboard:keyIsDown(Key.EXSEL) then
		print'EXSEL'
	end
	if keyboard:keyIsDown(Key.EREOF) then
		print'EREOF'
	end
	if keyboard:keyIsDown(Key.PLAY) then
		print'PLAY'
	end
	if keyboard:keyIsDown(Key.ZOOM) then
		print'ZOOM'
	end
	if keyboard:keyIsDown(Key.PA1) then
		print'PA1'
	end
	if keyboard:keyIsDown(Key.WIN_OEM_CLEAR) then
		print'WIN_OEM_CLEAR'
	end

	-- Additional keys (specific to Minko and for native support of some keys)
	if keyboard:keyIsDown(Key.CONTROL_RIGHT) then
		print'CONTROL_RIGHT'
	end
	if keyboard:keyIsDown(Key.SHIFT_RIGHT) then
		print'SHIFT_RIGHT'
	end

end

function main:initializeCamera(root, assets)
	self.camera = Node.create()
	self.camera.name = 'mainCamera'
	self.camera:addComponent(assets:script('script/camera.lua'))
	root:addChild(self.camera)
end