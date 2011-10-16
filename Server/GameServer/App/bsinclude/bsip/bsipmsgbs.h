/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
*
* 文件名称：bsipmsgbs.h
* 摘    要：各接口消息体定义（client与LBS、CBS间信息交互）
* 当前版本：1.4.2
* 作    者：李闻
* 完成日期：2005年9月6日
*
* 修改内容：在1.3的版本上进行修改，删除弃用的消息
*           添加寄售交易使用的消息
* 修改时间：2005年9月6日
*
* 修改内容：剥离宏定义
* 修改时间：2005年9月21日
*
* 修改内容：Author接口添加account_id字段
*           Account接口添加account_id、map_id字段
* 修改时间：2005年10月8日
*
* 修改内容：添加Notify消息汇总接口
* 修改时间：2005年10月13日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock消息中的item_id等字段
* 修改时间：2005年11月25日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock消息中的item_num、item_price等字段
* 修改时间：2005年11月28日
*
* 修改内容：修改版本，增加NORMAL_STAT消息
* 修改时间：2005年12月27日
*
* 修改内容：修改了 AUTHEN消息, GOLDCONSUME 消息, DEPOSIT 消息
* 修改时间：2006年2月21日
*
* 修改内容：增加了 account_lock_ex,account_unlock_ex 消息
* 修改时间：2006年2月24日
*
* 修改内容：增加了 award_ack_res 消息
* 修改时间：2006年2月27日
*
* 修改内容：增加了 item_sync 消息 和 gs_route 消息
* 修改时间：2006年8月16日
*
* 修改内容：增加了 统一对战平台用的消息
* 修改时间：2006年11月15日
*
* 修改内容：所有char1改为int；ptid和sdid合并为userid；
*           废除部分多于字段；修改部分字段名；调整字段顺序
* 修改时间：2007-2-1 17:19
*
* 修改内容：增加ConsignTransfer消息，废除原寄售消息，支持一次预冻多次解冻，
*           修改道具信息为变长数据，支持传世传奇
* 修改时间：2007-5-3 9:25
*
* 修改内容：道具信息结构更改，
*           修改individual字段名为appendix，128字节，所有响应均添加该字段
* 修改时间：2007-5-3 9:25
*
* 修改内容：增加EkeyBind消息
* 修改时间：2007-6-19 15:54
*
* 修改内容：修改BsipNormalStatReq消息补全1.6中的13个消息，修改BsipStat加入响应延迟统计
* 修改时间：2007-8-1
*/

/*
*bsip msg body ,
*/

#ifndef _BSIP_MSG_BS_H_
#define _BSIP_MSG_BS_H_

#include "../msgdefineinfo.h"


/*------------------------------消息体定义------------------------------*/
/*-------------------------------时长计费-------------------------------*/
/*---------------------------------授权---------------------------------*/
typedef struct BsipAuthorReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  user_type;                      //用户类型
    int  status;                         //用户状态
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];   //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipAuthorReqDef;

typedef struct BsipAuthorRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间（保留字段）
    char end_time[MAX_TIME_LEN+1];       //失效时间（保留字段）
}BsipAuthorResDef;

/*---------------------------------扣费---------------------------------*/
typedef struct BsipAccountReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  amount;                         //消费数量
    int  discount;                       //计费折扣率
    int  map_id;                         //地图id
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char begin_time[MAX_TIME_LEN+1];     //上线时间
    char end_time[MAX_TIME_LEN+1];       //下线时间
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  trade_area_id;                  //交易区（仅适用于传世短信）
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipAccountReqDef;

typedef struct BsipAccountRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的点数、秒数或天数(对应pay_type)
    int  map_id;                         //地图id
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
}BsipAccountResDef;

/*-------------------------------按次计费-------------------------------*/
/*-------------------------------账户认证-------------------------------*/
typedef struct BsipAccountAuthenReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  user_type;                      //用户类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;                 //业务类型编号
    int  area_id_player;                 //游戏区编号
    int  group_id_player;                //表示组id
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  indication_authen;              //认证标志
    int  pay_type;                       //指定仅仅认证这一种余额
}BsipAccountAuthenReqDef;

typedef struct BsipAccountAuthenRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char begin_time[MAX_TIME_LEN+1];     //生效时间（保留字段）
    char end_time[MAX_TIME_LEN+1];       //失效时间（保留字段）
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char appendix[128];
}BsipAccountAuthenResDef;

///  goods_info 的结构
typedef struct BsipGoods
{
	int serv_id_item;                   //道具属主（游戏）编号
	int area_id_item;                   //道具属主（游戏区）编号
	int group_id_item;                  //道具属主（游戏组）编号
    int item_id;                        //游戏道具编号
    int item_num;                       //游戏道具数量
	int item_amount;                    //该游戏道具的总价（非单价）
	int discount;
}BsipGoodsDef;

///  item_info 的结构
typedef struct BsipItem
{ //因为这个结构直接操作与GS，所以不需要serviceid等信息
    int  item_id;                        //游戏道具编号
    int  item_num;                       //游戏道具数量
    char item_property[32];              //该游戏道具的属性
}BsipItemDef;

/*---------------------------------预冻---------------------------------*/
typedef struct BsipAccountLockReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  fee;                            //手续费
    int  lock_period;                    //lock相对时长
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;                        //业务类型编号
    int  area_id_player;                        //游戏区编号
    int  group_id_player;                       //表示组id
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
    int  indication_lock;
    int  item_num;                       //有效道具种类数
    BsipGoodsDef goods_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(BsipGoodsDef)
}BsipAccountLockReqDef;

typedef struct BsipAccountLockRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
}BsipAccountLockResDef;

/*---------------------------------解冻---------------------------------*/
typedef struct BsipAccountUnlockReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  pay_type;                       //付费类型
    int  amount;                   //解冻金额，可以是部分预冻金额，分多次解冻
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  confirm;                        //确认扣除
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;                        //业务类型编号
    int  area_id_player;                        //游戏区编号
    int  group_id_player;                       //表示组id
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
    int  indication_unlock;
    int  item_num;                       //有效道具种类数
    BsipGoodsDef goods_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(BsipGoodsDef)
}BsipAccountUnlockReqDef;

typedef struct BsipAccountUnlockRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余金额
    int  balance_locked;                 //被冻结的未被解冻的剩余金额
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char appendix[128];                  //附加信息
}BsipAccountUnlockResDef;

/*---------------------------------寄售转帐---------------------------------*/
typedef struct BsipConsignTransferReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  app_type;
    int  serv_id_player_dest;
    int  area_id_player_dest;           //区编号
    int  group_id_player_dest;          //组编号
    int  uid_type_dest;                 //买家帐号类型：1－pt帐户；2－数字id
    char user_id_dest[MAX_PTID_LEN+1];  //账号id（数字账号或pt账号）
    int  fee_dest;                      //得钱的人所要出的手续费
    int  pay_type;                      //付费的方式
    int  amount;                        //金额
    int  serv_id_player_src;            //游戏区编号
    int  area_id_player_src;            //游戏区编号
    int  group_id_player_src;           //表示组id
    int  uid_type_src;                  //帐号类型：1－pt帐户；2－数字id
    char user_id_src[MAX_PTID_LEN+1];   //账号id（数字账号或pt账号）
    int  fee_src;                       //付出钱的人所要出的手续费
    char sess_id[MAX_SESSID_LEN+1];     //会话id
    char context_id[MAX_ORDERID_LEN+1]; //关联id
    char order_id[MAX_ORDERID_LEN+1];   //唯一标识
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1]; //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];     //数据包发送时间
    int  indication_unlock;
    int  item_num;                      //有效道具种类数
    BsipGoodsDef goods_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(BsipGoodsDef)
}BsipConsignTransferReqDef;

typedef struct BsipConsignTransferRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费的方式
    int  uid_type_dest;                  //帐号类型：1－pt帐户；2－数字id
    char user_id_dest[MAX_PTID_LEN+1];   //买家账号id（数字账号或pt账号）
    int  balance_dest;                   //剩余PT币
    int  uid_type_src;                   //帐号类型：1－pt帐户；2－数字id
    char user_id_src[MAX_PTID_LEN+1];    //买家账号id（数字账号或pt账号）
    int  balance_locked_src;             //付钱方所剩被预冻得余额
    int  balance_src;                    //剩余PT币
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char appendix[128];                  //附加信息
}BsipConsignTransferResDef;

/*---------------------------------预冻扩展---------------------------------*/
typedef struct BsipAccountLockExReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  fee;
    int  lock_period;                    //lock相对时长
    int  pay_type1;                      //付费类型
    int  amount1;                        //付费金额（对应pay_type）
    int  pay_type2;                      //付费类型
    int  amount2;                        //付费金额（对应pay_type）
    int  pay_type3;                      //付费类型
    int  amount3;                        //付费金额（对应pay_type）
    int  pay_type4;                      //付费类型
    int  amount4;                        //付费金额（对应pay_type）
    int  pay_type5;                      //付费类型
    int  amount5;                        //付费金额（对应pay_type）
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  item_num;                       //有效道具种类数
    BsipGoodsDef goods_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(BsipGoodsDef)
}BsipAccountLockExReqDef;

typedef struct BsipAccountLockExRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数（对应pay_type）
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
}BsipAccountLockExResDef;

/*---------------------------------解冻扩展---------------------------------*/
typedef struct BsipAccountUnlockExReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  confirm;                        //确认扣除
    int  pay_type1;                      //付费类型
    int  amount1;                        //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type2;                      //付费类型
    int  amount2;                        //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type3;                      //付费类型
    int  amount3;                        //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type4;                      //付费类型
    int  amount4;                        //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type5;                      //付费类型
    int  amount5;                        //剩余的点数、秒数或天数（对应pay_type）
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  item_num;                       //有效道具种类数
    BsipGoodsDef goods_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(BsipGoodsDef)
}BsipAccountUnlockExReqDef;

typedef struct BsipAccountUnlockExRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数（对应pay_type）
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数（对应pay_type）
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char appendix[128];                  //附加信息
}BsipAccountUnlockExResDef;

/*---------------------------------领奖---------------------------------*/
/*-------------------------------领奖认证-------------------------------*/
typedef struct BsipAwardAuthenReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  user_type;                      //用户类型
    int  couple_type;                    //耦合类型
    int  award_type;                     //领奖类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char award_patchid[MAX_AWARDPATCH_LEN+1]; //领奖批次号
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    char role_name[32+1];                //玩家当前角色名
    int  current_level;                  //玩家当前角色级别
    int  award_level;                    //玩家领奖级别
    char ind_msg[128];                   //附加信息，保留
}BsipAwardAuthenReqDef;

typedef struct BsipAwardAuthenRes
{
    int  result;                         //返回结果
    int  couple_type;                    //耦合类型
    int  bind_user;                      //邦定用户标记
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //游戏客户端的IP地址
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
    int  item_num;                       //有效道具种类数
    BsipItemDef item_info[1];            //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 item_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipItemDef)
}BsipAwardAuthenResDef;

/*-------------------------------领奖确认-------------------------------*/
typedef struct BsipAwardAck
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  confirm;                        //
    int  award_type;                     //领奖类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char award_time[MAX_TIME_LEN+1];     //发奖成功、失败时间
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}BsipAwardAckDef;

typedef struct BsipAwardAckRes
{
    int  result;                         //返回结果
    int  couple_type;                    //耦合类型
    int  award_type;                     //奖品类型
    int  pay_type;                       //充值类型
    int  balance;                        //充值后剩余的余额
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char appendix[128];                  //附加信息
}BsipAwardAckResDef;

/*-------------------------------寄售交易-------------------------------*/
/*-------------------------------寄售预冻-------------------------------*/
typedef struct BsipConsignLockReq
{
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  lock_period;                    //lock相对时长
    int  discount;                       //计费折扣率
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipConsignLockReqDef;

typedef struct BsipConsignLockRes
{
    int  result;                         //返回结果
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}BsipConsignLockResDef;

/*-------------------------------寄售解冻-------------------------------*/
typedef struct BsipConsignUnlockReq
{
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  confirm;                        //确认扣除
    int  responsable;                    //是否需要响应标志
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipConsignUnlockReqDef;

typedef struct BsipConsignUnlockRes
{
    int  result;                         //返回结果
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}BsipConsignUnlockResDef;

/*-------------------------------寄售充值-------------------------------*/
typedef struct BsipConsignDepositReq
{
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  user_type;                      //用户类型
    int  pay_type;                       //付费的方式
    int  amount;                         //元宝金额
    int  buyer_serv_id;                  //业务类型
    int  buyer_area_id;                  //游戏区编号
    int  buyer_group_id;                 //表示组id
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    int  buyer_uid_type;                 //买家帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char buyer_user_id[MAX_PTID_LEN+1];  //买家账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipConsignDepositReqDef;

typedef struct BsipConsignDepositRes
{
    int  result;                         //返回结果
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  buyer_serv_id;                  //业务类型
    int  buyer_area_id;                  //游戏区编号
    int  buyer_group_id;                 //表示组id
    int  pay_type;                       //付费的方式
    int  balance;                        //剩余PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    int  buyer_uid_type;                 //买家帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char buyer_user_id[MAX_PTID_LEN+1];  //买家账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
}BsipConsignDepositResDef;

/*-------------------------------元宝交易-------------------------------*/
/*-------------------------------元宝充值-------------------------------*/
typedef struct BsipGoldDepositReq
{
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  user_type;                      //用户类型
    int  pay_type;                       //付费类型
    int  amount;                         //充值金额
    int  sync_flag;                      //充值消息标志位
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}BsipGoldDepositReqDef;

typedef struct BsipGoldDepositRes
{
    int  result;                         //返回结果
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型编号
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}BsipGoldDepositResDef;

/*-------------------------------元宝换时间-------------------------------*/
typedef struct BsipGoldConsumeLockReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                       //付费类型
    int  src_amount;                         //消费数量
    int  dest_pay_type;                      //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}BsipGoldConsumeLockReqDef;

typedef struct BsipGoldConsumeLockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}BsipGoldConsumeLockResDef;

typedef struct BsipGoldConsumeUnlockReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  confirm;                        //确认扣除
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                   //付费类型
    int  src_amount;                     //消费数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}BsipGoldConsumeUnlockReqDef;

typedef struct BsipGoldConsumeUnlockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}BsipGoldConsumeUnlockResDef;

/*-------------------------------给用户充值-------------------------------*/
typedef struct BsipAccountImportReq
{
    int  sp_id;                          //Service provider的编号
    int  serv_id;                        //业务类型
    int  area_id;                        //游戏区编号
    int  group_id;                       //表示组id
    int  pay_type;                       //付费的方式
    int  amount;                         //充值金额
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}BsipAccountImportReqDef;

typedef struct BsipAccountImportRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费的方式
    int  balance;                        //剩余金额
    int  uid_type;                      //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
}BsipAccountImportResDef;

/*-------------------------------发货系统-------------------------------*/
typedef struct BsipItemSyncReq
{
    int  sp_id;                          //暂无实际用处，预留
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    int  serv_id;                        //游戏应用编码
    int  area_id;                        //游戏区号
    int  group_id;                       //游戏组号
    int  sync_times;                     //同步次数
    int  indication_sync;                //指令消息 预留
    char msg_sync[256];                  //同步消息
    char item_info[256];                 //道具信息，格式为：
                                         //道具编号1,数量1;道具编号2,数量2;道具编号3,数量3;……
                                         //不同道具间使用半角分号分隔，
                                         //道具编号和数量之间用半角逗号分隔
}BsipItemSyncReqDef;

typedef struct BsipItemSyncRes
{
    int  result;                         //同步结果：0成功；其他失败
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识，与请求消息该字段一致
}BsipItemSyncResDef;

/*--------------------------------GS路由--------------------------------*/
typedef struct BsipGsRouteReq
{
    char order_id[MAX_ORDERID_LEN+1];    //由lbs自己构造，cbs不校验，原样返回
    char lbs_ip[IP_ADDRESS_LEN+1];       //lbs ip地址
    int  lbs_port;                       //lbs端口号
    char gs_ip[IP_ADDRESS_LEN+1];        //gs ip地址
    int  gs_port;                        //gs端口号
    char action;                         //动作，'1'表示gs连上lbs；'2'表示gs断开lbs
} BsipGsRouteReqDef;

typedef struct BsipGsRouteRes
{
    int  result;                         //0表示成功
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识，与请求消息该字段一致
} BsipGsRouteResDef;

/*--------------------------gs统计消息---------------------------------*/
typedef struct BsipStat
{
    int  total_num;
    int  success_num;
    int  response_num;
    int  delay1_num;                    //req和res时间差1－2秒
    int  delay2_num;                    //req和res时间差2－5秒
    int  delay3_num;                    //req和res时间差大于5秒
    int  delay4_num;                    //res超时
    int  callback_delay_num;			//callback时间超过100ms
}BsipStatDef;

typedef struct BsipNormalStatReq
{
    int  sp_id;                        //Service provider的编号
    int  serv_id;                      //业务类型编号
    int  area_id;                      //游戏区编号
    int  group_id;                     //表示组id
    char report_time[MAX_TIME_LEN+1];  //统计时间
    char ip[IP_ADDRESS_LEN+1];         //ip游戏客户端地址
    char order_id[MAX_ORDERID_LEN+1];  //数据包id
    BsipStatDef author;
    BsipStatDef account;
    BsipStatDef account_authen;
    BsipStatDef account_lock;
    BsipStatDef account_unlock;
    BsipStatDef account_lock_ex;
    BsipStatDef account_unlock_ex;
    BsipStatDef gold_consume_lock;
    BsipStatDef gold_consume_unlock;
    BsipStatDef token_deposit;
    BsipStatDef award_authen;
    BsipStatDef award_ack;
    BsipStatDef consign_transfer;
}BsipNormalStatReqDef;

/*抵用券通讯协议*/
typedef struct BsipTokenDepositReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  pay_type;                       //付费类型
    int  amount;                         //充值金额
    int  item_id;                        //购买或回收的道具编号
    int  item_num;                       //购买或回收的道具数量
    int  item_price;                     //购买或回收的道具单价
    int  deposit_flag;                   //充值消息标志位 1-购买返还 2-道具回收 其他-待定
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //order_id的相关id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  valid_period;                   //充值数额有效期，“月”
}BsipTokenDepositReqDef;

typedef struct BsipTokenDepositRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的抵用券
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //order_id的相关id
    char order_id[MAX_ORDERID_LEN+1];   //数据包id
    char appendix[128];                  //附加信息
}BsipTokenDepositResDef;

/*------------------------------密宝绑定---------------------------------*/
typedef struct BsipEkeyBindReq
{
    int  sp_id;                          //Service provider的编号
    int  site_code;
    int  operation_type;                 //1表示密宝；其他待定
    int  indication_bind;                //1－绑定密宝；2－解除绑定密宝
    int  pay_type;
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}BsipEkeyBindReqDef;

typedef struct BsipEkeyBindRes
{
    int  result;                         //返回结果
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
}BsipEkeyBindResDef;

#endif
