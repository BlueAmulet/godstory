//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#ifndef _GAMEBASE_H_
#ifndef SXZPARTSYS
#include "T3D/gameBase.h"
#else
#include "game/gameBase.h"
#endif
#endif

#include "gfx/gfxTextureHandle.h"
struct IDirect3DVolumeTexture9;
#define MaxParticleSize 50.0
#define DELS(p)		                                   { if(p) { delete[] (p); (p) = NULL; } }
#define DEL(p)		                                   { if(p) { delete (p); (p) = NULL; } }
#define SAFE_RELEASE(p)										\
	{															\
	if( (p) != NULL )										\
		{														\
		(p)->Release();										\
		(p) = NULL;											\
		}														\
	}

class Ribbon : public SimDataBlock
{
	typedef SimDataBlock Parent;

#ifdef SXZPARTSYS
    friend class SxzRibbon;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

private:
	GFXTexHandle                                    m_TextureHandle;                     //�������
	StringTableEntry                                m_TextureName;

	Vector<F32>                                     m_vecSize;
	Vector<F32>                                     m_vecSizeRand;                      
	Vector<F32>                                     m_vecSizeTimePoint;                 
	Vector<Point3F>                                 m_vecEula;                          
	Vector<Point3F>                                 m_vecEulaRand;                       
	Vector<F32>                                     m_vecEulaTimePoint;  
	Vector<ColorF>                                  m_vecColor; 
	Vector<ColorF>                                  m_vecColorRand;                 
	Vector<F32>                                     m_vecColorTimePoint;    
	Vector<F32>                                     m_vecAlphaPoint;                     //ʱ���Alpha��ɫ���켣��n��n����0.0��1.0��
	Vector<F32>                                     m_vecAlphaTimePoint;                 //Alphaʱ��㣨�켣��n��n����������
	F32                                             m_fGlow;                             //�����
	bool                                            m_bRepeat;                           //�Ƿ��ظ�
	F32                                             m_fDisplayVelocity;                  //��ʾ����
	Vector<F32>                                     m_vecAnimationTimePoint;             //����ʱ���
	Vector<Point3F>                                 m_vecAnimationPosition;              //�ؼ�֡λ��
	bool                                            m_bFollow;                           //�Ƿ����
	S32                                             m_iRibbonNum;                        
	S32                                             m_iTextureStageState;                //���״̬��1-4��������
	F32                                             m_fEmitLife; 
	Point3F                                         m_vScale;                            //����
	Point3F                                         m_vSelfIllumination;                 //�Է�����ɫ
	F32                                             m_fPowValue;
public:
    Ribbon();
    ~Ribbon();

	/**
	@brief		��ȡʱ����ϵ�˿���ߴ��С

	@return		˿���ߴ��С
	*/  
	inline      bool             GetSizePointStep(S32 pos, F32* step)
	{
		if(m_vecSize.size() > pos)
		{
			*step = m_vecSize[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿��ŷ���ߴ��С�����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetSizeRand(S32 pos, F32* point)
	{
		if (m_vecSizeRand.size() > pos)
		{
			*point = m_vecSizeRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿���ߴ��Сʱ���

	@return		ʱ���
	*/  
	inline      bool              GetSizeTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeTimePoint.size() > pos)
		{
			*point = m_vecSizeTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�ŷ���Ƕ�ƫ��

	@return		ŷ���Ƕ�ƫ��
	*/  
	inline      bool               GetRandomEula(S32 pos, Point3F* point)
	{
		if(m_vecEula.size() > pos)
		{
			*point = m_vecEula[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿��ŷ���Ƕ�ƫ�Ƶ����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetEulaRand(S32 pos, Point3F* point)
	{
		if (m_vecEulaRand.size() > pos)
		{
			*point = m_vecEulaRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿��ŷ���Ƕ�ƫ�Ƶ�ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetEulaTimePoint(S32 pos, F32* point)
	{
		if (m_vecEulaTimePoint.size() > pos)
		{
			*point = m_vecEulaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�˿����ɫ

	@return		��ɫ
	*/  
	inline      bool               GetColorPointStep(S32 pos, ColorF* step)
	{
		if(m_vecColor.size() > pos)
		{
			*step = m_vecColor[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿����ɫ�����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetColorRand(S32 pos, ColorF* point)
	{
		if (m_vecColorRand.size() > pos)
		{
			*point = m_vecColorRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ˿����ɫ��ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetColorTimePoint(S32 pos, F32* point)
	{
		if(m_vecColorTimePoint.size() > pos)
		{
			*point = m_vecColorTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�������ɫ

	@return		��ɫ
	*/  
	inline      bool               GetAlphaPoint(S32 pos, F32* point)
	{
		if(m_vecAlphaPoint.size() > pos)
		{
			*point = m_vecAlphaPoint[pos];
			*point = (*point > 1.0f) ? 1.0f :(*point);
			*point = (*point < 0.0f) ? 0.0f :(*point);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ������ɫ��ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetAlphaTimePoint(S32 pos, F32* point)
	{
		if(m_vecAlphaTimePoint.size() > pos)
		{
			*point = m_vecAlphaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����

	@return		����
	*/  
	inline      GFXTexHandle         GetTexture()
	{
		return m_TextureHandle;
	}

	/**
	@brief		��ȡ�Ƿ��ظ�

	@return		�Ƿ��ظ�
	*/  
	inline      bool                 GetRepeat()
	{
#       ifdef SXZPARTSYS
            return false;
#       endif
		return m_bRepeat;
	}

	/**
	@brief		�����Ƿ����

	@return		��
	*/  
	inline      void                 SetFollow(bool data)
	{
		m_bFollow = data;
	}

	/**
	@brief		��ȡ�Ƿ����

	@return		�Ƿ����
	*/  
	inline      bool                 GetFollow()
	{
		return m_bFollow;
	}

	/**
	@brief		��ȡ���״̬

	@return		���״̬
	*/  
	inline      S32                 GetTextureStageState()
	{
		return m_iTextureStageState;
	}

	/**
	@brief		���ñ����

	@return		��
	*/  
	inline      void                SetGlow(F32 data)
	{
		m_fGlow = data;
	}

	/**
	@brief		��ȡ�����

	@return		�����
	*/  
	inline      F32                 GetGlow()
	{
		return m_fGlow;
	}

	/**
	@brief		��ȡ�Է�����ɫ

	@return		�Է�����ɫ
	*/  
	inline      Point3F             GetSelfIllumination()
	{
		return m_vSelfIllumination;
	}

	/**
	@brief		��ȡ�ع�ָ��

	@return		�ع�ָ��
	*/  
	inline      F32                 GetPowValue()
	{
		return m_fPowValue;
	}

	/**
	@brief		������ʾ����

	@return		��
	*/  
	inline      void                SetDisplayVelocity(float velocity)
	{
		m_fDisplayVelocity = velocity;
	}

	/**
	@brief		��ȡ��ʾ����

	@return		��ʾ����
	*/  
	inline      float               GetDisplayVelocity()
	{
		return m_fDisplayVelocity;
	}

	/**
	@brief		��ȡ����λ��

	@return		�Ƿ�ɹ�
	*/  
	bool                            GetAnimationPosition(F32 time, Point3F* pos);

	/**
	@brief		����˿����ϸ��

	@return		��
	*/  
	inline      void                SetRibbonNum(S32 num)
	{
		m_iRibbonNum = num;
	}

	/**
	@brief		��ȡ˿����ϸ��

	@return		˿����ϸ��
	*/  
	inline      S32                  GetRibbonNum()
	{
		return m_iRibbonNum;
	}

	/**
	@brief		���÷���������ʱ��

	@return		��
	*/  
	inline      void                SetEmitLife(F32 life)
	{
		m_fEmitLife = life;
	}

	/**
	@brief		��ȡ����������ʱ��

	@return		����������ʱ��
	*/  
	inline      F32                 GetEmitLife()
	{
		return m_fEmitLife;
	}

	/**
	@brief		��������ֵ

	@return		��
	*/  
	inline      void                 SetScale(Point3F data)
	{
		m_vScale = data;
	}

	/**
	@brief		��ȡ����ֵ

	@return		����ֵ
	*/  
	inline      Point3F              GetScale()
	{
		return m_vScale;
	}

	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	bool preload(bool server, char errorBuffer[256]);
	DECLARE_CONOBJECT(Ribbon);
	static void  initPersistFields();

	bool reload();
protected:
};

struct Particle;

class ParticleData : public SimDataBlock
{
	typedef SimDataBlock Parent;

#ifdef SXZPARTSYS
    friend class SxzParticle;
    friend class SxzExport;
#endif

public:
    MRandomLCG mRandGen;

private:
	GFXTexHandle                                    m_TextureHandle;                     //�������
	IDirect3DVolumeTexture9*                        m_pTexture;
	StringTableEntry                                m_TextureName;

	F32                                             m_fLifetime;                         //�����ڣ�ȫ�֣��������������
	F32                                             m_fLifetimeRand;                     //������������ȫ�֣��������������
	Vector<F32>                                     m_vecSizeXRand;                      //���Ӵ�С�ߴ�ֵ�������켣��n��n����������
	Vector<F32>                                     m_vecSizeXPoint;                     //���Ӵ�С�ߴ�ֵ���켣��n��n����������
	Vector<F32>                                     m_vecSizeXTimePoint;                 //���Ӵ�С�ߴ�ֵʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_vecSizeYRand;                      //���Ӵ�С�ߴ�ֵ�������켣��n��n����������
	Vector<F32>                                     m_vecSizeYPoint;                     //���Ӵ�С�ߴ�ֵ���켣��n��n����������
	Vector<F32>                                     m_vecSizeYTimePoint;                 //���Ӵ�С�ߴ�ֵʱ��㣨�켣��n��n����������
	Vector<Point3F>                                 m_vecEula;                           //ʱ����ϵ�ŷ���Ƕȣ��ٶȷ��򣨹켣��n��n�����������У�
	Vector<Point3F>                                 m_vecEulaRand;                       //ʱ����ϵ�ŷ���Ƕ�������Ӱ���ٶȷ��򣨹켣��n��n����������
	Vector<F32>                                     m_vecEulaTimePoint;                  //ŷ���Ƕ�ƫ��ʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_vecVelocityPoint;                  //�����ٶ�ֵ���켣��n��n�����������У�
	Vector<F32>                                     m_vecVelocityRand;                   //�����ٶ�ֵƫ�������켣��n��n����������
	Vector<F32>                                     m_vecVelocityTimePoint;              //�����ٶ�ʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_vecRepulseVelocityPoint;           //���������ٶ�ֵ���켣��n��n�����������У�
	Vector<F32>                                     m_vecRepulseVelocityRand;            //���������ٶ�ֵƫ�������켣��n��n����������
	Vector<F32>                                     m_vecRepulseVelocityTimePoint;       //���������ٶ�ʱ��㣨�켣��n��n����������
	Vector<ColorF>                                  m_vecColorPointRand;                 //ʱ�����ɫֵ�������켣��n��n����0.0��1.0��
	Vector<ColorF>                                  m_vecColorPoint;                     //ʱ�����ɫֵ���켣��n��n����0.0��1.0��
	Vector<F32>                                     m_vecColorTimePoint;                 //��ɫʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_vecAlphaPoint;                     //ʱ���Alpha��ɫ���켣��n��n����0.0��1.0��
	Vector<F32>                                     m_vecAlphaTimePoint;                 //Alphaʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_vecAnglePoint;                     //��ת�Ƕ�ƫ�ƣ��켣��n��n�����������У�
	Vector<F32>                                     m_vecAnglePointRand;
	Vector<F32>                                     m_vecAngleTimePoint;                 //��ת�Ƕ�ƫ��ʱ��㣨�켣��n��n����������
	Vector<F32>                                     m_fGravity;                          //����ֵ��ȫ�֣����������У�
	Vector<F32>                                     m_fGravityRand;                      //����ֵ����
	Vector<F32>                                     m_vecGravityTimePoint;               //����ʱ���
	Vector<F32>                                     m_vecEmitTimeIntervalTimePoint;      //����ʱ����ʱ���     
	Vector<F32>                                     m_vecEmitTimeIntervalRand;           //����ʱ��������
	Vector<F32>                                     m_vecEmitTimeInterval;               //����ʱ������ȫ�֣���������
	Vector<Point3F>                                 m_vecPosDeviation;                   //λ��ƫ�ƣ�ȫ�֣���������
	Vector<F32>                                     m_vecPosDeviationTimePoint;          //λ��ƫ��ʱ���
	Vector<F32>                                     m_vecEmitStateTimePoint;             //����ʱ���          
	Vector<bool>                                    m_vecEmitState;                      //����״̬
	S32                                             m_iPerEmitNum;                       //ÿ�η��������������ȫ�֣���������  
	bool                                            m_bBillboard;                        //�Ƿ���ù���ƣ�ȫ�֣�
	Vector<F32>                                     m_vecRotTimePoint;                   //��תʱ��㣨�켣��n��n����������
	Vector<Point3F>                                 m_vecRotValue;                       //��תֵ���켣��n��n�����������У�
	Vector<Point3F>                                 m_vecRotValueRand;                   //��תֵ�������켣��n��n�����������У�
	bool                                            m_bTextureSplinter;                  //�Ƿ������ͼ�ֿ飨ȫ�֣�
	bool                                            m_bTexSplinterBroadRandom;           //�Ƿ������ͼ�ֿ�������ţ�ȫ�֣�
	S32                                             m_iTextureSplinterStageNumLength;    //��ͼ�ֿ������ȫ�֣���������
	S32                                             m_iTextureSplinterStageNumWidth;     //��ͼ�ֿ������ȫ�֣���������
	S32                                             m_iTextureStageState;                //���״̬��ȫ�֣���1-4��������
	S32                                             m_dwMaxParticleNum;                  //�ܵ��������������ȫ�֣���������
	F32                                             m_fEmitLife;    

	F32                                             m_fGlow;                             //�����
	F32                                             m_fTexRemainTime;                    //������ͼ�ֿ鲥�ŵ�ʱ�䣨ȫ�֣���������
	F32                                             m_fFlickerInterval;                  //������˸ʱ������ȫ�֣���������
	F32                                             m_fFlickerIntervalRand;
	F32                                             m_fFlickerTime;                      //������˸ʱ�� ��ȫ�֣���������
	F32                                             m_fFlickerTimeRand;
	S32                                             m_iParticleRot;                      //������ת����ȫ�֣���0��1��2��
	bool                                            m_bRepeat;                           //�Ƿ��ظ����䣨ȫ�֣�
	F32                                             m_fDisplayVelocity;                  //��ʾ���ʣ�ȫ�֣�
	Vector<F32>                                     m_vecAnimationTimePoint;             //���ӷ���������ʱ���
	Vector<Point3F>                                 m_vecAnimationPosition;              //���ӷ������ؼ�֡λ��
	bool                                            m_bFollow;                           //�Ƿ����
	bool                                            m_bHalfCut;                          //�Ƿ��߽ض�
	Vector<Point3F>                                 m_vecScalePoint;                     //����
    Vector<F32>                                     m_vecScaleTimePoint;                 //����ʱ���
	bool                                            m_bHaveTail;                         //�Ƿ������β
	bool                                            m_bEmitByRadius;                     //�Ƿ����淢��
	F32                                             m_fEmitRadius;                       //���淢��뾶
	F32                                             m_fEmitRadiusOffset;                 //���淢��뾶ƫ��
	bool                                            m_bModelEmit;                        //ģ�ͷ���
	Vector<Point3F>                                 m_vecModelVertex;                    //����ģ�Ͷ�������
    Vector<Point3F>                                 m_vecModelNormal;                    //����ģ�ͷ�������(���־����˶�����)
	Point3F                                         m_vSelfIllumination;                 //�Է�����ɫ
    F32                                             m_fPowValue;

	Point2F*                                        m_fTextureCoordUV1;                  //��ͼ�ֿ�UV��ͼ����*���Զ����� ���ɱ༭��
	Point2F*                                        m_fTextureCoordUV2;                  //��ͼ�ֿ�UV��ͼ����*���Զ����� ���ɱ༭��
	Point2F*                                        m_fTextureCoordUV3;                  //��ͼ�ֿ�UV��ͼ����*���Զ����� ���ɱ༭��
	Point2F*                                        m_fTextureCoordUV4;                  //��ͼ�ֿ�UV��ͼ����*���Զ����� ���ɱ༭��
	S32                                             m_iTextureSplinterNum;               //��ͼ�ֿ����������Զ����� ���ɱ༭��
    bool                                            m_bSizeRandomBothXAndY;

	//����Ϊ������Ч����ռ����
	bool                                            m_bElectricity;
	F32                                             m_fGlowStrength;
	F32                                             m_fHeight;
	F32                                             m_fGlowFallOff;
	F32                                             m_fSpeed;
	F32                                             m_fSampleDist;
	F32                                             m_fAmbientGlow;
	F32                                             m_fAmbientGlowHeightScale;
	F32                                             m_fVertNoise;
public:
	ParticleData();
	~ParticleData();
	enum PDConst
	{
		PDC_NUM_KEYS = 4,
	};
	enum PARTICLEROT
	{
		PARTICLEROT_CW = 0,
		PARTICLEROT_CCW,
		PARTICLEROT_RANDOM,
	};
	/**
	@brief		�����Ƿ�������Ч��

	@return		��
	*/  
	inline      void             SetElectricityState(bool data)
	{
		m_bElectricity = data;
		if(m_bElectricity)
			m_bTextureSplinter = false;
	}

	/**
	@brief		��ȡ�Ƿ�������Ч��

	@return		�Ƿ�������Ч��
	*/  
	inline      bool             GetElectricityState()
	{
		return m_bElectricity;
	}

	/**
	@brief		��������

	@return		��
	*/  
	inline      void             SetGlowStrength(float data)
	{
		m_fGlowStrength = data;
	}

	/**
	@brief		��ȡ����

	@return		����
	*/  
	inline      float            GetGlowStrength()
	{
		return m_fGlowStrength;
	}

	/**
	@brief		���ÿ��

	@return		��
	*/  
	inline      void             SetHeight(float data)
	{
		m_fHeight = data;
	}

	/**
	@brief		��ȡ���

	@return		���
	*/  
	inline      float            GetHeight()
	{
		return m_fHeight;
	}

	/**
	@brief		����˥��ֵ

	@return		��
	*/  
	inline      void             SetGlowFallOff(float data)
	{
		m_fGlowFallOff = data;
	}

	/**
	@brief		��ȡ˥��ֵ

	@return		˥��ֵ
	*/  
	inline      float            GetGlowFallOff()
	{
		return m_fGlowFallOff;
	}

	/**
	@brief		���ñ仯�ٶ�

	@return		��
	*/  
	inline      void             SetSpeed(float data)
	{
		m_fSpeed = data;
	}

	/**
	@brief		��ȡ�仯�ٶ�

	@return		�仯�ٶ�
	*/  
	inline      float            GetSpeed()
	{
		return m_fSpeed;
	}

	/**
	@brief		���ø���ֵ

	@return		��
	*/  
	inline      void             SetSampleDist(float data)
	{
		m_fSampleDist = data;
	}

	/**
	@brief		��ȡ����ֵ

	@return		����ֵ
	*/  
	inline      float            GetSampleDist()
	{
		return m_fSampleDist;
	}

	/**
	@brief		��������

	@return		��
	*/  
	inline      void             SetAmbientGlow(float data)
	{
		m_fAmbientGlow = data;
	}

	/**
	@brief		��ȡ����

	@return		����
	*/  
	inline      float            GetAmbientGlow()
	{
		return m_fAmbientGlow;
	}

	/**
	@brief		������������ֵ

	@return		��
	*/  
	inline      void             SetAmbientGlowHeightScale(float data)
	{
		m_fAmbientGlowHeightScale = data;
	}

	/**
	@brief		��ȡ��������ֵ

	@return		��������ֵ
	*/  
	inline      float            GetAmbientGlowHeightScale()
	{
		return m_fAmbientGlowHeightScale;
	}

	/**
	@brief		���ö������ֵ

	@return		��
	*/  
	inline      void             SetVertNoise(float data)
	{
		m_fVertNoise = data;
	}

	/**
	@brief		��ȡ�������ֵ

	@return		�������ֵ
	*/  
	inline      float            GetVertNoise()
	{
		return m_fVertNoise;
	}

	/**
	@brief		��ȡ��������ʱ��

	@return		��������ʱ��
	*/  
	inline      F32              GetLifetime()
	{
		return (m_fLifetime + mRandGen.randF(-m_fLifetimeRand, m_fLifetimeRand));
	}

	/**
	@brief		��ȡʱ����ϵ����ӳߴ��С

	@return		���ӳߴ��С
	*/  
	inline      bool             GetSizePointXStep(S32 pos, F32* step)
	{
		if(m_vecSizeXPoint.size() > pos)
		{
			*step = (m_vecSizeXPoint[pos] + mRandGen.randF(-m_vecSizeXRand[pos], m_vecSizeXRand[pos]));
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ����ӳߴ��С

	@return		���ӳߴ��С
	*/  
	inline      bool             GetSizePointX(S32 pos, F32* step)
	{
		if(m_vecSizeXPoint.size() > pos)
		{
			*step = (m_vecSizeXPoint[pos]);
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���ӳߴ��Сʱ���

	@return		ʱ���
	*/  
	inline      bool              GetSizeXTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeXTimePoint.size() > pos)
		{
			*point = m_vecSizeXTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ����ӳߴ��С

	@return		���ӳߴ��С
	*/  
	inline      bool             GetSizePointYStep(S32 pos, F32* step)
	{
		if(m_vecSizeYPoint.size() > pos)
		{
			*step = (m_vecSizeYPoint[pos] + mRandGen.randF(-m_vecSizeYRand[pos], m_vecSizeYRand[pos]));
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ����ӳߴ��С

	@return		���ӳߴ��С
	*/  
	inline      bool             GetSizePointY(S32 pos, F32* step)
	{
		if(m_vecSizeYPoint.size() > pos)
		{
			*step = (m_vecSizeYPoint[pos]);
			*step = (*step < 0.0f) ? 0.0f : (*step);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���ӳߴ��Сʱ���

	@return		ʱ���
	*/  
	inline      bool              GetSizeYTimePoint(S32 pos, F32* point)
	{
		if (m_vecSizeYTimePoint.size() > pos)
		{
			*point = m_vecSizeYTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�ŷ���Ƕ�ƫ��

	@return		ŷ���Ƕ�ƫ��
	*/  
	inline      bool               GetRandomEula(S32 pos, Point3F* point)
	{
		if(m_vecEula.size() > pos)
		{
			*point = m_vecEula[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����ŷ���Ƕ�ƫ�Ƶ����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetEulaRand(S32 pos, Point3F* point)
	{
		if (m_vecEulaRand.size() > pos)
		{
			*point = m_vecEulaRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����ŷ���Ƕ�ƫ�Ƶ�ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetEulaTimePoint(S32 pos, F32* point)
	{
		if (m_vecEulaTimePoint.size() > pos)
		{
			*point = m_vecEulaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ��ٶ�

	@return		�ٶ�
	*/  
	inline      bool               GetRandomVelocity(S32 pos, F32* point)
	{
		if(m_vecVelocityPoint.size() > pos)
		{
			*point = m_vecVelocityPoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ�����ٶ�ƫ�Ƶ����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetVelocityRand(S32 pos, F32* point)
	{
		if (m_vecVelocityRand.size() > pos)
		{
			*point = m_vecVelocityRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ�����ٶ�ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetVelocityTimePoint(S32 pos, F32* point)
	{
		if (m_vecVelocityTimePoint.size() > pos)
		{
			*point = m_vecVelocityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ������ٶ�

	@return		�ٶ�
	*/  
	inline      bool               GetRandomRepulseVelocity(S32 pos, F32* point)
	{
		if(m_vecRepulseVelocityPoint.size() > pos)
		{
			*point = m_vecRepulseVelocityPoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���������ٶ�ƫ�Ƶ����ֵ

	@return		���ֵ
	*/  
	inline      bool               GetRepulseVelocityRand(S32 pos, F32* point)
	{
		if (m_vecRepulseVelocityRand.size() > pos)
		{
			*point = m_vecRepulseVelocityRand[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���������ٶ�ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetRepulseVelocityTimePoint(S32 pos, F32* point)
	{
		if (m_vecRepulseVelocityTimePoint.size() > pos)
		{
			*point = m_vecRepulseVelocityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�������ɫ

	@return		��ɫ
	*/  
	inline      bool               GetColorPointStep(S32 pos, ColorF* step)
	{
		if(m_vecColorPoint.size() > pos)
		{
			*step = (m_vecColorPoint[pos] + mRandGen.randC(-m_vecColorPointRand[pos], m_vecColorPointRand[pos]));
			step->clamp();
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ������ɫ��ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetColorTimePoint(S32 pos, F32* point)
	{
		if(m_vecColorTimePoint.size() > pos)
		{
			*point = m_vecColorTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�������ɫ

	@return		��ɫ
	*/  
	inline      bool               GetAlphaPoint(S32 pos, F32* point)
	{
		if(m_vecAlphaPoint.size() > pos)
		{
			*point = m_vecAlphaPoint[pos];
			*point = (*point > 1.0f) ? 1.0f :(*point);
			*point = (*point < 0.0f) ? 0.0f :(*point);
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ������ɫ��ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetAlphaTimePoint(S32 pos, F32* point)
	{
		if(m_vecAlphaTimePoint.size() > pos)
		{
			*point = m_vecAlphaTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡʱ����ϵ�������ת�Ƕ�

	@return		�Ƕ�ƫ����
	*/  
	inline      bool               GetAnglePointStep(S32 pos, F32* step)
	{
		if(m_vecAnglePoint.size() > pos)
		{
			*step = (m_vecAnglePoint[pos] + mRandGen.randF(-m_vecAnglePointRand[pos], m_vecAnglePointRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ������ת�Ƕȵ�ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetAngleTimePoint(S32 pos, F32* point)
	{
		if(m_vecAngleTimePoint.size() > pos)
		{
			*point = m_vecAngleTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����ֵ

	@return		����ֵ
	*/  
	inline      bool               GetGravityStep(S32 pos, F32* step)
	{
		if(m_fGravity.size() > pos)
		{
			*step = (m_fGravity[pos] + mRandGen.randF(-m_fGravityRand[pos], m_fGravityRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����������ʱ���

	@return		ʱ���
	*/  
	inline      bool               GetGravityTimePoint(S32 pos, F32* point)
	{
		if(m_vecGravityTimePoint.size() > pos)
		{
			*point = m_vecGravityTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����ʱ����

	@return		ʱ����
	*/  
	inline      bool               GetRandomEmitTimeInterval(S32 pos, F32* step)
	{
		if(m_vecEmitTimeInterval.size() > pos)
		{
			*step = (m_vecEmitTimeInterval[pos] + mRandGen.randF(-m_vecEmitTimeIntervalRand[pos], m_vecEmitTimeIntervalRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���ӷ���ʱ������ʱ���

	@return		ʱ���
	*/  
	inline      bool                GetEmitTimeIntervalTimePoint(S32 pos, F32* point)
	{
		if(m_vecEmitTimeIntervalTimePoint.size() > pos)
		{
			*point = m_vecEmitTimeIntervalTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ��ʼλ��ƫ����

	@return		��ʼλ��ƫ����
	*/  
	inline      bool                GetRandomPosDeviation(S32 pos, Point3F* point)
	{
		if(m_vecPosDeviation.size() > pos)
		{
			*point = m_vecPosDeviation[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���ӳ�ʼλ��ƫ������ʱ���

	@return		ʱ���
	*/  
	inline      bool                 GetPosDeviationTimePoint(S32 pos, F32* point)
	{
		if(m_vecPosDeviationTimePoint.size() > pos)
		{
			*point = m_vecPosDeviationTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����

	@return		����
	*/  
	inline      GFXTexHandle         GetTexture()
	{
		return m_TextureHandle;
	}

	/**
	@brief		��ȡ����

	@return		����
	*/  
	inline      IDirect3DVolumeTexture9* GetVolumeTexture()
	{
		return m_pTexture;
	}

	/**
	@brief		��ȡ����״̬

	@return		����״̬
	*/  
	inline      bool                 GetEmitState(S32 pos, bool* point)
	{
		if(m_vecEmitState.size() > pos)
		{
			*point = m_vecEmitState[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����״̬��ʱ���

	@return		ʱ���
	*/  
	inline      bool                 GetEmitStateTimePoint(S32 pos, F32* point)
	{
		if(m_vecEmitStateTimePoint.size() > pos)
		{
			*point = m_vecEmitStateTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡÿ�η������������

	@return		ÿ�η������������
	*/  
	inline      S32                  GetPerEmitNum()
	{
		return m_iPerEmitNum;
	}

	/**
	@brief		��ȡ�Ƿ���ù���ƻ���

	@return		�Ƿ���ù���ƻ���
	*/  
	inline      bool                 GetBillboardState()
	{
		return m_bBillboard;
	}

	/**
	@brief		��ȡ����X����תֵ��ʱ���

	@return		ʱ���
	*/  
	inline      bool                 GetRotDeviationTimePoint(S32 pos, F32* point)
	{
		if(m_vecRotTimePoint.size() > pos)
		{
			*point = m_vecRotTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ������ʱ����ϵ���תֵ

	@return		��תֵ
	*/  
	inline      bool                 GetRotDeviationStep(S32 pos, Point3F* step)
	{
		if(m_vecRotValue.size() > pos)
		{
			*step = (m_vecRotValue[pos] + mRandGen.randP3(-m_vecRotValueRand[pos], m_vecRotValueRand[pos]));
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ�Ƿ������ͼ�ֿ�

	@return		�Ƿ������ͼ�ֿ�
	*/  
	inline      bool                 GetTextureSplinterState()
	{
		return m_bTextureSplinter;
	}

	/**
	@brief		��ȡ��ͼ�ֿ�����

	@return		��ͼ�ֿ�����
	*/  
	inline      S32                  GetTextureSplinterNum()
	{
		return m_iTextureSplinterNum;
	}

	/**
	@brief		��ȡ��ͼ�ֿ����

	@return		��ͼ�ֿ����
	*/  
	inline      S32                  GetTextureSplinterStageNumLength()
	{
        return m_iTextureSplinterStageNumLength;
	}

	/**
	@brief		��ȡ��ͼ�ֿ����

	@return		��ͼ�ֿ����
	*/  
	inline      S32                  GetTextureSplinterStageNumWidth()
	{
		return m_iTextureSplinterStageNumWidth;
	}

	/**
	@brief		��ȡ��ͼ�ֿ���ͼ����

	@return		��
	*/  
	inline      void                 GetTextureSplinterCoord(S32 block, Point2F* uv1, Point2F* uv2, Point2F* uv3, Point2F* uv4)
	{
		if (block < m_iTextureSplinterNum)
		{
			*uv1 = m_fTextureCoordUV1[block];
			*uv2 = m_fTextureCoordUV2[block];
			*uv3 = m_fTextureCoordUV3[block];
			*uv4 = m_fTextureCoordUV4[block];
		}
	}

	/**
	@brief		��ȡ�Ƿ������ͼ�ֿ��������

	@return		�Ƿ������ͼ�ֿ��������
	*/  
	inline      bool                 GetTexSplinterBroadRandom()
	{
		return m_bTexSplinterBroadRandom;
	}

	/**
	@brief		��ȡ������ͼ�ֿ鲥�ŵ�ʱ��

	@return		������ͼ�ֿ鲥�ŵ�ʱ��
	*/  
	inline      F32                  GetTexRemainTime()
	{
		return m_fTexRemainTime;
	}

	/**
	@brief		ȡ���ܵ������������

	@return		�����������
	*/ 
	inline      S32                  GetMaxParticleNum()
	{
		return m_dwMaxParticleNum;
	}

	/**
	@brief		�����ܵ������������

	@return		��
	*/ 
	inline      void                 SetMaxParticleNum(S32 num)
	{
		m_dwMaxParticleNum =  num;
	}

	/**
	@brief		��ȡ������˸ʱ����

	@return		������˸ʱ����
	*/  
	inline      F32                  GetRandomFlickerInterval()
	{
		return (m_fFlickerInterval + mRandGen.randF(-m_fFlickerIntervalRand, m_fFlickerIntervalRand));
	}

	/**
	@brief		��ȡ������˸ʱ��

	@return		������˸ʱ��
	*/  
	inline      F32                  GetRandomFlickerTime()
	{
		return (m_fFlickerTime + mRandGen.randF(-m_fFlickerTimeRand, m_fFlickerTimeRand));
	}

	/**
	@brief		��ȡ������ת����

	@return		������ת����
	*/  
	inline      S32                  GetParticleRot()
	{
		return m_iParticleRot;
	}

	/**
	@brief		��ȡ�Ƿ��ظ�

	@return		�Ƿ��ظ�
	*/  
	inline      bool                 GetRepeat()
	{
#       ifdef SXZPARTSYS
            return false;
#       endif
		return m_bRepeat;
	}

	/**
	@brief		�����Ƿ����

	@return		��
	*/  
	inline      void                 SetFollow(bool data)
	{
		m_bFollow = data;
	}

	/**
	@brief		��ȡ�Ƿ����

	@return		�Ƿ����
	*/  
	inline      bool                 GetFollow()
	{
		return m_bFollow;
	}

	/**
	@brief		�����Ƿ��߽ض�

	@return		��
	*/  
	inline      void                 SetHalfCut(bool data)
	{
		m_bHalfCut = data;
	}

	/**
	@brief		��ȡ�Ƿ��߽ض�

	@return		�Ƿ��߽ض�
	*/  
	inline      bool                 GetHalfCut()
	{
		return m_bHalfCut;
	}

	/**
	@brief		�����Ƿ������β

	@return		��
	*/  
	inline      void                 SetHaveTail(bool data)
	{
		m_bHaveTail = data;
	}

	/**
	@brief		��ȡ�Ƿ������β

	@return		�Ƿ������β
	*/  
	inline      bool                 GetHaveTail()
	{
		return m_bHaveTail;
	}

	/**
	@brief		���÷���뾶

	@return		��
	*/  
	inline      void                 SetEmitRadius(F32 data)
	{
		m_fEmitRadius = data;
	}

	/**
	@brief		��ȡ����뾶

	@return		����뾶
	*/  
	inline      F32                  GetEmitRadius()
	{
		return m_fEmitRadius;
	}

	/**
	@brief		���÷���뾶ƫ��

	@return		��
	*/  
	inline      void                 SetEmitRadiusOffset(F32 data)
	{
		m_fEmitRadiusOffset = data;
	}

	/**
	@brief		��ȡ����뾶ƫ��

	@return		����뾶ƫ��
	*/  
	inline      F32                  GetEmitRadiusOffset()
	{
		return m_fEmitRadiusOffset;
	}

	/**
	@brief		�����Ƿ����淢��

	@return		��
	*/  
	inline      void                 SetEmitByRadius(bool data)
	{
		m_bEmitByRadius = data;
	}

	/**
	@brief		��ȡ�Ƿ����淢��

	@return		�Ƿ����淢��
	*/  
	inline      bool                 GetEmitByRadius()
	{
		return m_bEmitByRadius;
	}

	/**
	@brief		�����Ƿ����淢��

	@return		��
	*/  
	inline      void                 SetSizeRandomBothXAndY(bool data)
	{
		m_bSizeRandomBothXAndY = data;
	}

	/**
	@brief		��ȡ�Ƿ����淢��

	@return		�Ƿ����淢��
	*/  
	inline      bool                 GetSizeRandomBothXAndY()
	{
		return m_bSizeRandomBothXAndY;
	}

	/**
	@brief		�����Ƿ�ģ�ͷ���

	@return		��
	*/  
	inline      void                 SetModelEmit(bool data)
	{
		m_bModelEmit = data;
	}

	/**
	@brief		��ȡ�Ƿ�ģ�ͷ���

	@return		�Ƿ�ģ�ͷ���
	*/  
	inline      bool                 GetModelEmit()
	{
		return m_bModelEmit;
	}

	/**
	@brief		��ȡģ�ͷ��䶥�㼰���Ӧ�ķ���

	@return		ģ�ͷ��䶥�㼰���Ӧ�ķ���
	*/  
	inline      void                 GetModelVertexAndNormal(Point3F* vex, Point3F* nor)
	{
		if (m_vecModelVertex.size() <= 0)
		{
			*vex = Point3F(0.0f, 0.0f, 0.0f);
			*nor = Point3F(0.0f, 0.0f, 0.0f);
			return;
		}
		S32 temp = mRandGen.randI(0, m_vecModelVertex.size() - 1);
		*vex = m_vecModelVertex[temp];
		*nor = m_vecModelNormal[temp];
	}

	/**
	@brief		��ȡʱ����ϵ�����ֵ

	@return		�Ƿ�ɹ�
	*/  
	inline      bool                 GetScalePoint(S32 pos, Point3F* point)
	{
		if(m_vecScalePoint.size() > pos)
		{
			*point = m_vecScalePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ����ֵʱ���

	@return		�Ƿ�ɹ�
	*/  
	inline      bool                 GetScaleTimePoint(S32 pos, F32* point)
	{
		if(m_vecScaleTimePoint.size() > pos)
		{
			*point = m_vecScaleTimePoint[pos];
			return true;
		}
		return false;
	}

	/**
	@brief		��ȡ���״̬

	@return		���״̬
	*/  
	inline      S32                 GetTextureStageState()
	{
		return m_iTextureStageState;
	}

	/**
	@brief		���÷���������ʱ��

	@return		��
	*/  
	inline      void                SetEmitLife(F32 life)
	{
		m_fEmitLife = life;
	}

	/**
	@brief		��ȡ����������ʱ��

	@return		����������ʱ��
	*/  
	inline      F32                 GetEmitLife()
	{
		return m_fEmitLife;
	}

	/**
	@brief		���ñ����

	@return		��
	*/  
	inline      void                SetGlow(F32 data)
	{
		m_fGlow = data;
	}

	/**
	@brief		��ȡ�����

	@return		�����
	*/  
	inline      F32                 GetGlow()
	{
		return m_fGlow;
	}

	/**
	@brief		��ȡ�Է�����ɫ

	@return		�Է�����ɫ
	*/  
	inline      Point3F             GetSelfIllumination()
	{
		return m_vSelfIllumination;
	}

	/**
	@brief		��ȡ�ع�ָ��

	@return		�ع�ָ��
	*/  
	inline      F32                 GetPowValue()
	{
		return m_fPowValue;
	}

	/**
	@brief		������ʾ����

	@return		��
	*/  
	inline      void                SetDisplayVelocity(float velocity)
	{
		m_fDisplayVelocity = velocity;
	}

	/**
	@brief		��ȡ��ʾ����

	@return		��ʾ����
	*/  
	inline      float               GetDisplayVelocity()
	{
		return m_fDisplayVelocity;
	}

	/**
	@brief		��ȡ����λ��

	@return		�Ƿ�ɹ�
	*/  
	bool                            GetAnimationPosition(F32 time, Point3F* pos);

	inline   S32                 GetRotNum()
	{
		return m_vecRotValue.size();
	}
	inline   S32                 GetAngleNum()
	{
		return m_vecAnglePoint.size();
	}
	// move this procedure to Particle2
	void packData(BitStream* stream);
	void unpackData(BitStream* stream);
	bool onAdd();
	bool preload(bool server, char errorBuffer[256]);
	DECLARE_CONOBJECT(ParticleData);
	static void  initPersistFields();

	bool reload();
protected:
	/**
	@brief		����

	@return		��
	*/ 
	void                 Destory();

};

//*****************************************************************************
// Particle2
// 
// This structure should be as small as possible.
//*****************************************************************************
struct Particle
{
	F32                m_fAge;                                     //�����Ѿ�����ʱ��
	F32                m_fLife;
	F32                m_fCurSizeX;                                 //���ӵ�ǰ�Ĵ�С�ߴ�
	F32                m_fSizeStepX;
	F32                m_fSizeTimeX;
	F32                m_fCurSizeY;                                 //���ӵ�ǰ�Ĵ�С�ߴ�
	F32                m_fSizeStepY;
	F32                m_fSizeTimeY;
	Point3F            m_CurPos;                                   //���ӵ�ǰ��λ��
	ColorF             m_Color;                                    //��ǰ���ӵ���ɫ
	ColorF             m_ColorStep;
	F32                m_fColorTime;
	F32                m_fAlphaStep;
	F32                m_fAlphaTime;
	F32                m_fCurFlickerTime;
	F32                m_fFlickerTime;
	bool               m_bDraw;
	Point3F            m_vEula;
	F32                m_fVelocity;
    Point3F            m_vWorldPos;
	MatrixF            m_matParent;
	Point3F            m_vEmitterPos;

	union 
	{
		F32            m_fAngle;                                   //��ǰ������ת�ĽǶ�
		F32            m_fRot[3];                                  //��ǰ����3��������ת�Ļ���
	}Rotate;
	union
	{
		F32            m_fAngleStep;
		F32            m_fRotStep[3];
	}RotateStep;
	F32                m_fRotTime;

	F32                m_fGravity;                                 //���������ӵ�Ӱ���
	F32                m_fGravityStep;
	F32                m_fGravityTime;
	Particle*          m_pNext;                                    //ָ����һ���ӵ�ָ��
	Particle*          m_pPrev;                                    //ָ����һ���ӵ�ָ��
	F32                m_fViewZ;                                   //�ӿռ�����ֵ
	bool               m_bRotCCW;                                  //�Ƿ���ʱ����ת
	S32                m_iTextureSplinterCode;                     //��ͼ�ֿ���

	F32                m_fCurTexRemainTime;                        //����ֿ�Ĵ���ʱ��
	Particle()
	{
		m_fAge = -1.0f;
		m_fLife = 0.0f;
		m_fCurSizeX = 1.0f;
		m_fSizeStepX = 0.0f;
		m_fSizeTimeX = 0.0f;
		m_fCurSizeY = 1.0f;
		m_fSizeStepY = 0.0f;
		m_fSizeTimeY = 0.0f;
		m_CurPos = m_vWorldPos = Point3F(0.0f, 0.0f, 0.0f);
		m_Color = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
		m_ColorStep = ColorF(0.0f, 0.0f, 0.0f, 0.0f);
		m_fColorTime = 0.0f;
		m_fAlphaStep = 0.0f;
		m_fAlphaTime = 0.0f;
		m_fCurFlickerTime = 0.0f;
		m_fFlickerTime = 0.0f;
		m_bDraw = true;
		m_vEula = m_vEmitterPos = Point3F(0.0f, 0.0f, 0.0f);
        m_fVelocity = 0.0f;
		m_fGravity = 0.0f;
		m_fGravityStep = 0.0f;
		m_fGravityTime = 0.0f;
		m_fRotTime = 0.0f;
		m_pNext = NULL;
		m_pPrev = NULL;
		m_fViewZ = 0.0f;
		m_bRotCCW = false;
		m_iTextureSplinterCode = 0;
		m_fCurTexRemainTime = 0.0f;
		m_matParent.identity();
	}
};


#endif // _PARTICLE_H_