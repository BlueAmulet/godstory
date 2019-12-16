//==================================================================================
//注：所有脚本开头，均需注明本脚本的大致说明文字
//
//
//副本功能脚本，服务器端
//==================================================================================


//// 关闭副本
//SptCopymap_Close( %copymapInstId );
//
//// 开启副本
//SptCopymap_Open( %copymapId, %playerId );
//
//// 将玩家传送到副本
//SptCopyMap_TransportPlayer( %playerId, %copymapInstId, %pos, %triggerId );
//
//示例：SptCopyMap_TransportPlayer( %playerId, %copyMapInstId, "23.1431 -14.3424 127.587", 0 );
//
//// 将玩家请出副本
//SptCopymap_RemovePlayer( %playerId );
//
//// 将物体扔到制定的副本中
//obj.setLayerId( %copymapInstId );
//
//// 得到物体的副本ID
//obj.getLayerId();
//
////------------------------------------------------------------------------------
//
////事件
//
//// 玩家进入副本
//SptCopymap_OnPlayerEnter( %playerId );
//
//// 玩家离开副本
//SptCopymap_OnPlayerLeave( %playerId );
//
//// 副本开启成功
//SptCopymap_OpenAck( %CopyMapInstId, %playerId );
//// 副本开启失败
//SptCopymap_OpenAckFail( %CopyMapInstId, %playerId );
//
////--------------------------------------------------------------------------------
//// 示例
//


function SptCopymap_OpenAck( %CopyMapInstId, %PlayerID, %MapID )
{
	echo( "玩家[" @ %playerId @ "]开启副本[" @ %CopyMapInstId @ "]" );
	
//	%Pos = "";
//	
//	if(%MapId == 1302 ){%Pos = "-290.566 -230.297 14.3938";}	//传送到测试闹鬼的渔村（夜晚）
//	if(%MapId == 1303 ){%Pos = "23.1431 -14.3424 127.587";}	//传送至女娲试炼
//  if(%MapId == 1304 ){%Pos = "61.7801 -15.7809 76.6993";}	//仙游岭传送至水墨古镇（七日前）
//  if(%MapId == 1305 ){%Pos = "30.3023 -2.77872 126.125";}	//无忧草野传送至试炼迷境
//		
//  if(%Pos !$= "")
//		GoToNextMap_CopyMap( %PlayerID, %MapID );
}

//副本成功开启触发
function SvrCopymapOpen(%CopymapID, %MapID)
{
	echo("副本成功开启触发 = SvrCopymapOpen = " @ %CopymapID @" = "@ %MapID);
	
	%MapID = GetSubStr(%MapID,0,4);
	
	//创建副本怪物与NPC
		//万书楼
		if(%MapID == 1301)
			{
				SpNewNpc(0, 410900001, "4.63545 5.18364 133.91", %CopyMapID);
				SpNewNpc(0, 410900001, "-5.25908 -1.78902 133.91", %CopyMapID);
				SpNewNpc(0, 410900001, "-14.5403 11.1485 133.351", %CopyMapID);
				SpNewNpc(0, 410900001, "13.7722 10.586 133.351", %CopyMapID);
				SpNewNpc(0, 410900001, "-0.571578 -14.914 133.351", %CopyMapID);
				SpNewNpc(0, 410900001, "5.32464 -1.48335 133.91", %CopyMapID);
				SpNewNpc(0, 410900001, "-5.23395 4.89165 133.91", %CopyMapID);
				SpNewNpc(0, 410900001, "-5.23395 4.89165 133.91", %CopyMapID);
				SpNewNpc(0, 410900001, "0.0160513 18.6729 133.351", %CopyMapID);
				SpNewNpc(0, 410900001, "-14.7964 -6.35835 133.351", %CopyMapID);
				SpNewNpc3(0, 410900002, "11.5799 22.0287 133.352", %CopyMapID,"206.838","2 2 2");
			}

	 if(%MapID == 1302)
		 {
				//闹鬼的渔村（夜晚）
				SpNewNpc(0, 401302001, "-292.116 -226.02 14.7002", %CopyMapID);
				SpNewNpc(0, 410500062, "-249.949 -302.59 11.7549", %CopyMapID);
				SpNewNpc(0, 410500062, "-233.954 -296.305 11.5469", %CopyMapID);
				SpNewNpc(0, 410500062, "-240.661 -285.35 12.2351", %CopyMapID);
				SpNewNpc(0, 410500062, "-239.449 -317.456 11.365", %CopyMapID);
				SpNewNpc(0, 410500062, "-269.663 -304.087 12.0316", %CopyMapID);
				SpNewNpc(0, 410500062, "-246.238 -290.802 12.427", %CopyMapID);
				SpNewNpc(0, 410500062, "-291.92 -303.809 12.2447", %CopyMapID);
				SpNewNpc(0, 410500062, "-279.111 -311.168 12.0111", %CopyMapID);
				SpNewNpc(0, 410500062, "-286.86 -284.984 12.0111", %CopyMapID);
				SpNewNpc(0, 410500063, "-290.225 -314.828 12.1824", %CopyMapID);
				SpNewNpc(0, 410500063, "-304.818 -301.602 12.2028", %CopyMapID);
				SpNewNpc(0, 410500063, "-238.435 -328.096 11.7878", %CopyMapID);
				SpNewNpc(0, 410500063, "-239.808 -304.153 11.3503", %CopyMapID);
				SpNewNpc(0, 410500063, "-256.67 -299.138 11.9486", %CopyMapID);
				SpNewNpc(0, 410500063, "-217.3 -289.306 13.2668", %CopyMapID);
				SpNewNpc(0, 410500063, "-302.634 -316.896 15.0909", %CopyMapID);
				SpNewNpc(0, 410500063, "-297.49 -289.06 12.0111", %CopyMapID);
				SpNewNpc(0, 410500063, "-284.326 -298.478 12.1891", %CopyMapID);
				SpNewNpc(0, 410500063, "-251.19 -294.887 12.0111", %CopyMapID);

		}

	if(%MapID == 1303)
		{
			//女娲试炼区域-女娲神像体内
			SpNewNpc3(0, 401303101, "11.5799 22.0287 133.352", %CopyMapID,"206.838","2 2 2");
			SpNewNpc(0, 410700022, "4.63545 5.18364 133.91", %CopyMapID);
			SpNewNpc(0, 410700022, "-5.25908 -1.78902 133.91", %CopyMapID);
			SpNewNpc(0, 410700022, "-14.5403 11.1485 133.351", %CopyMapID);
			SpNewNpc(0, 410700022, "13.7722 10.586 133.351", %CopyMapID);
			SpNewNpc(0, 410700022, "-0.571578 -14.914 133.351", %CopyMapID);
			SpNewNpc(0, 410700022, "5.32464 -1.48335 133.91", %CopyMapID);
			SpNewNpc(0, 410700022, "-5.23395 4.89165 133.91", %CopyMapID);
			SpNewNpc(0, 410700022, "0.0160513 18.6729 133.351", %CopyMapID);
			SpNewNpc(0, 410700022, "14.9223 -6.17085 133.351", %CopyMapID);
			SpNewNpc(0, 410700022, "-14.7964 -6.35835 133.351", %CopyMapID);
	 }

	if(%MapID == 1304)
		{
			//古镇（七日前）
			SpNewNpc(0, 401304001, "59.8842 -7.23678 72.654", %CopyMapID);
			SpNewNpc(0, 401304002, "-1.93044 6.90355 72.6704", %CopyMapID);
			SpNewNpc(0, 411101009, "-2.31191 62.8413 72.3553", %CopyMapID);
			SpNewNpc(0, 411101009, "-17.3515 61.9189 72.2671", %CopyMapID);
			SpNewNpc(0, 411101009, "-5.95891 45.4058 71.9899", %CopyMapID);
			SpNewNpc(0, 411101009, "13.0716 47.9764 71.9643", %CopyMapID);
			SpNewNpc(0, 411101009, "-4.81668 31.6214 73.1723", %CopyMapID);
			SpNewNpc(0, 411101009, "9.07418 35.0263 73.1993", %CopyMapID);
			SpNewNpc(0, 411101009, "24.7555 64.6258 73.0722", %CopyMapID);
			SpNewNpc(0, 411101009, "56.2905 53.0159 72.0526", %CopyMapID);
			SpNewNpc(0, 411101009, "45.4509 61.4736 72.0307", %CopyMapID);
			SpNewNpc(0, 411101009, "93.0689 -19.413 73.451", %CopyMapID);
			SpNewNpc(0, 411101009, "100.617 -5.06695 73.8024", %CopyMapID);
			SpNewNpc(0, 411101009, "91.0004 21.5491 72.4825", %CopyMapID);
			SpNewNpc(0, 411101009, "105.922 -25.1777 73.2965", %CopyMapID);
			SpNewNpc(0, 411101009, "73.9947 41.7098 72.0156", %CopyMapID);
			SpNewNpc(0, 410800002, "-11.0933 55.2168 72.3326", %CopyMapID);
			SpNewNpc(0, 410800002, "63.4094 49.0313 72.5189", %CopyMapID);
			SpNewNpc(0, 410800002, "16.693 11.8888 72.6511", %CopyMapID);
			SpNewNpc(0, 410800002, "55.4435 5.12636 72.6531", %CopyMapID);
			SpNewNpc(0, 410800002, "100.549 -25.7892 73.2909", %CopyMapID);
			SpNewNpc(0, 410800002, "96.0988 8.9311 72.0879", %CopyMapID);
			SpNewNpc(0, 410800002, "1.76105 34.8433 72.2215", %CopyMapID);
			SpNewNpc(0, 410800002, "36.8337 56.1438 71.9577", %CopyMapID);
			SpNewNpc(0, 410800002, "21.0168 47.2983 72.0202", %CopyMapID);
			SpNewNpc(0, 410800002, "62.2342 18.9784 72.8318", %CopyMapID);
			SpNewNpc(0, 410800002, "40.5555 19.528 72.6573", %CopyMapID);
			SpNewNpc(0, 410701001, "32.0771 62.4388 71.9965", %CopyMapID);
			SpNewNpc(0, 410701001, "37.6169 60.7929 71.897", %CopyMapID);
			SpNewNpc(0, 410701001, "9.97809 51.984 71.9479", %CopyMapID);
			SpNewNpc(0, 410701001, "-22.7702 54.7809 72.5253", %CopyMapID);
			SpNewNpc(0, 410701001, "10.7787 63.6248 73.362", %CopyMapID);
			SpNewNpc(0, 410701001, "91.5884 -10.0679 72.8406", %CopyMapID);
			SpNewNpc(0, 410701001, "85.481 14.6978 71.9986", %CopyMapID);
			SpNewNpc(0, 410701001, "52.2233 53.933 71.9518", %CopyMapID);
			SpNewNpc3(0, 410701002, "35.4641 63.772 71.8882", %CopyMapID,0,"2 2 2");
		}
	if(%MapID == 1305)
		{
			//贪狼神域
			SpNewNpc3(0, 401305001, "5.77906 -10.4772 111.028", %CopyMapID,0,"2 2 2");
		}
}

