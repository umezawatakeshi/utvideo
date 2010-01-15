/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

class CFrameBuffer
{
private:
	static const int MAX_PLANE = 4;

private:
	int m_nPlanes;
	BYTE *m_pAllocatedAddr[MAX_PLANE];
	BYTE *m_pBufferAddr[MAX_PLANE];

public:
	CFrameBuffer(void);
	~CFrameBuffer(void);

public:
	void AddPlane(DWORD dwSize, DWORD dwMarginSize);

public:
	inline BYTE *GetPlane(int n)
	{
		return m_pBufferAddr[n];
	}

	inline BYTE *GetBuffer(void)
	{
		return GetPlane(0);
	}
};
