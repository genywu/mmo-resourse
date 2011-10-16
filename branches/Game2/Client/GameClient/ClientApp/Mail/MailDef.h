
#define		PERSONAL_MAIL		"PersonalMail"
#define		SEND				"PersonalMail/Send"	
#define		RECVER				"PersonalMail/Recver"
#define		REJECT				"PersonalMail/Reject"
#define		TITLE				"PersonalMail/Title"
#define     CONTENT				"PersonalMail/Content"
#define     READ				"PersonalMail/Read"
#define		RECVBOX				"PersonalMail/RecvBox"
#define		DELMAIL				"PersonalMail/DelMail"	
#define		MAILLIST			"PersonalMail/MailList"
#define		CHECKBOX0			"PersonalMail/Mail0"
#define		CHECKBOX1			"PersonalMail/Mail1"
#define		CHECKBOX2			"PersonalMail/Mail2"
#define		READMAIL			"PersonalMail/RecvContent"
//点击邮件中物品时取物品
#define		GETGOODSCONTAINER	"PersonalMail/MailContainer"
#define		GETGOODS0			"PersonalMail/MailContainer/GetGoods0"
#define     GETGOODS1			"PersonalMail/MailContainer/GetGoods1"
#define     GETGOODS2			"PersonalMail/MailContainer/GetGoods2"
#define     GETGOODS3			"PersonalMail/MailContainer/GetGoods3"

//发送邮件时添加物品
#define		ADDGOODSCONTAINER	"PersonalMail/GoodsList"
#define		ADDGOODS0			"PersonalMail/GoodsList/AddGoods0"
#define		ADDGOODS1			"PersonalMail/GoodsList/AddGoods1"
#define		ADDGOODS2			"PersonalMail/GoodsList/AddGoods2"
#define		ADDGOODS3			"PersonalMail/GoodsList/AddGoods3"

//付款取信
#define		PAYMAILSEL			"PersonalMail/PayMailSel"
//寄送金额与应付金额文本转换
#define		MONEY				"PersonalMail/PresentMoney_PayMoney"
//领取金币
#define		GOLD				"PersonalMail/Gold"
#define		GETGOLD				"PersonalMail/GetGold"

//同意付费
#define		AGREEPAY			"PersonalMail/AgreePay"


//event
#define		EVENT_ADDGOODS		"addgoodstomail"
#define		EVENT_DELGOODS      "delgoods"

//table
#define		STATIC_SEND			"PersonalMail/Send"
#define		STATIC_RECVBOX		"PersonalMail/RecvBox"
#define		STATIC_READ			"PersonalMail/Read"
#define		STATIC_DEL			"PersonalMail/DelMail"
#define		STATIC_REJECT		"PersonalMail/Reject"
#define		STATIC_AGREEPAY		"PersonalMail/AgreePay"
#define		STATIC_GETGOLD		"PersonalMail/GetGold"

#define		STATIC_RECVR		"PersonalMail/Recvr"
#define		STATIC_TITLE		"PersonalMail/staticTitle"
#define		STATIC_SENDCONTENT	"PersonalMail/SendContent"
#define		STATIC_GOODSSEND	"PersonalMail/GoodsSend"
#define		STATIC_GOODSRECV	"PersonalMail/GoodsRecv"
#define		STATIC_GOLD			"PersonalMail/StaticGold"
#define		STATIC_MONEY		"PersonalMail/Money"
#define		STATIC_PAYMAILSEL	"PersonalMail/StaticPayMailSel"
#define		STATIC_REVCCONTENT	"PersonalMail/StaticRead"




//#define		eIDS_MAIL_OPENFAILD							"玩家死亡，无法打开邮箱！"
//#define		eIDS_MAIL_TYPEERROR 						"效验邮件类型出错，请检查程序逻辑！"
//#define		eIDS_MAIL_ADDGOODSFAILD 					"此物品与之前的附件物品类型不符，物品添加失败！"
//#define		eIDS_MAIL_READFAILD ,						"阅读邮件失败！"
//#define		eIDS_MAIL_GOODS_CLICKPAYMENT 				"不能获取物品，请点击付款收信按钮！"
//#define		eIDS_MAIL_GOLDS_CLICKPAYMENT 				"不能获取金币，请点击付款收信按钮！"
//#define		eIDS_MAIL_PAYMENT 							"付款"
//#define		eIDS_MAIL_PRESENT 							"赠送"
//#define		eIDS_MAIL_REQUEST 							"索取"
//#define		eIDS_MAIL_PRESENT_NOGOLD 					"赠送这些物品所需的星钻不够，发送失败！"
//#define		eIDS_MAIL_PRESENT_ASSURE 					"您确实要赠送这些物品吗！"
//#define		eIDS_MAIL_REQUEST_ASSURE 					"您确实要要向对方索要这些物品吗！"
//#define		eIDS_MAIL_NO_GOLDS							"您的金币不足，发送失败！"
//#define		eIDS_MAIL_ISPAYGOLD							"是否确定让对方付费收信？"
//#define		eIDS_MAIL_NOTDEL_BYPAYGOLD 					"付费类信件无法删除！"
//#define		eIDS_MAIL_ISDELMAIL_HAVEGOODS 				"您选中要删除的信件当中含有物品附件，请确认是否删除？"
//#define		eIDS_MAIL_ISDELMAIL 						"是否确定删除所选信件？"
//#define		eIDS_MAIL_ISPAYTOACCEPT 					"是否同意以%d金币价格收取信件中的附件？"
//#define		eIDS_MAIL_NOTPAYTOACCEPT 					"您的金币不足，不能付款！"
//#define		eIDS_MAIL_ISAGREEDPRESENT 					"是否同意以%d星钻的价格将邮件中的附件赠送给%s？"
//#define		eIDS_MAIL_UPDATAGOODSLIST 					"附件中有商品已经撤柜无法购买，信件中予以删除！"
//#define		eIDS_MAIL_CANNOTADDGOODS 					"绑定物品不能作为附件寄送！"
////////////////////////////////////////////////////////////////////////////
//#define		eIDS_MAIL_ADDGOODSFAILD_BYMULIT 			"带有附件物品或金币的信件不能群发！"
//#define		eIDS_MAIL_GETNEWMAIL 						"您有新的信件，是关于%s。"
//#define		eIDS_MAIL_OPENFAILD_BYSHOPORTRADE 			"您正处于买卖或交易状态，不能打开邮箱！"
//#define		eIDS_MAIL_SENDTOSELF 						"对不起，不能给自己发送邮件！"
//#define		eIDS_MAIL_GETGOODSFAILED 					"取附件物品失败，请检查背包是否有空位！"
//#define		eIDS_MAIL_GROUPSEND 						"您确定要花费%d银币群发此邮件？"
//#define		eIDS_MAIL_NEWMAIL 							"您有新的邮件，请注意查收！"
//#define		eIDS_MAIL_HAVEGOODSORGOLD 					"您是否确定在邮件中附赠金币或物品？"
//#define		eIDS_MAIL_PAYNOGOLDS 						"请正确输入付款收信的金额！"
////////////////////////////////////////////////////////////////////////////
//#define eIDS_MAIL_SENDING_FAILED_INVALID_POSTAGE 			"邮资不够，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_SUBJECT 			"主题包含非法字符，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_TEXT 			"内容包含非法字符，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_EXTEXT 			"附加内容包含非法字符，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_GOLD 			"金币超出限制，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_GOODS 			"物品超出限制，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_MAXNAME 			"名字过长，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_MAXSUBJECT 		"主题过长，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_MAXTEXT 			"文字内容过长，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_INVALID_NOPLAYER 		"没有找到收信人，发送失败！"
//#define eIDS_MAIL_SENDING_FAILED_NOPLAYERORTITLE 			"没有收信人或主题，发送失败！"
//
//
//
//
//#define eIDS_MAIL_GOODS_FULL							"添加物品已满"