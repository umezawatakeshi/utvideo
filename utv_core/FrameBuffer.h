/* ï∂éöÉRÅ[ÉhÇÕÇrÇiÇhÇr â¸çsÉRÅ[ÉhÇÕÇbÇqÇkÇe */
/* $Id$ */

#pragma once

class CFrameBuffer
{
private:
	static const int MAX_PLANE = 4;

private:
	int m_nPlanes;
	uint8_t *m_pAllocatedAddr[MAX_PLANE];
	uint8_t *m_pBufferAddr[MAX_PLANE];
	size_t m_cbAllocated[MAX_PLANE];
	size_t m_cbBuffer[MAX_PLANE];
	size_t m_cbMargin[MAX_PLANE];

public:
	CFrameBuffer(void);
	~CFrameBuffer(void);

public:
	void AddPlane(size_t cbBuffer, size_t cbMargin);
	void ModifyPlane(int idx, size_t cbBuffer, size_t cbMargin);
	void ReleasePlane(int idx);

private:
	void SetPlane(int idx, size_t cbBuffer, size_t cbMargin);

public:
	inline uint8_t *GetPlane(int n)
	{
		return m_pBufferAddr[n];
	}

	inline uint8_t *GetBuffer(void)
	{
		return GetPlane(0);
	}
};
