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

	// 클리퍼(Clipper)는 DirectDraw에서 윈도우 모드 애플리케이션이 창이 다른 창에 의해 일부 가려졌을 때, 그 가려진 부분을 자동으로 처리해주는 역할
	// ex) 화면 위의 메세지 박스

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
	INT_VECTOR2* pivOutSrcStart,      // 계산된 소스 시작 좌표 (스프라이트 시트 내)
	INT_VECTOR2* pivOutDestStart,     // 계산된 화면상의 시작 좌표
	INT_VECTOR2* pivOutDestSize,      // 실제 렌더링할 영역의 크기
	const RECT& srcRect,              // 스프라이트 시트 내에서의 렌더링 영역 (예: 프레임 영역)
	const INT_VECTOR2* pDestPos,      // 화면상의 렌더링 시작 좌표 (예: {screenX, screenY})
	const INT_VECTOR2* pBufferSize)   // 화면(버퍼) 크기 (예: {width, height})
{

	// 1. srcRect를 기반으로 원하는 스프라이트 영역 크기 계산
	INT_VECTOR2 desiredSize;
	desiredSize.x = srcRect.right - srcRect.left;
	desiredSize.y = srcRect.bottom - srcRect.top;

	// 2. 화면상의 렌더링 영역 클리핑: destPos와 desiredSize를 이용하여 실제 그릴 영역 결정
	int destStartX = std::max<int>(pDestPos->x, 0);
	int destStartY = std::max<int>(pDestPos->y, 0);
	int destEndX = std::min<int>(pDestPos->x + desiredSize.x, pBufferSize->x);
	int destEndY = std::min<int>(pDestPos->y + desiredSize.y, pBufferSize->y);

	int clippedWidth = destEndX - destStartX;
	int clippedHeight = destEndY - destStartY;

	if (clippedWidth <= 0 || clippedHeight <= 0)
		return FALSE;			// 렌더링할 영역이 없음

	// 3. 스프라이트 시트 내 소스 시작 좌표 계산:
	// 화면상의 시작 좌표와 원래 destPos의 차이를 srcRect의 좌측/상단에 더함
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

// sx, sy : DirectX 스크린 좌표계상에서의 위치

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

	// 화면의 너비 (출력 대상)
	int screenWidth = static_cast<int>(m_dwWidth);

	// 스프라이트 프레임의 크기 계산 (srcRect 기준)
	int frameWidth = srcRect.right - srcRect.left;
	int frameHeight = srcRect.bottom - srcRect.top;

	// 스프라이트 시트 전체 이미지의 크기
	/*
	int imageWidth = static_cast<int>(pImgData->GetWidth());
	int imageHeight = static_cast<int>(pImgData->GetHeight());
	*/

	// 렌더링할 소스 시작 좌표 (스프라이트 시트 로컬 좌표계)
	INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

	// 화면상의 시작 좌표 (스크린 좌표계)
	INT_VECTOR2 destStart = { screenX, screenY };

	// 복사할 영역의 크기 초기값은 스프라이트 프레임의 크기로 설정
	INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
	INT_VECTOR2 clippedSize = {};

	// 대상 위치와 버퍼 크기
	INT_VECTOR2 destPos = { screenX, screenY };
	INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

	// CalcClipArea() 함수는 destStart와 spriteSize를 기반으로
	// 실제로 화면에 그려질 영역(클립된 영역)을 계산
	if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
		goto lb_return;


	// pImgData에서 srcStart.y 행의 압축된 데이터를 가져오기
	const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

	// 대상 버퍼의 첫 행 포인터 (destStart.y 행부터 시작)
	char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

	// 클립된 영역의 높이만큼 반복
	for (int y = 0; y < clippedSize.y; y++)
	{
		// 각 줄의 스트림 데이터를 순회
		for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
		{
			PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
			DWORD pixelColor = pStream->dwPixel;
			int pixelCount = static_cast<int>(pStream->wPixelNum);

		


			/*
			이는 0번압축에서 전체 Sprite이미지 중 Rect범위안에 해당하는 이미지만 Render하기 위한 Case들임
			만약 아래 Case들을 고려하지 않으면 스프라이트 시트 전체가 렌더링됨
			
			pStream->wPosX 는 전체 이미지의 로컬좌표계상에서의 연속된 픽셀이 시작되는 X좌표이라는 점을 고려해야함
			즉 {wPosX, wPosX + pixelCount} 만큼이 현재 Line에서 연속된 구간이라는것
			기존 좌우 Screen범위에 맞춰 그려야할 수를 조절하는 로직을 수정하던가, 그 전에 작업을 추가해서 처리해야함

			우선 현재 srcRect 범위 안에 맞춰 시작 X를 정해야할 것 같음
			즉 로컬좌표계 계산부터 수행하기

			Case1)   wPosX, wPosX + pixelCount가 Rect범위 좌측을 넘어갔을 시 당연히 Render할 필요가 없음
			Case2)   wPosX는 넘어가고 wPosX + pixelCount는 Rect범위 안쪽에 있을 시
					   이 경우, 시작 좌표는 Rect.left 가 될테고, PixelCount수는 (wPosX + pixelCount - Rect.left) 이게 될 것

			Case3)	wPosX, wPosX + pixelCount가 Rect범위 안쪽일 시 그대로 사용
			Case4)	wPosX는 Rect안쪽, wPosX + pixelCount는 Rect바깥쪽
					    이 경우, 시작좌표는 wPosX, pixelCount는 (Rect.Right - wPosX)

			Case5)	wPosX, wPosX + pixelCount 둘 다 Rect 바깥쪽일 시 Render할 필요가 없음

			이러면 Rect안에서 그려질 localRenderStartX, pixelCount를 구해낸 것
			이를 기반으로 screenRenderX = screenX + localRenderStartX
			이런식으로 두면 될 거 같음

			이후 기존 screenRenderX가 화면 좌측경계, 우측경계를 벗어낫을 시 처리도 수행하면 될 거 같음

			추가로 Rect기준 Local 좌표계로 생각했을 때 (Rect.left를 빼면됨)
			즉 (srcStart.x - srcRect.left) 값이 양수인 경우 화면경계 좌측에서 clipping이 일어난 것이므로 이에 대한 처리도 추가함
			
			*/

			
			// Rect 기준 Local 좌표계로 변경해서 생각하기 (srcRect.left를 빼주는 것)
			// 이 때 (srcStart.x - srcRect.left) 가 양수이면 clipping이 일어난 것이므로, 그 부분을 고려하여 localX와 frameWidth를 조절
			// localX = (wPosX - srcRect.left) - (srcStart.x - srcRect.left) = wPosX - srcStart.x
			int localX = static_cast<int>(pStream->wPosX) - srcStart.x;			// 위의 수식에 의해 srcRect.left는 소거됨
			frameWidth = srcRect.right - srcStart.x;
			

			// Case1, Case5: srcRect 범위를 벗어난 경우
			if (localX + pixelCount <= 0 || localX >= frameWidth)
				continue;

			
			// Case2: 왼쪽이 벗어난 경우
			if (localX < 0) {
				pixelCount += localX; // localX는 음수
				localX = 0;
			}

			// Case4: 오른쪽을 벗어난 경우
			if (localX + pixelCount > frameWidth) {
				pixelCount = frameWidth - localX;
			}


			// 최종적으로 화면상의 x 좌표 = destStart.x + localX
			int drawX = destStart.x + localX;


			// 화면 좌측 경계를 벗어나면 조정
			
			if (drawX < 0)
			{
				pixelCount += drawX;
				drawX = 0;
			}
			

			// 화면 우측 경계를 벗어나면 조정
			if (drawX + pixelCount > screenWidth)
			{
				pixelCount = screenWidth - drawX;
			}


			// 대상 버퍼 내 해당 픽셀 위치 계산 (픽셀당 4Byte)
			char* destPixelPtr = destLinePtr + (drawX * 4);

			// 해당 스트림의 픽셀들을 복사
			for (int x = 0; x < pixelCount; x++)
			{
				*(DWORD*)destPixelPtr = pixelColor;
				destPixelPtr += 4;
			}
		}
		lineDesc++; // 다음 행의 압축 데이터
		destLinePtr += m_dwWriteBufferPitch; // 대상 버퍼의 다음 행 포인터
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

    // 화면의 너비 (출력 대상)
    int screenWidth = static_cast<int>(m_dwWidth);

    // 스프라이트 프레임의 크기 (srcRect 기준)
    int frameWidth = srcRect.right - srcRect.left;
    int frameHeight = srcRect.bottom - srcRect.top;

    // 렌더링할 소스 시작 좌표 (스프라이트 시트 로컬 좌표계)
    INT_VECTOR2 srcStart = { srcRect.left, srcRect.top };

    // 화면상의 시작 좌표 (스크린 좌표계)
    INT_VECTOR2 destStart = { screenX, screenY };

    // 복사할 영역의 크기는 스프라이트 프레임의 크기로 초기화
    INT_VECTOR2 spriteSize = { frameWidth, frameHeight };
    INT_VECTOR2 clippedSize = {};

    // 대상 위치와 버퍼 크기
    INT_VECTOR2 destPos = { screenX, screenY };
    INT_VECTOR2 bufferSize = { screenWidth, static_cast<int>(m_dwHeight) };

    // CalcClipArea() 함수는 destStart와 spriteSize를 기반으로 실제 화면에 그려질 영역(클립된 영역)을 계산
    if (!CalcSpriteClipArea(&srcStart, &destStart, &clippedSize, srcRect, &destPos, &bufferSize))
        goto lb_return;

    // pImgData에서 srcStart.y 행의 압축 데이터를 가져옴
    const COMPRESSED_LINE* lineDesc = pImgData->GetCompressedImage(srcStart.y);

    // 대상 버퍼의 첫 행 포인터 (destStart.y 행부터 시작)
    char* destLinePtr = m_pWriteBuffer + (destStart.y) * m_dwWriteBufferPitch;

    // 클립된 영역의 높이만큼 반복
    for (int y = 0; y < clippedSize.y; y++)
    {
        // 각 행의 압축된 스트림 데이터를 순회
        for (DWORD i = 0; i < lineDesc->dwStreamNum; i++)
        {
            PIXEL_STREAM* pStream = lineDesc->pPixelStream + i;
            DWORD pixelColor = pStream->dwPixel;
            int pixelCount = static_cast<int>(pStream->wPixelNum);

			// Rect 좌상단을 기준으로 하는 로컬좌표계르 기준으로 생각해야 로직짜기가 편함
			// 따라서 srcRect.left를 빼줘야함
			int localX = static_cast<int>(pStream->wPosX) - srcRect.left;
			// frameWidth는 srcRect의 너비
			frameWidth = srcRect.right - srcRect.left;

            // 현재 스트림이 srcRect 범위를 벗어나면 건너뜀
            if (localX + pixelCount <= 0 || localX >= frameWidth)
                continue;

            // Case2: 왼쪽을 벗어난 경우
            if (localX < 0) {
                pixelCount += localX; // localX는 음수
                localX = 0;
            }

            // Case4: 오른쪽을 벗어난 경우
            if (localX + pixelCount > frameWidth) {
                pixelCount = frameWidth - localX;
            }

            // 좌우 반전 처리:
            // 원래 좌우 복사는 destStart.x + localX로 계산하지만,
            // 반전된 경우엔 해당 프레임의 오른쪽 끝에서부터 localX만큼 떨어진 위치로 복사하고,
            // pixelCount만큼 픽셀을 채우므로,
            // drawX = destStart.x + (frameWidth - localX - pixelCount)
            int drawX = destStart.x + (frameWidth - localX - pixelCount);
			
			// 이번에는 여기서 최종 좌표위치를 왼쪽에서 Clipping 된 만큼 옮겨줘야함
			// (srcStart.x - srcRect.left) 가 양수라면 왼쪽에서 Clippiing이 일어난 상황

			drawX -= (srcStart.x - srcRect.left);

            // 화면 좌측 경계 클리핑
            if (drawX < 0)
            {
                pixelCount += drawX;
                drawX = 0;
            }

            // 화면 우측 경계 클리핑
            if (drawX + pixelCount > screenWidth)
            {
                pixelCount = screenWidth - drawX;
            }

            // 대상 버퍼 내 해당 픽셀 위치 계산 (픽셀당 4Byte)
            // (반전된 이미지는 오른쪽에서부터 채워지므로, 여기서는 일반적인 순서로 복사해도 동일한 색상 반복 복사 시 문제없음)
            char* destPixelPtr = destLinePtr + (drawX * 4);

            // 동일 색상의 픽셀들을 pixelCount만큼 복사
            for (int x = 0; x < pixelCount; x++)
            {
                *(DWORD*)destPixelPtr = pixelColor;
                destPixelPtr += 4;
            }
        }
        lineDesc++; // 다음 행의 압축 데이터
        destLinePtr += m_dwWriteBufferPitch; // 대상 버퍼의 다음 행 포인터
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

	// texRectSide는 텍스트의 외곽선효과
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

	// 먼저 텍스트의 외곽선을 검정색으로 그림
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
