#ifndef _LOGTYPES_H_
#define _LOGTYPES_H_

#include "../dbStruct.h"

/************************************************************************/
/* �������е�LOG��Ϣ�ṹ
/************************************************************************/

//// Database Struct Sample
//DB_STRUCT( lgPlayerData )
//{
//	DECL_DB_TABLE( lgPlayerData, TB_PLAYERDATA);
//
//	DECL_DB_FIELD( lgPlayerData, mId,		int,	DB_TYPE_INDEX,	����,		32	);
//};

/************************************************************************/
/* �ʺŵ���
/************************************************************************/
DB_STRUCT(lg_account_login)
{
    DECL_DB_TABLE(lg_account_login,"�ʺŵ���");

    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,30);      //�ʺ�����
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,30);      //IP��ַ
    DECL_DB_FIELD(area,       int,         DB_TYPE_INT,   10);      //��������
    DECL_DB_FIELD(result,     std::string, DB_TYPE_STRING,10);      //������
    //DECL_DB_FIELD(result2,     std::string, DB_TYPE_STRING,30);      //������
    DECL_DB_FIELD(result3,     int, DB_TYPE_INT,14);                //������
};

/************************************************************************/
/* �ʺŵǳ�
/************************************************************************/
DB_STRUCT(lg_account_logout)
{
    DECL_DB_TABLE(lg_account_logout,"�ʺŵǳ�");

    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,30);      //�ʺ�����
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,30);      //IP��ַ
    DECL_DB_FIELD(area,       int,         DB_TYPE_INT,   10);      //��������
};

//��ɫ����
DB_STRUCT(lg_char_login)
{
    DECL_DB_TABLE(lg_char_login,"��ɫ����");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //��ɫ����
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //�ʺ�����
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,32);      //IP��ַ
    DECL_DB_FIELD(mapId,      int,         DB_TYPE_INT,   10);      //��¼��ͼ
};

//��ɫ�ǳ�
DB_STRUCT(lg_char_logout)
{
    DECL_DB_TABLE(lg_char_logout,"��ɫ�ǳ�");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //��ɫ����
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //�ʺ�����
    DECL_DB_FIELD(ip,         std::string, DB_TYPE_STRING,32);      //IP��ַ
    DECL_DB_FIELD(mapId,      int,         DB_TYPE_INT,   10);      //�ǳ���ͼ
};

/************************************************************************/
/* ���ﴴ��
/************************************************************************/
DB_STRUCT(lg_char_create)
{
    DECL_DB_TABLE(lg_char_create,"���ﴴ��");
    
    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //��ɫ����
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //�ʺ�����
    DECL_DB_FIELD(playerId,   int,         DB_TYPE_INT,   32);      //��ɫID
    DECL_DB_FIELD(job,        int,         DB_TYPE_INT,   8);       //ְҵ
    DECL_DB_FIELD(level,      int,         DB_TYPE_INT,   8);       //�ȼ�
    DECL_DB_FIELD(sex,        int,         DB_TYPE_INT,   8);       //�Ա�
    DECL_DB_FIELD(race,       int,         DB_TYPE_INT,   8);       //����
};

/************************************************************************/
/* ����ɾ��
/************************************************************************/
DB_STRUCT(lg_char_delete)
{
    DECL_DB_TABLE(lg_char_delete,"����ɾ��");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,32);      //��ɫ����
    DECL_DB_FIELD(accountName,std::string, DB_TYPE_STRING,32);      //�ʺ�����
    DECL_DB_FIELD(playerId,   int,         DB_TYPE_INT,   32);      //��ɫID
    DECL_DB_FIELD(job,        int,         DB_TYPE_INT,   8);       //ְҵ
    DECL_DB_FIELD(level,      int,         DB_TYPE_INT,   8);       //�ȼ�
    DECL_DB_FIELD(sex,        int,         DB_TYPE_INT,   8);       //�Ա�
    DECL_DB_FIELD(race,       int,         DB_TYPE_INT,   8);       //����
};

/************************************************************************/
/* ����ѧϰ
/************************************************************************/
DB_STRUCT(lg_skill_learn)
{
    DECL_DB_TABLE(lg_skill_learn,"����ѧϰ");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //���ĵĽ�Ǯ
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //���ĵľ���
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
};

/************************************************************************/
/* �����ѧϰ
/************************************************************************/
DB_STRUCT(lg_livingskill_learn)
{
    DECL_DB_TABLE(lg_livingskill_learn,"�����ѧϰ");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //���ĵĽ�Ǯ
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //���ĵľ���
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
};

/************************************************************************/
/* ��������
/************************************************************************/
DB_STRUCT(lg_skill_update)
{
    DECL_DB_TABLE(lg_skill_update,"��������");

    DECL_DB_FIELD(skillName, std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(level,     int,         DB_TYPE_INT,8);          //�ȼ�
    DECL_DB_FIELD(costGold,  int,         DB_TYPE_INT,32);         //���ĵĽ�Ǯ
    DECL_DB_FIELD(costExp,   int,         DB_TYPE_INT,32);         //���ĵľ���
    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
};

/************************************************************************/
/* ��Ʒ����
/************************************************************************/
DB_STRUCT(lg_shopping_buy)
{
    DECL_DB_TABLE(lg_shopping_buy,"���߹���");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
    DECL_DB_FIELD(buyfrom,   std::string, DB_TYPE_STRING,30);      //��Դ(�̵�,���̵�ع�)
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //��������
    DECL_DB_FIELD(costType,  int,         DB_TYPE_STRING,30);      //��������(���,Ԫ��)
    DECL_DB_FIELD(costVal,   int,         DB_TYPE_INT,   32);      //����ֵ
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //����X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //����Y
};

DB_STRUCT(lg_shopping_rebuy)
{
    DECL_DB_TABLE(lg_shopping_rebuy,"���߻ع�");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
    DECL_DB_FIELD(buyfrom,   std::string, DB_TYPE_STRING,30);      //��Դ(�̵�,���̵�ع�)
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //��������
    DECL_DB_FIELD(costType,  int,         DB_TYPE_STRING,30);      //��������(���,Ԫ��)
    DECL_DB_FIELD(costVal,   int,         DB_TYPE_INT,   32);      //����ֵ
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //����X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //����Y
};

/************************************************************************/
/* ��Ʒ����
/************************************************************************/
DB_STRUCT(lg_shopping_sell)
{
    DECL_DB_TABLE(lg_shopping_sell,"���߳���");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(itemUID,   U64,         DB_TYPE_BIGINT,32);      //����UID
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //��������
    DECL_DB_FIELD(itemPrice, int,         DB_TYPE_INT,   8);       //�۸�
};

/************************************************************************/
/* ʰȡ����
/************************************************************************/
DB_STRUCT(lg_pickup_item)
{
    DECL_DB_TABLE(lg_pickup_item,"����ʰȡ");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,30);      //��ɫ����
    DECL_DB_FIELD(monsterName,std::string,DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(itemUID,   U64,         DB_TYPE_BIGINT,32);      //����UID
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);      //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);       //��������
    DECL_DB_FIELD(map,       int,         DB_TYPE_INT,   32);      //MAPID
    DECL_DB_FIELD(posX,      float,       DB_TYPE_FLOAT, 32);      //����X
    DECL_DB_FIELD(posY,      float,       DB_TYPE_FLOAT, 32);      //����Y
};

/************************************************************************/
/* ������Ʒ
/************************************************************************/
DB_STRUCT(lg_drop_item)
{
    DECL_DB_TABLE(lg_drop_item,"��Ʒ����");

    DECL_DB_FIELD(playerName,std::string, DB_TYPE_STRING,  30);      //��ɫ����
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,30);        //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,   8);         //��������
};

/************************************************************************/
/* ��Ʒ���׵õ���
/************************************************************************/
DB_STRUCT(lg_trade_itemGet)
{
    DECL_DB_TABLE(lg_trade_itemGet,"���׻����Ʒ");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //�Է����׵��������
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //��ǰ�������
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* ��Ʒ���׳�ȥ��
/************************************************************************/
DB_STRUCT(lg_trade_itemLost)
{
    DECL_DB_TABLE(lg_trade_itemLost,"����ʧȥ��Ʒ");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //��ǰ�������
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //��ȡ��Ʒ���������
    DECL_DB_FIELD(itemName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount, int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* ���׵õ��Ľ�Ǯ 
/************************************************************************/
DB_STRUCT(lg_trade_goldGet)
{
    DECL_DB_TABLE(lg_trade_goldGet,"���׻�ý�Ǯ");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //�Է����׵��������
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //��ȡ��ҵ��������
    DECL_DB_FIELD(gold,      int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* ����ʧȥ�Ľ�Ǯ
/************************************************************************/
DB_STRUCT(lg_trade_goldLost)
{   
    DECL_DB_TABLE(lg_trade_goldLost,"����ʧȥ��Ǯ");

    DECL_DB_FIELD(fromPlayer,std::string, DB_TYPE_STRING,  30);      //��ǰ�������
    DECL_DB_FIELD(toPlayer,  std::string, DB_TYPE_STRING,  30);      //��ȡ��ҵ��������
    DECL_DB_FIELD(gold,      int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* �ʼ�
/************************************************************************/
DB_STRUCT(lg_mail)
{
    DECL_DB_TABLE(lg_mail,"�ʼ�");

    DECL_DB_FIELD(sendName, std::string, DB_TYPE_STRING,  30);      //������
    DECL_DB_FIELD(recvName, std::string, DB_TYPE_STRING,  30);      //������
    DECL_DB_FIELD(itemName, std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount,int,         DB_TYPE_INT,     8);       //��������
    DECL_DB_FIELD(gold,     int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* �ʼĻ�ȡ��Ʒ
/************************************************************************/
DB_STRUCT(lg_mail_itemGet)
{
    DECL_DB_TABLE(lg_mail_itemGet,"�ʼ������Ʒ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(sendName,   std::string, DB_TYPE_STRING,  30);      //������
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);        //��������
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);         //��������
};

/************************************************************************/
/* �ʼĻ�ý�Ǯ
/************************************************************************/
DB_STRUCT(lg_mail_goldGet)
{
    DECL_DB_TABLE(lg_mail_goldGet,"�ʼ���ý�Ǯ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(sendName,   std::string, DB_TYPE_STRING,  30);      //������
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* �����д�����Ʒ
/************************************************************************/
DB_STRUCT(lg_bank_saveItem)
{
    DECL_DB_TABLE(lg_bank_saveItem,"���д���");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* ����ȡ����Ʒ
/************************************************************************/
DB_STRUCT(lg_bank_getItem)
{
    DECL_DB_TABLE(lg_bank_getItem,"����ȡ��");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* ���д�Ǯ
/************************************************************************/
DB_STRUCT(lg_bank_saveGold)
{
    DECL_DB_TABLE(lg_bank_saveGold,"���д�Ǯ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* ����ȡǮ
/************************************************************************/
DB_STRUCT(lg_bank_getGold)
{
    DECL_DB_TABLE(lg_bank_getGold,"����ȡǮ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //���
};


/************************************************************************/
/* ����������
/************************************************************************/
DB_STRUCT(lg_quest_finishInfo)
{
    DECL_DB_TABLE(lg_quest_finishInfo,"����������");

    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(playerLevel,int,         DB_TYPE_INT,     32);      //��ҵȼ�
    DECL_DB_FIELD(playerRace, int,         DB_TYPE_INT,     32);      //���ְҵ
};

/************************************************************************/
/* �����õ���Ʒ
/************************************************************************/
DB_STRUCT(lg_quest_itemGet)
{
    DECL_DB_TABLE(lg_quest_itemGet,"������Ʒ��ȡ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* �����ý��
/************************************************************************/
DB_STRUCT(lg_quest_goldGet)
{
    DECL_DB_TABLE(lg_quest_goldGet,"�����һ�ȡ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* �����þ���
/************************************************************************/
DB_STRUCT(lg_quest_expGet)
{
    DECL_DB_TABLE(lg_quest_expGet,"�������ȡ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(questName,  std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(exp,        int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* ���������Ϣ
/************************************************************************/
DB_STRUCT(lg_death)
{
    DECL_DB_TABLE(lg_death,"������Ϣ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(killerName, std::string, DB_TYPE_STRING,  30);      //ɱ����ҵ��˻��������
};




/************************************************************************/
/* ������ʧ����Ʒ
/************************************************************************/
DB_STRUCT(lg_death_itemLost)
{
    DECL_DB_TABLE(lg_death_itemLost,"��������");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(itemCount,  int,         DB_TYPE_INT,     8);       //��������
};

/************************************************************************/
/* ����ʱ��ʧ�Ľ�Ǯ
/************************************************************************/
DB_STRUCT(lg_death_goldLost)
{
    DECL_DB_TABLE(lg_death_goldLost,"������Ǯ");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(gold,       int,         DB_TYPE_INT,     32);      //���
};

/************************************************************************/
/* ����ʹ��
/************************************************************************/
DB_STRUCT(lg_item_use)
{
    DECL_DB_TABLE(lg_item_use,"����ʹ��");

    DECL_DB_FIELD(playerName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(itemName,   std::string, DB_TYPE_STRING,  30);      //��������
    DECL_DB_FIELD(useName,    std::string, DB_TYPE_STRING,  30);      //��;
};


/************************************************************************/
/* ��ֻ�õľ���
/************************************************************************/
DB_STRUCT(lg_monster_exp)
{
    DECL_DB_TABLE(lg_monster_exp,"ɱ�־���");

    DECL_DB_FIELD(playerName,  std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(playerLevel, int,         DB_TYPE_INT,     32);      //��ҵȼ�
    DECL_DB_FIELD(playerRace,  int,         DB_TYPE_INT,     32);      //���ְҵ
    DECL_DB_FIELD(monsterName, std::string, DB_TYPE_STRING,  30);      //��ǰ���
    DECL_DB_FIELD(exp,         int,         DB_TYPE_DATETIME,32);      //exp
};

#endif /*_LOGTYPES_H_*/