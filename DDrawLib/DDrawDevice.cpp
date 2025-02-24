// DDraw.cpp: implementation of the CDDrawDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../Common/typedef.h"
#include <stdio.h>
#include "DDrawDevice.h"
#include <algorithm>
#include "../Util/Util.h"
#include "../ImageData/ImageData.h"
#include "../Util/TGAImage.h"
#include "../Game/DebugManager.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDDrawDevice::CDDrawDevice()
{
	memset(this, 0, sizeof(CDDrawDevice));

}

#include <ddraw.h>

BOOL CDDrawDevice::InitializeDDraw(HWND hWnd)
{
	BOOL	bResult = FALSE;

	m_hWnd = hWnd;

	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;


	if (DD_OK != DirectDrawCreate(nullptr, &m_pDD, nullptr))
	{
		MessageBox(hWnd, L"Fail to Create DirectDraw", L"Error", MB_OK);
		goto lb_return;
	}

	if (DD_OK != m_pDD->QueryInterface(IID_IDirectDraw7, (LPVOID*)&m_pDD7))
	{
		MessageBox(hWnd, L"Fail to Create DirectDraw 4", L"Error", MB_OK);
		goto lb_return;
	}

	HRESULT hr = m_pDD7->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Set CooperativeLevel", L"ERROR", MB_OK);
		goto lb_return;
	}



	// Create the primary surface.
	hr = m_pDD7->CreateSurface(&ddsd, &m_pDDPrimary, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to CreateSurface", L"ERROR", MB_OK);
		goto lb_return;
	}
	// Create a clipper object which handles all our clipping for cases when
	// our window is partially obscured by other windows. This is not needed
	// for apps running in fullscreen mode.

	// Ŭ����(Clipper)�� DirectDraw���� ������ ��� ���ø����̼��� â�� �ٸ� â�� ���� �Ϻ� �������� ��, �� ������ �κ��� �ڵ����� ó�����ִ� ����
	// ex) ȭ�� ���� �޼��� �ڽ�

	hr = m_pDD->CreateClipper(0, &m_pClipper, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Create Clipper", L"ERROR", MB_OK);
		goto lb_return;
	}
	// Associate the clipper with our window. Note that, afterwards, the
	// clipper is internally referenced by the primary surface, so it is safe
	// to release our local reference to it.
	m_pClipper->SetHWnd(0, hWnd);
	m_pDDPrimary->SetClipper(m_pClipper);

	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	OnUpdateWindowPos();

	DWORD dwWidth = m_rcWindow.right - m_rcWindow.left;
	DWORD dwHeight = m_rcWindow.bottom - m_rcWindow.top;

	if (!CreateBackBuffer(dwWidth, dwHeight))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}

	bResult = TRUE;

lb_return:
	return bResult;

}


BOOL CDDrawDevice::CreateBackBuffer(DWORD dwWidth, DWORD dwHeight)
{
	BOOL	bResult = FALSE;

	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = dwWidth;
	ddsd.dwHeight = dwHeight;

	HRESULT hr = m_pDD7->CreateSurface(&ddsd, &m_pDDBack, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to Create Surface", L"ERROR", MB_OK);
		goto lb_return;
	}
	m_pDDBack->GetSurfaceDesc(&ddsd);
	m_dwWidth = ddsd.dwWidth;
	m_dwHeight = ddsd.dwHeight;
	bResult = TRUE;
lb_return:
	return bResult;
}

void CDDrawDevice::Clear()
{
	if (!m_pWriteBuffer)
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return;
	}
	for (DWORD y = 0; y < m_dwHeight; y++)
	{
		memset(m_pWriteBuffer + y * m_dwWriteBufferPitch, 0, 4 * m_dwWidth);
	}
}

BOOL CDDrawDevice::LockBackBuffer(char** ppBits, DWORD* pdwWidth, DWORD* pdwHeight, DWORD* pdwPitch)
{
	BOOL	bResult = FALSE;

	if (m_pDDBack)
	{

		DDSURFACEDESC2 ddsc;
		memset(&ddsc, 0, sizeof(DDSURFACEDESC2));
		ddsc.dwSize = sizeof(DDSURFACEDESC2);

		m_pDDBack->Lock(nullptr, &ddsc, DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr);

		*ppBits = (char*)ddsc.lpSurface;
		*pdwPitch = ddsc.lPitch;
		*pdwWidth = ddsc.dwWidth;
		*pdwHeight = ddsc.dwHeight;
	}

	bResult = TRUE;
lb_return:
	return bResult;
}
void CDDrawDevice::UnlockBackBuffer()
{
	if (m_pDDBack)
		m_pDDBack->Unlock(nullptr);
}

DWORD CDDrawDevice::CaptureBackBuffer(FILE* fp)
{
	DWORD	dwResult = 0;

	if (!m_pDDBack)
		goto lb_return;

	DDSURFACEDESC2 ddsc;
	memset(&ddsc, 0, sizeof(DDSURFACEDESC2));
	ddsc.dwSize = sizeof(DDSURFACEDESC2);

	m_pDDBack->Lock(nullptr, &ddsc, DDLOCK_WAIT | DDLOCK_READONLY, nullptr);

	dwResult = WriteTGAImage(fp, (char*)ddsc.lpSurface, ddsc.dwWidth, ddsc.dwHeight, ddsc.lPitch, 32);
	m_pDDBack->Unlock(nullptr);

lb_return:
	return dwResult;
}


void CDDrawDevice::SetPixelRandom()
{
	DWORD	dwColor = 0xffffffff;

	char*	pBits = nullptr;
	DWORD	dwWidth = 0;
	DWORD	dwHeight = 0;
	DWORD	dwPitch = 0;
	if (!LockBackBuffer(&pBits, &dwWidth, &dwHeight, &dwPitch))
	{
		__debugbreak();
	}
	DWORD x = rand() % dwWidth;
	DWORD y = rand() % dwHeight;
	DWORD*	pDest = (DWORD*)(pBits + x * 4 + y * dwPitch);
	*pDest = dwColor;

	UnlockBackBuffer();

}
void CDDrawDevice::OnUpdateWindowSize()
{
	CleanupBackBuffer();

	OnUpdateWindowPos();

	DWORD dwWidth = m_rcWindow.right - m_rcWindow.left;
	DWORD dwHeight = m_rcWindow.bottom - m_rcWindow.top;

	CreateBackBuffer(dwWidth, dwHeight);

	if (BeginDraw())
	{
		Clear();
		EndDraw();
	}

}
void CDDrawDevice::OnUpdateWindowPos()
{
	GetClientRect(m_hWnd, &m_rcWindow);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.left);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.right);
}
void CDDrawDevice::DrawRect(int sx, int sy, int iWidth, int iHeight, DWORD dwColor)
{

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();

#endif
	int start_x = max(0, sx);
	int start_y = max(0, sy);
	int end_x = min(sx + iWidth, (int)m_dwWidth);
	int end_y = min(sy + iHeight, (int)m_dwHeight);

	for (int y = start_y; y < end_y; y++)
	{
		for (int x = start_x; x < end_x; x++)
		{
			DWORD*	pDest = (DWORD*)(m_pWriteBuffer + x * 4 + y * m_dwWriteBufferPitch);
			*pDest = dwColor;
		}
	}
}
BOOL CDDrawDevice::CalcClipArea(INT_VECTOR2* pivOutSrcStart, INT_VECTOR2* pivOutDestStart, INT_VECTOR2* pivOutDestSize, const INT_VECTOR2* pivPos, const INT_VECTOR2* pivImageSize)
{
	INT_VECTOR2	ivBufferSize = { (int)m_dwWidth, (int)m_dwHeight };
	BOOL bResult = ::CalcClipArea(pivOutSrcStart, pivOutDestStart, pivOutDestSize, pivPos, pivImageSize, &ivBufferSize);
	return bResult;
}

BOOL CDDrawDevice::CalcSpriteClipArea(
	INT_VECTOR2* pivOutSrcStart,      // ���� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ��)
	INT_VECTOR2* pivOutDestStart,     // ���� ȭ����� ���� ��ǥ
	INT_VECTOR2* pivOutDestSize,      // ���� �������� ������ ũ��
	const RECT& srcRect,              // ��������Ʈ ��Ʈ �������� ������ ���� (��: ������ ����)
	const INT_VECTOR2* pDestPos,      // ȭ����� ������ ���� ��ǥ (��: {screenX, screenY})
	const INT_VECTOR2* pBufferSize)   // ȭ��(����) ũ�� (��: {width, height})
{

	// 1. srcRect�� ������� ���ϴ� ��������Ʈ ���� ũ�� ���
	INT_VECTOR2 desiredSize;
	desiredSize.x = srcRect.right - srcRect.left;
	desiredSize.y = srcRect.bottom - srcRect.top;

	// 2. ȭ����� ������ ���� Ŭ����: destPos�� desiredSize�� �̿��Ͽ� ���� �׸� ���� ����
	int destStartX = std::max<int>(pDestPos->x, 0);
	int destStartY = std::max<int>(pDestPos->y, 0);
	int destEndX = std::min<int>(pDestPos->x + desiredSize.x, pBufferSize->x);
	int destEndY = std::min<int>(pDestPos->y + desiredSize.y, pBufferSize->y);

	int clippedWidth = destEndX - destStartX;
	int clippedHeight = destEndY - destStartY;

	if (clippedWidth <= 0 || clippedHeight <= 0)
		return FALSE;			// �������� ������ ����

	// 3. ��������Ʈ ��Ʈ �� �ҽ� ���� ��ǥ ���:
	// ȭ����� ���� ��ǥ�� ���� destPos�� ���̸� srcRect�� ����/��ܿ� ����
	int srcStartX = destStartX - pDestPos->x + srcRect.left;
	int srcStartY = destStartY - pDestPos->y + srcRect.top;

	pivOutSrcStart->x = srcStartX;
	pivOutSrcStart->y = srcStartY;
	pivOutDestStart->x = destStartX;
	pivOutDestStart->y = destStartY;
	pivOutDestSize->x = clippedWidth;
	pivOutDestSize->y = clippedHeight;


	return TRUE;
}




BOOL CDDrawDevice::BeginDraw()
{
	BOOL	bResult = FALSE;

	char*	pBuffer = nullptr;
	DWORD	dwBufferWidth = 0;
	DWORD	dwBufferHeight = 0;
	DWORD	dwPitch = 0;
	if (!LockBackBuffer(&pBuffer, &dwBufferWidth, &dwBufferHeight, &dwPitch))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}
#ifdef _DEBUG
	if (dwBufferWidth != m_dwWidth)
		__debugbreak();

	if (dwBufferHeight != m_dwHeight)
		__debugbreak();
#endif
	m_pWriteBuffer = pBuffer;
	m_dwWriteBufferPitch = dwPitch;
	bResult = TRUE;
lb_return:
	return bResult;
}
void CDDrawDevice::EndDraw()
{
	UnlockBackBuffer();
	m_pWriteBuffer = nullptr;
	m_dwWriteBufferPitch = 0;
}
BOOL CDDrawDevice::DrawBitmap(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();

#endif


	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;


	char* pSrc = pBits + (ivSrcStart.x + ivSrcStart.y * iBitmapWidth) * 4;
	char* pDest = m_pWriteBuffer + (ivDestStart.x * 4) + ivDestStart.y * m_dwWriteBufferPitch;

	for (int y = 0; y < ivDestSize.y; y++)
	{
		for (int x = 0; x < ivDestSize.x; x++)
		{
			*(DWORD*)pDest = *(DWORD*)pSrc;
			pSrc += 4;
			pDest += 4;
		}
		pSrc -= (ivDestSize.x * 4);
		pSrc += (iBitmapWidth * 4);
		pDest -= (ivDestSize.x * 4);
		pDest += m_dwWriteBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}

BOOL CDDrawDevice::DrawBitmapWithColorKey(int sx, int sy, int iBitmapWidth, int iBitmapHeight, char* pBits, DWORD dwColorKey)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();

#endif


	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;


	char* pSrc = pBits + (ivSrcStart.x + ivSrcStart.y * iBitmapWidth) * 4;
	char* pDest = m_pWriteBuffer + (ivDestStart.x * 4) + ivDestStart.y * m_dwWriteBufferPitch;

	for (int y = 0; y < ivDestSize.y; y++)
	{
		for (int x = 0; x < ivDestSize.x; x++)
		{
			if (*(DWORD*)pSrc != dwColorKey)
			{
				*(DWORD*)pDest = *(DWORD*)pSrc;
			}
			pSrc += 4;
			pDest += 4;
		}
		pSrc -= (ivDestSize.x * 4);
		pSrc += (iBitmapWidth * 4);
		pDest -= (ivDestSize.x * 4);
		pDest += m_dwWriteBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}

// sx, sy : DirectX ��ũ�� ��ǥ��󿡼��� ��ġ

BOOL CDDrawDevice::DrawImageData(int sx, int sy, const CImageData* pImgData)
{
	BOOL	bResult = FALSE;

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();

#endif

	int iScreenWidth = (int)m_dwWidth;

	int iBitmapWidth = (int)pImgData->GetWidth();
	int iBitmapHeight = (int)pImgData->GetHeight();

	INT_VECTOR2	ivSrcStart = {};
	INT_VECTOR2	ivDestStart = {};

	INT_VECTOR2	ivPos = { sx, sy };
	INT_VECTOR2	ivImageSize = { iBitmapWidth, iBitmapHeight };
	INT_VECTOR2 ivDestSize = {};

	if (!CalcClipArea(&ivSrcStart, &ivDestStart, &ivDestSize, &ivPos, &ivImageSize))
		goto lb_return;

	const COMPRESSED_LINE* pLineDesc = pImgData->GetCompressedImage(ivSrcStart.y);
	char* pDestPerLine = m_pWriteBuffer + (ivDestStart.y) * m_dwWriteBufferPitch;


	for (int y = 0; y < ivDestSize.y; y++)
	{

		for (DWORD i = 0; i < pLineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM*	pStream = pLineDesc->pPixelStream + i;
			DWORD	dwPixelColor = pStream->dwPixel;
			int		iPixelNum = (int)pStream->wPixelNum;

			int dest_x = sx + (int)pStream->wPosX;
			if (dest_x < 0)
			{
				iPixelNum += dest_x;
				dest_x = 0;
			}
			
			if (dest_x + iPixelNum > iScreenWidth)
			{
				iPixelNum = iScreenWidth - dest_x;
			}

			char* pDest = pDestPerLine + (DWORD)(dest_x * 4);
			
			for (int x = 0; x < iPixelNum; x++)
			{
				*(DWORD*)pDest = dwPixelColor;
				pDest += 4;
			}
		}

		pLineDesc++;
		pDestPerLine += m_dwWriteBufferPitch;
	}
	//
	bResult = TRUE;
lb_return:
	return bResult;

}

BOOL CDDrawDevice::DrawSprite(int screenX, int screenY, const CImageData* pImgData, const RECT& srcRect)
{
	BOOL bResult = FALSE;

#ifdef _DEBUG
	if (!m_pWriteBuffer)
		__debugbreak();
#endif

	// ȭ���� �ʺ� (��� ���)
	int screenWidth = static_cast<int>(m_dwWidth);

	// ��������Ʈ �������� ũ�� ��� (srcRect ����)
	int frameWidth = srcRect.right - srcRect.left;
	int frameHeight = srcRect.bottom - srcRect.top;

	// ��������Ʈ ��Ʈ ��ü �̹����� ũ��
	/*
	int imageWidth = static_cast<int>(pImgData->GetWidth());
	int imageHeight = static_cast<int>(pImgData->GetHeight());
	*/

	// �������� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ���� ��ǥ��)
	INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

	// ȭ����� ���� ��ǥ (��ũ�� ��ǥ��)
	INT_VECTOR2 destStart = { screenX, screenY };

	// ������ ������ ũ�� �ʱⰪ�� ��������Ʈ �������� ũ��� ����
	INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
	INT_VECTOR2 clippedSize = {};

	// ��� ��ġ�� ���� ũ��
	INT_VECTOR2 destPos = { screenX, screenY };
	INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

	// CalcClipArea() �Լ��� destStart�� spriteSize�� �������
	// ������ ȭ�鿡 �׷��� ����(Ŭ���� ����)�� ���
	if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
		goto lb_return;


	// pImgData���� srcStart.y ���� ����� �����͸� ��������
	const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

	// ��� ������ ù �� ������ (destStart.y ����� ����)
	char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

	// Ŭ���� ������ ���̸�ŭ �ݺ�
	for (int y = 0; y < clippedSize.y; y++)
	{
		// �� ���� ��Ʈ�� �����͸� ��ȸ
		for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
			DWORD pixelColor = pStream->dwPixel;
			int pixelCount = static_cast<int>(pStream->wPixelNum);

		


			/*
			�̴� 0�����࿡�� ��ü Sprite�̹��� �� Rect�����ȿ� �ش��ϴ� �̹����� Render�ϱ� ���� Case����
			���� �Ʒ� Case���� ������� ������ ��������Ʈ ��Ʈ ��ü�� ��������
			
			pStream->wPosX �� ��ü �̹����� ������ǥ��󿡼��� ���ӵ� �ȼ��� ���۵Ǵ� X��ǥ�̶�� ���� ����ؾ���
			�� {wPosX, wPosX + pixelCount} ��ŭ�� ���� Line���� ���ӵ� �����̶�°�
			���� �¿� Screen������ ���� �׷����� ���� �����ϴ� ������ �����ϴ���, �� ���� �۾��� �߰��ؼ� ó���ؾ���

			�켱 ���� srcRect ���� �ȿ� ���� ���� X�� ���ؾ��� �� ����
			�� ������ǥ�� ������ �����ϱ�

			Case1)   wPosX, wPosX + pixelCount�� Rect���� ������ �Ѿ�� �� �翬�� Render�� �ʿ䰡 ����
			Case2)   wPosX�� �Ѿ�� wPosX + pixelCount�� Rect���� ���ʿ� ���� ��
					   �� ���, ���� ��ǥ�� Rect.left �� ���װ�, PixelCount���� (wPosX + pixelCount - Rect.left) �̰� �� ��

			Case3)	wPosX, wPosX + pixelCount�� Rect���� ������ �� �״�� ���
			Case4)	wPosX�� Rect����, wPosX + pixelCount�� Rect�ٱ���
					    �� ���, ������ǥ�� wPosX, pixelCount�� (Rect.Right - wPosX)

			Case5)	wPosX, wPosX + pixelCount �� �� Rect �ٱ����� �� Render�� �ʿ䰡 ����

			�̷��� Rect�ȿ��� �׷��� localRenderStartX, pixelCount�� ���س� ��
			�̸� ������� screenRenderX = screenX + localRenderStartX
			�̷������� �θ� �� �� ����

			���� ���� screenRenderX�� ȭ�� �������, ������踦 ����� �� ó���� �����ϸ� �� �� ����

			�߰��� Rect���� Local ��ǥ��� �������� �� (Rect.left�� �����)
			�� (srcStart.x - srcRect.left) ���� ����� ��� ȭ���� �������� clipping�� �Ͼ ���̹Ƿ� �̿� ���� ó���� �߰���
			
			*/

			
			// Rect ���� Local ��ǥ��� �����ؼ� �����ϱ� (srcRect.left�� ���ִ� ��)
			// �� �� (srcStart.x - srcRect.left) �� ����̸� clipping�� �Ͼ ���̹Ƿ�, �� �κ��� ����Ͽ� localX�� frameWidth�� ����
			// localX = (wPosX - srcRect.left) - (srcStart.x - srcRect.left) = wPosX - srcStart.x
			int localX = static_cast<int>(pStream->wPosX) - srcStart.x;			// ���� ���Ŀ� ���� srcRect.left�� �Ұŵ�
			frameWidth = srcRect.right - srcStart.x;
			

			// Case1, Case5: srcRect ������ ��� ���
			if (localX + pixelCount <= 0 || localX >= frameWidth)
				continue;

			
			// Case2: ������ ��� ���
			if (localX < 0) {
				pixelCount += localX; // localX�� ����
				localX = 0;
			}

			// Case4: �������� ��� ���
			if (localX + pixelCount > frameWidth) {
				pixelCount = frameWidth - localX;
			}


			// ���������� ȭ����� x ��ǥ = destStart.x + localX
			int drawX = destStart.x + localX;


			// ȭ�� ���� ��踦 ����� ����
			
			if (drawX < 0)
			{
				pixelCount += drawX;
				drawX = 0;
			}
			

			// ȭ�� ���� ��踦 ����� ����
			if (drawX + pixelCount > screenWidth)
			{
				pixelCount = screenWidth - drawX;
			}


			// ��� ���� �� �ش� �ȼ� ��ġ ��� (�ȼ��� 4Byte)
			char* destPixelPtr = destLinePtr + (drawX * 4);

			// �ش� ��Ʈ���� �ȼ����� ����
			for (int x = 0; x < pixelCount; x++)
			{
				*(DWORD*)destPixelPtr = pixelColor;
				destPixelPtr += 4;
			}
		}
		lineDesc++; // ���� ���� ���� ������
		destLinePtr += m_dwWriteBufferPitch; // ��� ������ ���� �� ������
	}
	bResult = TRUE;
lb_return:
	return bResult;
}


BOOL CDDrawDevice::DrawSpriteFlip(int screenX, int screenY, const CImageData* pImgData, const RECT& srcRect)
{
    BOOL bResult = FALSE;

#ifdef _DEBUG
    if (!m_pWriteBuffer)
        __debugbreak();
#endif

    // ȭ���� �ʺ� (��� ���)
    int screenWidth = static_cast<int>(m_dwWidth);

    // ��������Ʈ �������� ũ�� (srcRect ����)
    int frameWidth = srcRect.right - srcRect.left;
    int frameHeight = srcRect.bottom - srcRect.top;

    // �������� �ҽ� ���� ��ǥ (��������Ʈ ��Ʈ ���� ��ǥ��)
    INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

    // ȭ����� ���� ��ǥ (��ũ�� ��ǥ��)
    INT_VECTOR2 destStart = { screenX, screenY };

    // ������ ������ ũ��� ��������Ʈ �������� ũ��� �ʱ�ȭ
    INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
    INT_VECTOR2 clippedSize = {};

    // ��� ��ġ�� ���� ũ��
    INT_VECTOR2 destPos = { screenX, screenY };
    INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

    // CalcClipArea() �Լ��� destStart�� spriteSize�� ������� ���� ȭ�鿡 �׷��� ����(Ŭ���� ����)�� ���
    if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
        goto lb_return;

    // pImgData���� srcStart.y ���� ���� �����͸� ������
    const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

    // ��� ������ ù �� ������ (destStart.y ����� ����)
    char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

    // Ŭ���� ������ ���̸�ŭ �ݺ�
    for (int y = 0; y < clippedSize.y; y++)
    {
        // �� ���� ����� ��Ʈ�� �����͸� ��ȸ
        for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
        {
            PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
            DWORD pixelColor = pStream->dwPixel;
            int pixelCount = static_cast<int>(pStream->wPixelNum);

			// Rect �»���� �������� �ϴ� ������ǥ�踣 �������� �����ؾ� ����¥�Ⱑ ����
			// ���� srcRect.left�� �������
			int localX = static_cast<int>(pStream->wPosX) - srcRect.left;
			// frameWidth�� srcRect�� �ʺ�
			frameWidth = srcRect.right - srcRect.left;

            // ���� ��Ʈ���� srcRect ������ ����� �ǳʶ�
            if (localX + pixelCount <= 0 || localX >= frameWidth)
                continue;

            // Case2: ������ ��� ���
            if (localX < 0) {
                pixelCount += localX; // localX�� ����
                localX = 0;
            }

            // Case4: �������� ��� ���
            if (localX + pixelCount > frameWidth) {
                pixelCount = frameWidth - localX;
            }

            // �¿� ���� ó��:
            // ���� �¿� ����� destStart.x + localX�� ���������,
            // ������ ��쿣 �ش� �������� ������ ���������� localX��ŭ ������ ��ġ�� �����ϰ�,
            // pixelCount��ŭ �ȼ��� ä��Ƿ�,
            // drawX = destStart.x + (frameWidth - localX - pixelCount)
            int drawX = destStart.x + (frameWidth - localX - pixelCount);
			
			// �̹����� ���⼭ ���� ��ǥ��ġ�� ���ʿ��� Clipping �� ��ŭ �Ű������
			// (srcStart.x - srcRect.left) �� ������ ���ʿ��� Clippiing�� �Ͼ ��Ȳ

			drawX -= (srcStart.x - srcRect.left);

            // ȭ�� ���� ��� Ŭ����
            if (drawX < 0)
            {
                pixelCount += drawX;
                drawX = 0;
            }

            // ȭ�� ���� ��� Ŭ����
            if (drawX + pixelCount > screenWidth)
            {
                pixelCount = screenWidth - drawX;
            }

            // ��� ���� �� �ش� �ȼ� ��ġ ��� (�ȼ��� 4Byte)
            // (������ �̹����� �����ʿ������� ä�����Ƿ�, ���⼭�� �Ϲ����� ������ �����ص� ������ ���� �ݺ� ���� �� ��������)
            char* destPixelPtr = destLinePtr + (drawX * 4);

            // ���� ������ �ȼ����� pixelCount��ŭ ����
            for (int x = 0; x < pixelCount; x++)
            {
                *(DWORD*)destPixelPtr = pixelColor;
                destPixelPtr += 4;
            }
        }
        lineDesc++; // ���� ���� ���� ������
        destLinePtr += m_dwWriteBufferPitch; // ��� ������ ���� �� ������
    }
    bResult = TRUE;
lb_return:
    return bResult;
}




void CDDrawDevice::UpdateInfoTxt()
{
	m_dwInfoTxtLen = swprintf_s(m_wchInfoTxt, L"FPS : %u", m_dwFPS);
}
BOOL CDDrawDevice::CheckFPS()
{
	BOOL	bUpdated = FALSE;

	ULONGLONG CurTick = GetTickCount64();
	if (CurTick - m_LastDrawTick > 1000)
	{
		DWORD	dwOldPFS = m_dwFPS;

		m_dwFPS = m_dwFrameCount;
		m_LastDrawTick = CurTick;
		m_dwFrameCount = 0;
		if (m_dwFPS != dwOldPFS)
		{
			UpdateInfoTxt();
			bUpdated = TRUE;
		}
		else
		{
			int a = 0;
		}
	}
	m_dwFrameCount++;
	return bUpdated;
}
#if defined(ARM64) && !defined(_DEBUG)
//#pragma optimize( "gpsy",off)
//#pragma optimize( "g",off)
void CDDrawDevice::OnDraw()
{
	
	//WCHAR	wchTxt[128];
	//swprintf_s(wchTxt, L"%p, %p, %d,%d, %d,%d\n", m_pDDPrimary, m_pDDBack, m_rcWindow.left, m_rcWindow.top, m_rcWindow.right, m_rcWindow.bottom);
	//OutputDebugString(wchTxt);
	m_pDDPrimary->Blt(&m_rcWindow, m_pDDBack, nullptr, DDBLT_WAIT, nullptr);
}
//#pragma optimize( "",on)
#else
void CDDrawDevice::OnDraw()
{
	m_pDDPrimary->Blt(&m_rcWindow, m_pDDBack, nullptr, DDBLT_WAIT, nullptr);
}
#endif

void CDDrawDevice::ProcessGDI(HDC hDC)
{


}
BOOL CDDrawDevice::BeginGDI(HDC* pOutDC)
{
	BOOL	bResult = FALSE;
	HDC	hDC = nullptr;

	HRESULT hr = m_pDDBack->GetDC(&hDC);
	if (FAILED(hr))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		goto lb_return;
	}
	bResult = TRUE;
	*pOutDC = hDC;

lb_return:
	return bResult;
}
void CDDrawDevice::DrawInfo(HDC hDC)
{
	const WCHAR* wchTxt = m_wchInfoTxt;
	DWORD dwLen = m_dwInfoTxtLen;

	WriteText(wchTxt, dwLen, 0, 0, 0xffff0000, hDC);
}
void CDDrawDevice::EndGDI(HDC hDC)
{
	ProcessGDI(hDC);
	m_pDDBack->ReleaseDC(hDC);
}
void CDDrawDevice::WriteText(const WCHAR* wchTxt, DWORD dwLen, int x, int y, DWORD dwColor, HDC hDC)
{
	SetBkMode(hDC, TRANSPARENT);

	RECT	textRect, texRectSide[4];
	int		iWidth = 0;
	int		iHeight = 0;
	GetFontSize(&iWidth, &iHeight, wchTxt, dwLen, hDC);

	textRect.left = x;
	textRect.top = y;
	textRect.right = textRect.left + iWidth;
	textRect.bottom = textRect.top + iHeight;

	// texRectSide�� �ؽ�Ʈ�� �ܰ���ȿ��
	texRectSide[0].left = textRect.left - 1;
	texRectSide[0].top = textRect.top - 1;
	texRectSide[0].right = textRect.right - 1;
	texRectSide[0].bottom = textRect.bottom - 1;


	texRectSide[1].left = textRect.left + 1;
	texRectSide[1].top = textRect.top - 1;
	texRectSide[1].right = textRect.right + 1;
	texRectSide[1].bottom = textRect.bottom - 1;

	texRectSide[2].left = textRect.left + 1;
	texRectSide[2].top = textRect.top + 1;
	texRectSide[2].right = textRect.right + 1;
	texRectSide[2].bottom = textRect.bottom + 1;

	texRectSide[3].left = textRect.left - 1;
	texRectSide[3].top = textRect.top + 1;
	texRectSide[3].right = textRect.right - 1;
	texRectSide[3].bottom = textRect.bottom + 1;

	// ���� �ؽ�Ʈ�� �ܰ����� ���������� �׸�
	SetTextColor(hDC, 0x00000000);
	for (DWORD i = 0; i < 4; i++)
	{
		DrawText(hDC, wchTxt, -1, &texRectSide[i], DT_LEFT | DT_WORDBREAK);
	}

	DWORD r = (dwColor & 0x00ff0000) >> 16;
	DWORD g = (dwColor & 0x0000ff00) >> 8;
	DWORD b = (dwColor & 0x000000ff);

	COLORREF color = RGB(r, g, b);
	SetTextColor(hDC, color);
	DrawText(hDC, wchTxt, -1, &textRect, DT_LEFT | DT_WORDBREAK);


}

BOOL CDDrawDevice::GetFontSize(int* piOutWidth, int* piOutHeight, const WCHAR* wchString, DWORD dwStrLen, HDC hDC)
{
	BOOL	bResult = FALSE;
	*piOutWidth = 1;
	*piOutHeight = 1;

	SIZE	strSize;
	BOOL bGetSize = GetTextExtentPoint32W(hDC, wchString, dwStrLen, &strSize);


	if (bGetSize)
	{
		*piOutWidth = strSize.cx;
		*piOutHeight = strSize.cy;
		bResult = TRUE;
	}
	return bResult;
}
void CDDrawDevice::CleanupBackBuffer()
{
	if (m_pDDBack)
	{
		m_pDDBack->Release();
		m_pDDBack = nullptr;
	}
}
void CDDrawDevice::Cleanup()	
{
	CleanupBackBuffer();

	if (m_pDDPrimary)
	{
		m_pDDPrimary->SetClipper(nullptr);
		m_pDDPrimary->Release();
		m_pDDPrimary = nullptr;
	}
	if (m_pClipper)
	{
		m_pClipper->Release();
		m_pClipper = nullptr;
	}
	if (m_pDD7)
	{
		ULONG ref_count = m_pDD7->Release();
		if (ref_count)
			__debugbreak();
		m_pDD7 = nullptr;
	}
	if (m_pDD)
	{
		ULONG ref_count = m_pDD->Release();
		if (ref_count)
			__debugbreak();
		m_pDD = nullptr;
	}
}

CDDrawDevice::~CDDrawDevice()
{
	Cleanup();
}
