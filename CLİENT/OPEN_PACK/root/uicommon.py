import ui
import localeInfo
import app
import ime
import uiScriptLocale

if app.ENABLE_PLAYER_PIN_SYSTEM:
	import item, uiToolTip

class PopupDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.accceptButton = self.GetChild("accept")
			self.accceptButton.SetEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("PopupDialog.LoadDialog.BindObject")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()
		self.acceptEvent()

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetText(self, text):
		self.message.SetText(text)

	def SetAcceptEvent(self, event):
		self.acceptEvent = event

	def SetButtonName(self, name):
		self.accceptButton.SetText(name)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnIMEReturn(self):
		self.Close()
		return True

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		def GetTextSize(self):
			if self.message:
				return self.message.GetTextSize()			
			return (0,0)		
		def GetLineHeight(self):
			if self.message:
				return self.message.GetLineHeight()			
			return 0			
		def SetLineHeight(self, Height):
			self.message.SetLineHeight(Height)			
		def GetTextLineCount(self):
			return self.message.GetTextLineCount()
			

class InputDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialog.py")

		getObject = self.GetChild
		self.board = getObject("Board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputSlot = getObject("InputSlot")
		self.inputValue = getObject("InputValue")

	def Open(self):
		self.inputValue.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetNumberMode(self):
		self.inputValue.SetNumberMode()

	def SetSecretMode(self):
		self.inputValue.SetSecret()

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		width = length * 6 + 10
		self.SetBoardWidth(max(width + 50, 160))
		self.SetSlotWidth(width)
		self.inputValue.SetMax(length)

	def SetSlotWidth(self, width):
		self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
		self.inputValue.SetSize(width, self.inputValue.GetHeight())
		if self.IsRTL():
			self.inputValue.SetPosition(self.inputValue.GetWidth(), 0)

	def SetBoardWidth(self, width):
		self.SetSize(max(width + 50, 160), self.GetHeight())
		self.board.SetSize(max(width + 50, 160), self.GetHeight())
		if self.IsRTL():
			self.board.SetPosition(self.board.GetWidth(), 0)
		self.UpdateRect()

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetText(self):
		return self.inputValue.GetText()

class InputDialogWithDescription(InputDialog):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		if localeInfo.IsARABIC() :
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "inputdialogwithdescription.py")
		else:
			pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description = getObject("Description")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription(self, text):
		self.description.SetText(text)

class InputDialogWithDescription2(InputDialog):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription2.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description1 = getObject("Description1")
			self.description2 = getObject("Description2")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription1(self, text):
		self.description1.SetText(text)

	def SetDescription2(self, text):
		self.description2.SetText(text)

class QuestionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog.py")

		self.board = self.GetChild("board")
		self.textLine = self.GetChild("message")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SAFE_SetAcceptEvent(self, event):
		self.acceptButton.SAFE_SetEvent(event)

	def SAFE_SetCancelEvent(self, event):
		self.cancelButton.SAFE_SetEvent(event)

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)

	def SetCancelEvent(self, event):
		self.cancelButton.SetEvent(event)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetAcceptText(self, text):
		self.acceptButton.SetText(text)

	def SetCancelText(self, text):
		self.cancelButton.SetText(text)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	if app.ENABLE_EXTEND_INVEN_SYSTEM:
		def GetTextSize(self):
			if self.textLine:
				return self.textLine.GetTextSize()	
			return (0,0)		
		def GetLineHeight(self):
			if self.textLine:
				return self.textLine.GetLineHeight()			
			return 0			
		def SetLineHeight(self, Height):
			self.textLine.SetLineHeight(Height)		
		def GetTextLineCount(self):
			return self.textLine.GetTextLineCount()

class QuestionDialog2(QuestionDialog):

	def __init__(self):
		QuestionDialog.__init__(self)
		self.__CreateDialog()

	def __del__(self):
		QuestionDialog.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def SetText1(self, text):
		self.textLine1.SetText(text)

	def SetText2(self, text):
		self.textLine2.SetText(text)

class QuestionDialogWithTimeLimit(QuestionDialog2):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()
		self.endTime = 0

	def __del__(self):
		QuestionDialog2.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self, msg, timeout):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		self.SetText1(msg)
		self.endTime = app.GetTime() + timeout

	def OnUpdate(self):
		leftTime = max(0, self.endTime - app.GetTime())
		self.SetText2(localeInfo.UI_LEFT_TIME % (leftTime))

class MoneyInputDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.moneyHeaderText = localeInfo.MONEY_INPUT_DIALOG_SELLPRICE
		self.__CreateDialog()
		self.SetMaxLength(13)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/moneyinputdialog.py")

		getObject = self.GetChild
		self.board = self.GetChild("board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputValue = getObject("InputValue")
		self.inputValue.SetNumberMode()
		self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		self.moneyText = getObject("MoneyValue")

	def Open(self):
		self.inputValue.SetText("")
		self.inputValue.SetFocus()
		self.__OnValueUpdate()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		length = min(13, length)
		self.inputValue.SetMax(length)

	def SetMoneyHeaderText(self, text):
		self.moneyHeaderText = text

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def SetValue(self, value):
		value=str(value)
		self.inputValue.SetText(value)
		self.__OnValueUpdate()
		ime.SetCursorPosition(len(value))

	def GetRealMoney(self):
		return int(self.inputValue.GetText())

	def GetText(self):
		return self.inputValue.GetText()

	def __OnValueUpdate(self):
		ui.EditLine.OnIMEUpdate(self.inputValue)

		text = self.inputValue.GetText()

		money = 0
		if text and text.isdigit():
			try:
				money = long(text)
			except ValueError:
				money = 99999999999

		self.moneyText.SetText(self.moneyHeaderText + localeInfo.NumberToMoneyString(money))

if app.ENABLE_DROP_DIALOG_EXTENDED_SYSTEM:
	class ItemQuestionDialog2(ui.ScriptWindow):

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__CreateDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/itemquestiondialog.py")
			self.board = self.GetChild('board')
			self.textLine = self.GetChild('message')
			self.textLine2 = self.GetChild('message2')
			self.deleteButton = self.GetChild('deletebutton')
			self.sellButton = self.GetChild('sellbutton')
			self.cancelButton = self.GetChild('cancelbutton')

		def Open(self):
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.Hide()

		def SetWidth(self, width):
			height = self.GetHeight()
			self.SetSize(width, height)
			self.board.SetSize(width, height)
			self.SetCenterPosition()
			self.UpdateRect()

		def SetDeleteAcceptEvent(self, event):
			self.deleteButton.SetEvent(event)

		def SetSellAcceptEvent(self, event):
			self.sellButton.SetEvent(event)

		def SetCancelEvent(self, event):
			self.cancelButton.SetEvent(event)

		def SetText(self, text):
			self.textLine.SetText(text)

		def SetText2(self, text):
			self.textLine2.SetText(text)

		def OnPressEscapeKey(self):
			self.Close()
			return True


if app.ENABLE_CAOS_EVENT:
	class QuestionDialogCaosEvent(QuestionDialog):

		def __init__(self):
			QuestionDialog.__init__(self)
			self.__CreateDialog()

		def __del__(self):
			QuestionDialog.__del__(self)

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/questiondialogcaosevent.py")

			self.board = self.GetChild("board")
			self.textLine1 = self.GetChild("message1")
			self.textLine2 = self.GetChild("message2")
			self.acceptButton = self.GetChild("accept")
			self.cancelButton = self.GetChild("cancel")
			self.GetChild("TitleBar").SetCloseEvent(self.Close)

		def SetText1(self, text):
			self.textLine1.SetText(text)

		def SetText2(self, text):
					self.textLine2.SetText(text)

if app.ENABLE_PLAYER_PIN_SYSTEM:
	class InputPinCodeDialog(ui.ScriptWindow):
		PIN_CODE_TOOLTIP = [
			localeInfo.PIN_CODE_TOOLTIP_LINE1,
			localeInfo.PIN_CODE_TOOLTIP_LINE2,
			localeInfo.PIN_CODE_TOOLTIP_LINE3,
			localeInfo.PIN_CODE_TOOLTIP_LINE4,
			localeInfo.PIN_CODE_TOOLTIP_LINE5
		]

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__CreateDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InputPinCodeDialog.py")

			getObject = self.GetChild
			self.board = getObject("Board")
			self.pinCodeSlot = getObject("PinCodeSlot")
			self.pinCodeValue = getObject("PinCodeValue")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.toolTipButton = getObject("ToolTipButton")
			self.toolTip = self.CreateToolTip(localeInfo.INPUT_PIN_CODE_DIALOG_TITLE, self.PIN_CODE_TOOLTIP)
			self.toolTip.SetTop()
			self.toolTipButton.SetToolTipWindow(self.toolTip)

		def CreateToolTip(self, title, descList):
			toolTip = uiToolTip.ToolTip()
			toolTip.SetTitle(title)
			toolTip.AppendSpace(7)

			for desc in descList:
				toolTip.AutoAppendTextLine(desc)

			toolTip.AlignHorizonalCenter()
			toolTip.SetTop()
			return toolTip

		def Open(self):
			self.pinCodeValue.SetFocus()
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Close(self):
			self.ClearDictionary()
			self.board = None
			self.toolTipButton = None
			self.toolTip = None
			self.acceptButton = None
			self.cancelButton = None
			self.pinCodeSlot = None
			self.pinCodeValue = None
			self.Hide()

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetNumberMode(self):
			self.pinCodeValue.SetNumberMode()

		def SetUseCodePage(self, bUse = True):
			self.pinCodeValue.SetUseCodePage(bUse)

		def SetSecretMode(self):
			self.pinCodeValue.SetSecret()

		def SetFocus(self):
			self.pinCodeValue.SetFocus()

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.pinCodeValue.OnIMEReturn = event

		def SetCancelEvent(self, event):
			self.board.SetCloseEvent(event)
			self.cancelButton.SetEvent(event)
			self.pinCodeValue.OnPressEscapeKey = event

		def GetText(self):
			return self.pinCodeValue.GetText()

	class InputNewPinCodeDialog(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.__CreateDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __CreateDialog(self):
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/InputNewPinCodeDialog.py")

			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.pinCodeSlot = getObject("PinCodeSlot")
			self.pinCodeSlotCheck = getObject("PinCodeSlotCheck")
			self.pinCodeValue = getObject("PinCodeValue")
			self.pinCodeValueCheck = getObject("PinCodeValueCheck")

			self.pinCodeValue.SetTabEvent(lambda arg = 1: self.OnNextFocus(arg))
			self.pinCodeValueCheck.SetTabEvent(lambda arg = 2: self.OnNextFocus(arg))

		def OnNextFocus(self, arg):
			if 1 == arg:
				self.pinCodeValue.KillFocus()
				self.pinCodeValueCheck.SetFocus()
			elif 2 == arg:
				self.pinCodeValueCheck.KillFocus()
				self.pinCodeValue.SetFocus()

		def Open(self):
			self.pinCodeValue.SetFocus()
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Destroy(self):
			self.Close()

		def Close(self):
			self.ClearDictionary()
			self.board = None
			self.acceptButton = None
			self.cancelButton = None
			self.pinCodeSlot = None
			self.pinCodeSlotCheck = None
			self.pinCodeValue = None
			self.pinCodeValueCheck = None
			self.Hide()

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetNumberMode(self):
			self.pinCodeValue.SetNumberMode()
			self.pinCodeValueCheck.SetNumberMode()

		def SetUseCodePage(self, bUse = True):
			self.pinCodeValue.SetUseCodePage(bUse)
			self.pinCodeValueCheck.SetUseCodePage(bUse)

		def SetSecretMode(self):
			self.pinCodeValue.SetSecret()
			self.pinCodeValueCheck.SetSecret()

		def SetFocus(self):
			self.pinCodeValue.SetFocus()

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.pinCodeValueCheck.OnIMEReturn = event

		def SetCancelEvent(self, event):
			self.board.SetCloseEvent(event)
			self.cancelButton.SetEvent(event)
			self.pinCodeValue.OnPressEscapeKey = event
			self.pinCodeValueCheck.OnPressEscapeKey = event

		def GetText(self):
			return self.pinCodeValue.GetText()

		def GetTextCheck(self):
			return self.pinCodeValueCheck.GetText()