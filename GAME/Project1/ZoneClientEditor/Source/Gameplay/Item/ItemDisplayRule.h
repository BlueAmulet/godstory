#pragma once

/*
U32 nItemDisplayID[] = 
{
	420000001,
	420000002,
	420000003,
	420000004,
	420000005,
	420000005,
	420000006,
	420000007,
	420000008,
	420000009,
	4200000010,
	4200000011,
	4200000012,
	4200000013,
	4200000014,
	4200000015,
	4200000016,
	4200000017,
	4200000018,
	4200000019,
	4200000020,
	4200000021,
	4200000022,
	4200000023,
	4200000024,
	4200000025,
	4200000026,
	4200000027,
};
*/



U32 getItemDispID(U32 nItemID)
{
	U32 nDisIDRet = 0;
	U32 nPrev4 = nItemID / 10000;

	switch(nPrev4)
	{
	case 10101:
		nDisIDRet = 420000001;
		break;

	case 10102:
	case 10103:
	case 10104:
	case 10105:
	case 10106:
	case 10107:
	case 10108:
		nDisIDRet = 420000002;
		break;
	
	case 10201:
		nDisIDRet = 420000003;
		break;

	case 10202:
		nDisIDRet = 420000004;
		break;

	case 10203:
	case 10204:
	case 10205:
	case 10206:
	case 10207:
	case 10208:
		nDisIDRet = 420000003;
		break;

	case 10209:
	case 10210:
	case 10211:
		nDisIDRet = 420000004;
		break;

	case 10301:
	case 10302:
	case 10303:
		nDisIDRet = 420000005;
		break;

	case 10401:
		nDisIDRet = 420000006;
		break;

	case 10402:
	case 10403:
		nDisIDRet = 42000007;
		break;

	case 10404:
		nDisIDRet = 42000008;
		break;

	case 10501:
		nDisIDRet = 42000009;
		break;

	case 10502:
	case 10503:
	case 10504:
		nDisIDRet = 42000010;
		break;

	case 10505:
	case 10506:
		nDisIDRet = 42000014;
		break;

	case 10507:
	case 10508:
		nDisIDRet = 42000011;
		break;

	case 10509:
		nDisIDRet = 42000012;
		break;

	case 10510:
		nDisIDRet = 42000013;
		break;

	case 10601:
	case 10602:
	case 10603:
	case 10604:
	case 10605:
	case 10606:
	case 10607:
	case 10608:
	case 10609:
		nDisIDRet = 42000014;
		break;

	case 10701:
	case 10702:
	case 10703:
	case 10704:
	case 10705:
	case 10706:
	case 10707:
	case 10708:
	case 10709:
		nDisIDRet = 42000015;
		break;

	case 10801:
	case 10802:
		nDisIDRet = 42000016;
		break;

	case 10901:
	case 10902:
	case 10903:
	case 10904:
	case 10905:
	case 10906:
	case 10907:
		nDisIDRet = 42000017;
		break;

	case 11001:
	case 11002:
	case 11003:
	case 11004:
	case 11005:
	case 11006:
	case 11007:
	case 11008:
	case 11009:
	case 11010:
	case 11011:
	case 11012:
	case 11013:
	case 11014:
	case 11015:
		nDisIDRet = 42000018;
		break;

	case 11101:
	case 11102:
	case 11103:
	case 11104:
	case 11105:
	case 11106:
	case 11107:
	case 11108:
	case 11109:
	case 11110:
	case 11111:
	case 11112:
	case 11113:
	case 11114:
	case 11115:
		nDisIDRet = 42000019;
		break;

	case 11201:
	case 11202:
		nDisIDRet = 42000020;
		break;

	case 11301:
	case 11302:
	case 11303:
	case 11304:
		nDisIDRet = 42000021;
		break;
	
	case 11401:
		nDisIDRet = 42000022;
		break;

	case 11402:
		nDisIDRet = 42000023;
		break;

	case 11403:
	case 11404:
		nDisIDRet = 42000024;
		break;

	case 11405:
		nDisIDRet = 42000025;
		break;

	case 11406:
		nDisIDRet = 42000026;
		break;

	case 11407:
		nDisIDRet = 42000021;
		break;

	default:
		break;
	}

	return nDisIDRet;
}