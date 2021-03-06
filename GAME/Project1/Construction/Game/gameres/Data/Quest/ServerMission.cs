//==================================================================================
//服务端所有的脚本入口，只存脚本加载路径的添加，每添加一个脚本，均需注明脚本的大致内容
//
//
//本处加载的脚本，所有地图服务器均会载入
//==================================================================================


//==================================================================================
echo("开始载入脚本ServerMission");

//加载脚本标识，标明当前为服务端脚本
$Now_Script = 2;

//加载客户端与服务端共享的函数集-包括共享的脚本文件
Exec("./C_S/C_S_Function.cs");

//加载任务脚本
Exec("./Server/S_Mission_Normal.cs");
Exec("./Server/S_Mission_Special.cs");

//加载脚本函数集
Exec("./Server/S_Function.cs");

//加载物品触发脚本
Exec("./Server/S_Trigger.cs");

//加载NPC功能性脚本
Exec("./Server/S_Npc_Job.cs");

//加载副本功能脚本
Exec("./Server/S_CopyMap.cs");

//根据地图编号加载相应地图脚本
//Exec("./Server/Z1001.cs");
//Exec("./Server/Z1009.cs");
//Exec("./Server/Z" @ GetZoneID() @ ".cs");
//Exec("./Server/1001/Z1001.cs");
//地图脚本名称前加Z，是为了方便排序时，将地图相关的脚本排列在最底层

echo("结束载入脚本ServerMission");
//=============================================================================



