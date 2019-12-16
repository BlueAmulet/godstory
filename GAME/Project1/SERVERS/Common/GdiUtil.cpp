#include "GdiUtil.h"

GdiUtil::Helper::Helper()
{
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

GdiUtil::Helper::~Helper()
{
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

GdiUtil::Helper GdiUtil::sHelper;

char* GdiUtil::genImage( wchar_t* buf, int width, int height )
{
	if( !buf )
		return NULL;

	// 创建调色板位图，节省传输资源
	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap( width, height, PixelFormat8bppIndexed );
	Gdiplus::Graphics* pPaint = Gdiplus::Graphics::FromImage( bmp );
	Gdiplus::Font* pFont = new Gdiplus::Font( L"System", 11 );

	Gdiplus::PointF pf;
	pf.X = 0;
	pf.Y = 0;

	Gdiplus::SolidBrush* brush = new Gdiplus::SolidBrush( Gdiplus::Color::Black );

	pPaint->Clear( Gdiplus::Color::White );

	// 生成一个默认的灰度调色板
	int size = bmp->GetPaletteSize();
	Gdiplus::ColorPalette* pPal = (Gdiplus::ColorPalette*)malloc( size );

#pragma region 设置调色板内容
	
	for( int i = 0; i < pPal->Count; i++ )
	{
		pPal->Entries[i] = 0xff000000 | i << 24 | i << 16 | i;
	}

	bmp->SetPalette( pPal );

#pragma endregion 设置调色板内容

	pPaint->DrawString( buf, wcslen( buf ), pFont, pf, brush );

	Gdiplus::Rect rect(0, 0, width, height);
	char *bufImg = new char[width * height];

	Gdiplus::BitmapData data;
	bmp->LockBits( &rect, Gdiplus::ImageLockModeRead, PixelFormat8bppIndexed, &data );

	char* pSrc = (char*)data.Scan0;
	char* pDest = bufImg;

	int length = width * PixelSize;

	for( int i = 0; i < height; i++ )
	{
		memcpy( pDest,  pSrc, length);
		pSrc += data.Stride;
		pDest += length;
	}

	bmp->UnlockBits( &data );

	delete brush;
	delete pFont;
	delete pPaint;
	delete bmp;
	free( pPal );

	return bufImg;
}

void GdiUtil::loadImage( IDirectDrawSurface7* pSurface, char* buf, int length, int width, int height )
{
	Gdiplus::Bitmap* pBmp = Gdiplus::Bitmap::FromDirectDrawSurface7( pSurface );
	Gdiplus::Graphics* pPaint = Gdiplus::Graphics::FromImage( pBmp );

	// 生成一个默认的灰度调色板
	int size = pBmp->GetPaletteSize();
	Gdiplus::ColorPalette* pPal = (Gdiplus::ColorPalette*)malloc( size );

#pragma region 设置调色板内容

	for( int i = 0; i < pPal->Count; i++ )
	{
		pPal->Entries[i] = 0xff000000 | i << 24 | i << 16 | i;
	}

	pBmp->SetPalette( pPal );

#pragma endregion 设置调色板内容

	Gdiplus::Rect rect(0, 0, width, height);
	char *bufImg = new char[width * height];

	Gdiplus::BitmapData data;
	pBmp->LockBits( &rect, Gdiplus::ImageLockModeRead, PixelFormat8bppIndexed, &data );

	char* pSrc = (char*)data.Scan0;
	char* pDest = bufImg;

	for( int i = 0; i < height; i++ )
	{
		memcpy( pDest,  pSrc, length);
		pSrc += data.Stride;
		pDest += length;
	}

	pBmp->UnlockBits( &data );


	delete pPal;
	delete pPaint;
	delete pBmp;
}
