import ui
import uiToolTip
import uiCommon
import constInfo
import item
import dbg
import wndMgr
import net
import time
import app
from datetime import datetime, timedelta

class LuckyDrawWindow(ui.ScriptWindow):

    class JoinItemBox:
        gridBox = None
        tooltip = None
        vnum = 0
        count = 0

        def __init__(self, _gridBox, _tooltip, _vnum, _count):
            self.tooltip = _tooltip
            self.gridBox = _gridBox
            self.vnum = _vnum
            self.count = _count

            self.gridBox.SetItemSlot(0, self.vnum, self.count)
            self.gridBox.SetOverInItemEvent(self.OverIn)
            self.gridBox.SetOverOutItemEvent(self.OverOut)
            wndMgr.RefreshSlot(self.gridBox.GetWindowHandle())
        
        def OverOut(self):
            self.tooltip.HideToolTip()
            pass

        def OverIn(self, event):
            self.tooltip.SetItemToolTip(self.vnum)
            pass

        
    class AwardSlot:
        gridBox = None
        tooltip = None
        items = []

        def __init__(self, _gridBox, _tooltip, _items):
            self.tooltip = _tooltip
            self.gridBox = _gridBox
            self.items = _items

            self.gridBox.SetOverInItemEvent(self.OverIn)
            self.gridBox.SetOverOutItemEvent(self.OverOut)
            wndMgr.RefreshSlot(self.gridBox.GetWindowHandle())
        
        def OverOut(self):
            self.tooltip.HideToolTip()
            pass

        def OverIn(self, slotNumber):
            if slotNumber >= len(self.items):
                return
            vnum = self.items[slotNumber]
            self.tooltip.SetItemToolTip(vnum)
            pass


    class StretchedImageBox(ui.ExpandedImageBox):
        index = 0
        vnum = 0
        tooltip = None

        def __init__(self):
            ui.ExpandedImageBox.__init__(self)

        
        def SetNonStrechedImage(self, _tooltip):
            self.tooltip = _tooltip

            item.SelectItem(self.vnum)

            self.LoadImage(item.GetIconImageFileName())

            self.OnMouseOverIn = lambda selfArg = self : selfArg.OverIn()
            self.OnMouseOverOut = lambda selfArg = self : selfArg.OverOut()

        
        def SetStretchedImage(self, width, height, _tooltip):
            self.tooltip = _tooltip

            item.SelectItem(self.vnum)

            # Load the image
            self.LoadImage(item.GetIconImageFileName())
            
            # Get the original image size
            original_width = self.GetWidth()
            original_height = self.GetHeight()
            
            
            # Calculate the scaling factors
            scale_width = float(width) / original_width
            scale_height = float(height) / original_height
            
            # Set the new size
            self.SetSize(width, height)
            
            # Scale the image
            self.SetScale(scale_width, scale_height)
            
            # Update the render box to apply the new size
            self.UpdateRect()

            self.OnMouseOverIn = lambda selfArg = self : selfArg.OverIn()
            self.OnMouseOverOut = lambda selfArg = self : selfArg.OverOut()

        def OverIn(self):
            self.tooltip.SetItemToolTip(self.vnum)
            pass

        def OverOut(self):
            self.tooltip.HideToolTip()
            pass
        

    rows = []

    def __del__(self):
        ui.ScriptWindow.__del__(self)
    
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.isLoaded = False
        self.tooltipItem = uiToolTip.ItemToolTip()
        self.tooltipItem.HideToolTip()
        self.__LoadScript()

    def __LoadScript(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "uiscript/luckydraw.py")
        except:
            import exception
            exception.Abort("LuckyDrawWindow.__LoadScript.BindObject")

        self.titleBar = self.GetChild("TitleBar")
        self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

        self.winners_slots = [
            self.GetChild("WinnersSlot1"),
            self.GetChild("WinnersSlot2"),
            self.GetChild("WinnersSlot3")
        ]

        self.GetChild("WinnersTitle1").SetPackedFontColor(0xFFFFFF00)
        self.GetChild("WinnersTitle2").SetPackedFontColor(0xFFFF9300)
        self.GetChild("WinnersTitle3").SetPackedFontColor(0xFFc7c2bd)

        self.itemSlotWindow = self.GetChild("ItemSlot")

        #self.AddSlots()

        self.isLoaded = True
    
    def GetLeftTimeString(self, endTime):
        current_time = app.GetGlobalTimeStamp()

        time_left = endTime - current_time

        if time_left < 0:
            return "Çekiliþ bitti"

        days = int(time_left // (24 * 3600))
        time_left = time_left % (24 * 3600)
        hours = int(time_left // 3600)
        time_left %= 3600
        minutes = int(time_left // 60)
        seconds = int(time_left % 60)
        
        return ("{0}g {1}s {2}d {3}sn".format(days, hours, minutes, seconds))
    
    def Open(self, joinItemVnum, joinItemCount, entryPrice, endTime, totalParticipants,
              maxParticipants, maxParticipantsPerPlayer, playerParticipants,
                awardItems, winner1Name, winner2Name, winner3Name, isFinished, isWon):
        self.joinItemVnum = joinItemVnum
        self.joinItemCount = joinItemCount
        self.endTime = endTime
        self.totalParticipants = totalParticipants
        self.maxParticipants = maxParticipants
        self.maxParticipantsPerPlayer = maxParticipantsPerPlayer
        self.playerParticipants = playerParticipants
        self.awardItems = awardItems
        self.entryPrice = entryPrice
        self.winner1Name = winner1Name
        self.winner2Name = winner2Name
        self.winner3Name = winner3Name
        self.isFinished = isFinished
        self.isWon = isWon

        if False == self.isLoaded:
            self.__LoadScript()
        
        self.Show()

        formatted_price = "{:,}".format(self.entryPrice).replace(",", ".")
        self.GetChild("ParticipationFee").SetText(formatted_price)

        self.GetChild("ParticipantsCount").SetText("Katýlýmcýlar: "+str(self.totalParticipants)+"/"+str(self.maxParticipants))
        self.GetChild("MyParticipationCount").SetText("Katýlýmlarým: "+str(self.playerParticipants)+"/"+str(self.maxParticipantsPerPlayer))

        self.GetChild("WinnersTitle1").SetText("1.Kazanan:" + str(winner1Name))
        self.GetChild("WinnersTitle2").SetText("2.Kazanan:" + str(winner2Name))
        self.GetChild("WinnersTitle3").SetText("3.Kazanan:" + str(winner3Name))

        self.GetChild("JoinButton").SetEvent(ui.__mem_func__(self.AddParticipant))
        self.GetChild("ClaimButton").SetEvent(ui.__mem_func__(self.Claim))

        if self.isFinished == 1:
            self.GetChild("JoinButton").Hide()
        else:
            self.GetChild("JoinButton").Show()

        if self.isWon == 0:
            self.GetChild("ClaimButton").Hide()
        else:
            self.GetChild("ClaimButton").Show()

        self.AddSlots()
        pass
    
    def AddParticipant(self):
        net.SendLuckyDrawAddParcitipant()
        net.SendLuckyDrawCurrent()

    def Claim(self):
        net.SendLuckyDrawClaimAward()
        self.GetChild("ClaimButton").Hide()

    def OnUpdate(self):
        self.GetChild("TimerText").SetText(self.GetLeftTimeString(self.endTime))

    def AddSlots(self):
        joinItemBox = LuckyDrawWindow.JoinItemBox(self.itemSlotWindow, self.tooltipItem, self.joinItemVnum, self.joinItemCount)

        for i in range(3):
            row = []
            items = []
            gridBox = self.GetChild("WinnersGrid" + str(i + 1))
            j = 0
            for j in range(8):
                index = (i * 16) + (j * 2)
                if self.awardItems[index] != 0:
                    gridBox.SetItemSlot(j, self.awardItems[index], self.awardItems[index + 1])
                    items.append(self.awardItems[index])
                j += 1
            LuckyDrawWindow.AwardSlot(gridBox, self.tooltipItem, items)

    def Close(self):
        self.Hide()

    def OnClose(self):
        self.Close()
		
    def OnPressEscapeKey(self):
        self.Close()
        return True

