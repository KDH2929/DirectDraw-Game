#pragma once


#pragma pack(push,4)
struct PIXEL_STREAM
{
	WORD	wPosX;
	WORD	wPixelNum;
	DWORD	dwPixel;
};
#pragma pack(pop)
struct COMPRESSED_LINE
{
	DWORD	dwStreamNum;
	DWORD	dwLineDataSize;

	// union을 사용하면 두 멤버가 같은 메모리 공간을 공유하게 되어, 별도로 각각 저장할 경우보다 메모리 사용을 줄일 수 있음
	// 아래 둘 중 하나의 용도로 쓰겠다는 의미
	// union의 크기는 멤버 중에서 가장 큰 크기를 가진 요소의 크기로 결정

	union
	{
		struct
		{
			PIXEL_STREAM*	pPixelStream;
		};
		struct
		{
			DWORD	dwOffset;
		};
	};
};
class CImageData
{
	DWORD	m_dwWidth = 0;
	DWORD	m_dwHeight = 0;
	COMPRESSED_LINE*	m_pCompressedImage = nullptr;
	DWORD	CreatePerLine(const char* pDest, int iMaxMemSize, const DWORD* pSrcBits, DWORD dwWidth, DWORD dwColorKey);
public:
	const COMPRESSED_LINE*	GetCompressedImage(int y) const
	{
	#ifdef _DEBUG
		if (y < 0)
			__debugbreak();
		if (y >= (int)m_dwHeight)
			__debugbreak();
	#endif
		return m_pCompressedImage + y; 
	}
	BOOL	Create(const char* pSrcBits, DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey);
	DWORD	GetWidth() const { return m_dwWidth; }
	DWORD	GetHeight() const { return m_dwHeight; }

	CImageData();
	~CImageData();
};