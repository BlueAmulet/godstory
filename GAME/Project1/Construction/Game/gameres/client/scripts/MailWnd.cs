//===========================================================================================================
// 文档说明:邮件界面操作
// 创建时间:2009-12-4
// 创建人: soar
//===========================================================================================================

$currentMailItem = 0;
$currentMailPage = 0;

function OpenMailWnd()
{
	if(!MailWnd.IsVisible())
	{
	  MailWnd.setvisible(1);
	  GameMainWndGui_Layer3.PushToBack(MailWnd);
	  $IsDirty++;
	  
	  MailWnd_Container.clearItems();
	  WriteMailGui();
	  $currentMailPage = 1;
	  $mailPageCount = 0;
	  $currentMailId = -1;
	  SptMail_Request($currentMailPage-1,6);
  }
  else
  {
  	 MailWnd.setvisible(0);
  	 $IsDirty--;
  }
}

function CloseMailWnd()
{
  MailWnd.setvisible(0);
  $IsDirty--;
}

function sendMailError()
{
	Dialog_OK_Content.setContent("错误：邮件标题或内容不能为空！");
	Dialog_OnlyOK.setVisible(1);
}

function SendMail()
{
	%playerId = MailWnd_SendmailPage_FriendNameButton.getSelected();
	%title = MailWnd_SendmailPage_Topic.getText();
	%content = MailWnd_SendmailPage_Edit.getText();
	if( %title $= "" || %content $= "" )
	{
		sendMailError();
		return ;
	}

	SptMail_Send(0,0,%title,%content,0, %playerId);
}

function RelayMail()
{
	MailWnd_SendmailPage_Topic.setText( "Re:"@ MailWnd_CollectionPage_Topic.text );
	MailWnd_SendmailPage_Edit.setText( "" );
	GuiMail_UpdateRecver();
	MailWnd_SendmailPage_FriendNameButton.setSelected( $mailSenderId );
	
	echo( "Mail player id: "@ $mailSenderId );
	
  MailWnd_GetMoneyButton.setVisible(0);
  MailWnd_SendmailButton.setText("发 送");
  MailWnd_SendmailButton.command = "SendMail();";
  MailWnd_SelfMoney.setVisible(1);
  MailWnd_SendmailPage.setVisible(1);
  MailWnd_CollectionPage.setVisible(0);
}

function addMailItem( %item, %key )
{
  MailWnd_Container.addItem( %item, %key );
}

function WriteMailGui()
{
	GuiMail_UpdateRecver();
  MailWnd_GetMoneyButton.setVisible(0);
  //MailWnd_WriteMailButton.setText("回 复");
  //MailWnd_WriteMailButton.command = "";
  MailWnd_SendmailButton.setText("发 送");
  MailWnd_SendmailButton.command = "SendMail();";
  MailWnd_SelfMoney.setVisible(1);
  MailWnd_SendmailPage.setVisible(1);
  MailWnd_CollectionPage.setVisible(0);
}

function ReadMailGui()
{
  MailWnd_GetMoneyButton.setVisible(1);
  MailWnd_SendmailButton.setText("回 复");
  MailWnd_SendmailButton.command = "RelayMail();";
  MailWnd_SelfMoney.setVisible(0);
  MailWnd_SendmailPage.setVisible(0);
  MailWnd_CollectionPage.setVisible(1);
}

function createMailItem( %name, %parent, %x, %y, %mail, %title, %senderName, %msg, %sendTime )
{
	%color = "255 255 255";
	%sendId = getWord( %mail, 0 );
	%mailId = getWord( %mail, 1 );
	
	if( %sendId == 0 )
	{
		%color = "20 255 20";
		%senderName = "系统";
	}
	
  %mailItem = new GuiBitmapCtrl(%name) {
        canSaveDynamicFields = "1";
        Enabled = "1";
        isContainer = "1";
        Profile = "GuiDefaultProfile";
        HorizSizing = "right";
        VertSizing = "bottom";
        position = %x @ " " @ %y;
        MinExtent = "8 2";
        Extent = "242 44";
        canSave = "1";
        Visible = "1";
        mustPrerender = "0";
        hovertime = "1000";
        bitmap = "gameres/gui/images/GUIWindow40_1_004";
        wrap = "0";
        
        mailContent = %msg;
        senderId = %sendId;
        mailId = %mailId;
        
        new GuiBitmapCtrl() {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiDefaultProfile";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "5 5";
          Extent = "32 32";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          wrap = "0";
        };
        new GuiTextCtrl(%name@"senderName") {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiCurrencyShowInfoTextProfile_4";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "49 4";
          Extent = "97 20";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          Margin = "0 0 0 0";
          Padding = "0 0 0 0";
          AnchorTop = "1";
          AnchorBottom = "0";
          AnchorLeft = "1";
          AnchorRight = "0";
          text = %senderName;
          maxLength = "8";
        };
        new GuiTextCtrl(%name@"title") {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiCurrencyShowInfoTextProfile_4";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "49 21";
          Extent = "110 20";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          Margin = "0 0 0 0";
          Padding = "0 0 0 0";
          AnchorTop = "1";
          AnchorBottom = "0";
          AnchorLeft = "1";
          AnchorRight = "0";
          text = %title;
          maxLength = "20";
        };
        new GuiTextCtrl(%name@"days") {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiCurrencyShowInfoTextProfile_1";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "215 -1";
          Extent = "24 20";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          Margin = "0 0 0 0";
          Padding = "0 0 0 0";
          AnchorTop = "1";
          AnchorBottom = "0";
          AnchorLeft = "1";
          AnchorRight = "0";
          text = "7天";
          maxLength = "1024";
        };
        new GuiTextCtrl(%name@"time") {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiCurrencyShowInfoTextProfile_1";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "179 26";
          Extent = "60 20";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          Margin = "0 0 0 0";
          Padding = "0 0 0 0";
          AnchorTop = "1";
          AnchorBottom = "0";
          AnchorLeft = "1";
          AnchorRight = "0";
          text = %sendTime;
          maxLength = "1024";
        };
        new GuiButtonCtrl(%name@"button") {
          canSaveDynamicFields = "0";
          Enabled = "1";
          isContainer = "0";
          Profile = "GuiShopSelectButtonProfile";
          HorizSizing = "right";
          VertSizing = "bottom";
          position = "0 0";
          Extent = "242 44";
          MinExtent = "8 2";
          canSave = "1";
          Visible = "1";
          mustPrerender = "0";
          hovertime = "1000";
          groupNum = "-1";
          buttonType = "RadioButton";
          useMouseEvents = "0";
          IsStateOn = "0";
          Command = "setCurrentMailItem(\""@ %name @"button\", "@%name@" );";
        };
      };
      
  %parent.add( %mailItem );
}

function setCurrentMailItem( %mailItem, %mail )
{
  $currentMailItem.setStateOn(0);
  $currentMailItem = %mailItem;
  $currentMail = %mail;
 
 	readMail( (%mail@"senderName").text, (%mail@"title").text, %mail.mailContent, 0, 0, 0, %mail.senderId );
  $currentMailId = %mail.mailId;
  ReadMailGui();
}

function readMail( %name, %title, %msg, %itemId, %itemCount, %money, %senderId )
{
	MailWnd_CollectionPage_Topic.setText( %title );
  MainWnd_CollectionPage_Text.setContent( %msg );
  MailWnd_CollectionPage_FriendName.setText( %name );
  $mailSenderId = %senderId;
  ReadMailGui();
}

function sendMailOk()
{
	MailWnd_SendmailPage_Topic.setText("");
	MailWnd_SendmailPage_Edit.setText("");
	
	Dialog_OK_Content.setContent("您的邮件发送成功！");
	Dialog_OnlyOK.setVisible(1);
}

// 清空邮件响应
function deleteAllMailAck()
{
	// 重新发送接收邮件请求
  MailWnd_Container.clearItems();
  WriteMailGui();
  $currentMailPage = 1;
  $mailPageCount = 0;
  $currentMailId = -1;
  SptMail_Request($currentMailPage-1,6);
}

function refreshMail()
{
  MailWnd_Container.clearItems();
  $currentMailPage = 1;
  $mailPageCount = 0;
  $currentMailId = -1;
  SptMail_Request($currentMailPage-1,6);
}

function deleteCurrentMail()
{
	SptMail_Delete( $currentMailId );
	Dialog_OK_Cancel.setVisible(0);
	refreshMail();
}

function deleteMailError()
{
	Dialog_OK_Content.setContent("请选择要删除的邮件！");
	Dialog_OnlyOK.setVisible(1);
}

function showDeleteMailDialog()
{
	if( $currentMailId == -1 )
	{
		deleteMailError();
		return ;
	}
		
	Dialog_OK_Cancel_OKBTN.command = "deleteCurrentMail();";
	Dialog_OK_Cancel_CANCBTN.command = "Dialog_OK_Cancel.setVisible(0);";
	Dialog_OK_Cancel_Content.setContent("确定要删除这封邮件吗？");
	Dialog_OK_Cancel.setVisible(1);
}

function deleteAllMail()
{
	Dialog_OK_Cancel_OKBTN.command = "deleteAllMailOk();";
	Dialog_OK_Cancel_CANCBTN.command = "Dialog_OK_Cancel.setVisible(0);";
	Dialog_OK_Cancel_Content.setContent("确定要清空邮件吗？");
	Dialog_OK_Cancel.setVisible(1);
}

function deleteAllMailOk()
{
	SptDeleteAllMail();
	Dialog_OK_Cancel.setVisible(0);
	refreshMail();
}

function setMailArgs( %recvCount, %letterCount, %packageCount )
{
	%pageCount = mfloor( %recvCount / 6 );
	if( %pageCount % 6 != 0 )
		%pageCount = %pageCount + 1;
		
	$mailPageCount = %pageCount;
	if( $mailPageCount == 0 )
		$currentMailPage = 0;
		
	if( %recvCount <= 6 && %recvCount > 0 )
	{
		$currentMailPage = 1;
		$mailPageCount = 1;	
	}
		
  %count = %recvCount @ "/30";
  if( %recvCount == 30 ) {
      %count = %count @ "(已满)";
      MailWnd_MailFullText.text = "已收信件已满，请及时删除";
    }
    
  MailWnd_CollectionNum.text = %count;
  
  MailWnd_NotReceiptParcellNum.text = %packageCount;
  MailWnd_NotReceiptMailNum.text = %letterCount;
  setMailPages();
}

function prviousMailPage()
{
	MailWnd_Container.clearItems();
	if( $currentMailPage > 1 )
		$currentMailPage = $currentMailPage - 1;
	
	setMailPages();
	SptMail_Request($currentMailPage-1,6);
}

function nextMailPage()
{
	MailWnd_Container.clearItems();
	if( $currentMailPage < $mailPageCount )
		$currentMailPage = $currentMailPage + 1;
	
	setMailPages();
	SptMail_Request($currentMailPage-1,6);
}

function setMailPages()
{
	MailWnd_MailPageNum.setText( $currentMailPage @"/"@ $mailPageCount );
}

function clearMailRecvers()
{
  MailWnd_SendmailPage_FriendNameButton.clear();
}

function addMailRecver( %name, %id )
{
  MailWnd_SendmailPage_FriendNameButton.add( %name, %id, 0 );
}
