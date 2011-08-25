/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
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

public:
	CFrameBuffer(void);
	~CFrameBuffer(void);

public:
	void AddPlane(DWORD dwSize, DWORD dwMarginSize);

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
