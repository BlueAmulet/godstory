
#include "Encrypt.h"
/*
������Կ����
type Ϊ�㷨����
KeyValue Ϊ��Կ�ַ���
*/
bool CEncrypt::MakeKey(EncryptEnum type, const char* KeyValue)
{
	if(type>=Encrypt_MaxNum)
		return false;
	m_type = type;		

	if(!KeyValue || !KeyValue[0])
		return false;

	int nlen = (int)strlen(KeyValue);

	if(type== Encrypt_XOR256)		//��Ϊ���̫�򵥣�������ԿΪ2048bit
	{
		memset(XORKey,0,256);
		memcpy(XORKey,KeyValue,nlen);
	}
	else							//��Կ����Ϊ128bit
	{		
		if( nlen>BlockSize || nlen<=0)
			return false;
		memset(PublicKey, 0,BlockSize);
		memcpy(PublicKey, KeyValue,nlen);
	}
	return true;
}

//��Ϊ������ʹ��IBM's Mars�㷨������Release�汾��������󣬲�֪Ϊ��?������ʱ�����Ż�
#pragma optimize("", off)

/*
���ܷ���
InString Ϊ�������ַ���
OutString Ϊ�������ɵ��ַ���
BufferLen Ϊ�ַ�������
*/
bool CEncrypt::Encrypt(char* InString, char* OutString, int BufferLen)
{
	try
	{
		//���2048λ��ģʽ�����㷨
		if(m_type == Encrypt_XOR256)
		{
			//��ʼ��OutString
			memset(OutString,0,BufferLen);

			CXOR256 oXOR256(XORKey,256);			
			oXOR256.Crypt(InString,OutString,BufferLen);
			return true;
		}

		//BlowFish�����㷨
		if(m_type == Encrypt_BlowFish_ECB || m_type==Encrypt_BlowFish_CBC || m_type==Encrypt_BlowFish_CFB)
		{			
			//���Դ�ַ�������С��8��ֱ�Ӽ����
			if(BufferLen<8)
			{
				memcpy(OutString,InString,BufferLen);
				char* pB = OutString;
				for (int i=0; i<BufferLen; i++)
					*pB++ ^= 0x5b;
				return true;
			}

			int iType = 0;
			if(m_type == Encrypt_BlowFish_ECB)
				iType = CBlowFish::ECB;
			else if (m_type == Encrypt_BlowFish_CBC)
				iType = CBlowFish::CBC;
			else
				iType = CBlowFish::CFB;
			
			//����������ַ������Ȳ�Ϊ8�ı���
			if(BufferLen%8!=0)
			{
				memcpy(OutString,InString,BufferLen);

				int nLen = BufferLen - BufferLen%8 + 1;

				char* srcStr = new char[nLen];
				char* objStr = new char[nLen];
				memset(objStr,0,nLen);

				for(int i=0 ;i<nLen;i++)
					srcStr[i] = InString != 0 ? *InString++ : 0;

				CBlowFish oBlowFish((unsigned char*)PublicKey, BlockSize);
				oBlowFish.Encrypt((unsigned char*)srcStr,(unsigned char*)objStr,nLen-1,iType);

				memcpy(OutString,objStr,nLen-1);

				delete[] srcStr;
				delete[] objStr;
			}
			else
			{
				//��ʼ��OutString
				memset(OutString,0,BufferLen);

				CBlowFish oBlowFish((unsigned char*)PublicKey, BlockSize);
				oBlowFish.Encrypt((unsigned char*)InString,(unsigned char*)OutString,BufferLen,iType);
			}
			return true;
		}

		//AES�����㷨
		if(m_type == Encrypt_AES_ECB || m_type==Encrypt_AES_CBC || m_type==Encrypt_AES_CFB)
		{
			//���Դ�ַ�������С��BlockSize��ֱ�Ӽ����
			if(BufferLen<BlockSize)
			{
				memcpy(OutString,InString,BufferLen);
				char* pB = OutString;
				for (int i=0; i<BufferLen; i++)
					*pB++ ^= 0x6a;
				return true;
			}

			int iType = 0;
			if(m_type == Encrypt_AES_ECB)
				iType = CRijndael::ECB;
			else if (m_type == Encrypt_AES_CBC)
				iType = CRijndael::CBC;
			else
				iType = CRijndael::CFB;
			
			//����������ַ������Ȳ�ΪBlockSize�ı���
			if(BufferLen%BlockSize!=0)
			{
				memcpy(OutString,InString,BufferLen);

				int nLen = BufferLen - BufferLen%BlockSize + 1;				

				char* srcStr = new char[nLen];
				char* objStr = new char[nLen];
				memset(objStr,0,nLen);

				for(int i=0 ;i<nLen;i++)
					srcStr[i] = InString != 0 ? *InString++ : 0;

				CRijndael oRijndael;
				oRijndael.MakeKey(PublicKey, CRijndael::sm_chain0, BlockSize, BlockSize);
				oRijndael.Encrypt(srcStr,objStr,nLen-1,iType);

				memcpy(OutString,objStr,nLen-1);

				delete[] srcStr;
				delete[] objStr;
			}
			else
			{
				//��ʼ��OutString
				memset(OutString,0,BufferLen);

				CRijndael oRijndael;
				oRijndael.MakeKey(PublicKey, CRijndael::sm_chain0, BlockSize, BlockSize);
				oRijndael.Encrypt(InString,OutString,BufferLen,iType);
			}
			return true;
		}

		//IBM's MARS�㷨
		//ע�⣺��Ϊ������õ�Mars���ܷ�����
		//ֻ�ܲ����ַ�������Ϊ16�ַ��������ԶԳ��ַ�������ֶ�ѭ������
		if(m_type==Encrypt_MARS)
		{
			//��ʼ��OutString
			memset(OutString,0,BufferLen);

			mars Mars;
			Mars.set_key((unsigned char*)PublicKey,16);			//������Կ
			
			int iLen = BufferLen%16==0? BufferLen:BufferLen + 16 - BufferLen%16;
			char* Instr = new char[iLen];
			memset(Instr,0,iLen);
			memcpy(Instr,InString,BufferLen);

			int iSection = iLen / 16;
			for(int i = 0; i < iSection; i++)
			{	
				unsigned char in_block[16],out_block[16];			
				memset(in_block,0,16);
				memset(out_block,0,16);
				memcpy(in_block,Instr + 16 * i,16);
				Mars.encrypt(in_block,out_block);
				if(i != iSection -1)
					memcpy(OutString + 16 * i,out_block,16);
				else
					memcpy(OutString + 16 * i,out_block,iLen==BufferLen?16:iLen-BufferLen);
			}

			delete[] Instr;
			return true;
		}

		//Serpent�㷨
		//ע�⣺��Ϊ������õ�Serpent���ܷ�����
		//ֻ�ܲ����ַ�������Ϊ16�ַ��������ԶԳ��ַ�������ֶ�ѭ������
		if(m_type==Encrypt_SERPENT)
		{
			//��ʼ��OutString
			memset(OutString,0,BufferLen);

			serpent Serpent;
			Serpent.set_key((unsigned long*)PublicKey,16);			//������Կ

			int iLen = BufferLen%16==0? BufferLen:BufferLen + 16 - BufferLen%16;
			char* Instr = new char[iLen];
			memset(Instr,0,iLen);
			memcpy(Instr,InString,BufferLen);

			int iSection = iLen / 16;
			for(int i = 0; i < iSection; i++)
			{	
				unsigned long in_block[4],out_block[4];			
				memset(in_block,0,16);
				memset(out_block,0,16);
				memcpy(in_block,Instr + 16 * i,16);
				Serpent.encrypt(in_block,out_block);
				if(i != iSection -1)
					memcpy(OutString + 16 * i,out_block,16);
				else
					memcpy(OutString + 16 * i,out_block,iLen==BufferLen?16:iLen-BufferLen);
			}

			delete[] Instr;
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}

/*
���ܷ���
InString Ϊ�������ַ���
OutString Ϊ�������ɵ��ַ���
BufferLen Ϊ�ַ�������
*/
bool CEncrypt::Decrypt(char* InString, char* OutString, int BufferLen)
{
	try
	{
		//���2048λ��ģʽ�����㷨
		if(m_type == Encrypt_XOR256)
		{
			//��ʼ��OutString
			memset(OutString,0,BufferLen);

			CXOR256 oXOR256(XORKey,256);			
			oXOR256.Crypt(InString,OutString,BufferLen);
			return true;
		}

		//BlowFish�����㷨
		if(m_type == Encrypt_BlowFish_ECB || m_type==Encrypt_BlowFish_CBC || m_type==Encrypt_BlowFish_CFB)
		{

			//���Դ�ַ�������С��8��ֱ�Ӽ����
			if(BufferLen<8)
			{
				memcpy(OutString,InString,BufferLen);
				char* pB = OutString;
				for (int i=0; i<BufferLen; i++)
					*pB++ ^= 0x5b;
				return true;
			}

			int iType = 0;
			if(m_type == Encrypt_BlowFish_ECB)
				iType = CBlowFish::ECB;
			else if (m_type == Encrypt_BlowFish_CBC)
				iType = CBlowFish::CBC;
			else
				iType = CBlowFish::CFB;
			
			//����������ַ������Ȳ�Ϊ8�ı���
			if(BufferLen%8!=0)
			{
				//��ʼ��OutString
				memcpy(OutString,InString,BufferLen);

				int nLen = BufferLen - BufferLen%8 + 1;				

				char* srcStr = new char[nLen];
				char* objStr = new char[nLen];
				memset(objStr,0,nLen);

				for(int i=0 ;i<nLen;i++)
					srcStr[i] = InString != 0 ? *InString++ : 0;

				CBlowFish oBlowFish((unsigned char*)PublicKey, BlockSize);
				oBlowFish.Decrypt((unsigned char*)srcStr,(unsigned char*)objStr,nLen-1,iType);

				memcpy(OutString,objStr,nLen-1);

				delete[] srcStr;
				delete[] objStr;
			}
			else
			{
				//��ʼ��OutString
				memset(OutString,0,BufferLen);

				CBlowFish oBlowFish((unsigned char*)PublicKey, BlockSize);
				oBlowFish.Decrypt((unsigned char*)InString,(unsigned char*)OutString,BufferLen,iType);
			}
			return true;
		}

		//AES�����㷨
		if(m_type == Encrypt_AES_ECB || m_type==Encrypt_AES_CBC || m_type==Encrypt_AES_CFB)
		{
			
			//���Դ�ַ�������С��BlockSize��ֱ�Ӽ����
			if(BufferLen<BlockSize)
			{
				memcpy(OutString,InString,BufferLen);
				char* pB = OutString;
				for (int i=0; i<BufferLen; i++)
					*pB++ ^= 0x6a;
				return true;
			}

			int iType = 0;
			if(m_type == Encrypt_AES_ECB)
				iType = CRijndael::ECB;
			else if (m_type == Encrypt_AES_CBC)
				iType = CRijndael::CBC;
			else
				iType = CRijndael::CFB;
			
			//����������ַ������Ȳ�ΪBlockSize�ı���
			if(BufferLen%BlockSize!=0)
			{
				memcpy(OutString,InString,BufferLen);

				int nLen = BufferLen - BufferLen%BlockSize + 1;				

				char* srcStr = new char[nLen];
				char* objStr = new char[nLen];
				memset(objStr,0,nLen);

				for(int i=0 ;i<nLen;i++)
					srcStr[i] = InString != 0 ? *InString++ : 0;

				CRijndael oRijndael;
				oRijndael.MakeKey(PublicKey, CRijndael::sm_chain0, BlockSize, BlockSize);
				oRijndael.Decrypt(srcStr,objStr,nLen-1,iType);

				memcpy(OutString,objStr,nLen-1);

				delete[] srcStr;
				delete[] objStr;
			}
			else
			{
				//��ʼ��OutString
				memset(OutString,0,BufferLen);

				CRijndael oRijndael;
				oRijndael.MakeKey(PublicKey, CRijndael::sm_chain0, BlockSize, BlockSize);
				oRijndael.Decrypt(InString,OutString,BufferLen,iType);
			}
			return true;
		}

		//IBM's MARS�㷨
		//ע�⣺��Ϊ������õ�Mars���ܷ�����
		//ֻ�ܲ����ַ�������Ϊ16�ַ��������ԶԳ��ַ�������ֶ�ѭ������
		if(m_type==Encrypt_MARS)
		{
			mars Mars;
			Mars.set_key((unsigned char*)PublicKey,16);			//������Կ
			
			//��ʼ��OutString
			memset(OutString,0,BufferLen);
			
			int iLen = BufferLen%16==0? BufferLen:BufferLen + 16 - BufferLen%16;
			char* Instr = new char[iLen];
			memset(Instr,0,iLen);
			memcpy(Instr,InString,BufferLen);

			int iSection = iLen / 16;
			
			for(int i= 0; i < iSection; i++)
			{
				unsigned char in_block[16],out_block[16];
				memset(in_block,0,16);
				memset(out_block,0,16);
				memcpy(in_block,Instr + 16 * i,16);
				Mars.decrypt(in_block,out_block);
				if(i != iSection -1)
					memcpy(OutString + 16 * i,out_block,16);
				else
					memcpy(OutString + 16 * i,out_block,iLen==BufferLen?16:iLen-BufferLen);
			}

			delete[] Instr;
			return true;
		}

		//Serpent�㷨
		//ע�⣺��Ϊ������õ�Serpent���ܷ�����
		//ֻ�ܲ����ַ�������Ϊ16�ַ��������ԶԳ��ַ�������ֶ�ѭ������
		if(m_type==Encrypt_SERPENT)
		{
			//��ʼ��OutString
			memset(OutString,0,BufferLen);

			serpent Serpent;
			Serpent.set_key((unsigned long*)PublicKey,16);			//������Կ

			int iLen = BufferLen%16==0? BufferLen:BufferLen + 16 - BufferLen%16;
			char* Instr = new char[iLen];
			memset(Instr,0,iLen);
			memcpy(Instr,InString,BufferLen);

			int iSection = iLen / 16;
			for(int i = 0; i < iSection; i++)
			{	
				unsigned long in_block[4],out_block[4];			
				memset(in_block,0,16);
				memset(out_block,0,16);
				memcpy(in_block,Instr + 16 * i,16);
				Serpent.decrypt(in_block,out_block);
				if(i != iSection -1)
					memcpy(OutString + 16 * i,out_block,16);
				else
					memcpy(OutString + 16 * i,out_block,iLen==BufferLen?16:iLen-BufferLen);
			}

			delete[] Instr;
			return true;
		}
	}
	catch(...)
	{
		return false;
	}
	return false;
}
#pragma optimize("", on)