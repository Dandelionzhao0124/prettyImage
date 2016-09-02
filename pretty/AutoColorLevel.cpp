#include "AutoColorLevel.h" 
#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CHANNEL 3

unsigned char* AutoColorLevel(unsigned char* srcData, int height, int width, int stride) {

	int m_inputMinBlue = 256, m_inputMinGreen = 256, m_inputMinRed = 256;
	int m_inputMaxBlue = -1, m_inputMaxGreen = -1, m_inputMaxRed = -1;
	int i, j;
	int m_outMinBlue = 0, m_outMinGreen = 0, m_outMinRed = 0;
	int m_outMaxBlue = 255, m_outMaxGreen = 255, m_outMaxRed = 255;

	int offset = stride - width * CHANNEL;
	int len = width * height;
	unsigned char* pSrc = srcData;

	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {
			m_inputMinBlue = (pSrc[0] < m_inputMinBlue) ? pSrc[0] : m_inputMinBlue;
			m_inputMinGreen = (pSrc[1] < m_inputMinGreen) ? pSrc[1] : m_inputMinGreen;
			m_inputMinRed = (pSrc[2] < m_inputMinRed) ? pSrc[2] : m_inputMinRed;

			m_inputMaxBlue = (pSrc[0] > m_inputMaxBlue) ? pSrc[0] : m_inputMaxBlue;
			m_inputMaxGreen = (pSrc[1] > m_inputMaxGreen) ? pSrc[1] : m_inputMaxGreen;
			m_inputMaxRed = (pSrc[2] > m_inputMaxRed) ? pSrc[2] : m_inputMaxRed;

			pSrc += CHANNEL;		
		}
		pSrc += offset;
	}
	m_inputMinBlue = (m_inputMinBlue > 127) ? 127 : m_inputMinBlue;
	m_inputMinGreen = (m_inputMinGreen > 127) ? 127 : m_inputMinGreen;
	m_inputMinRed = (m_inputMinRed > 127) ? 127 : m_inputMinRed;

	m_inputMaxBlue = (m_inputMaxBlue < 128) ? 128 : m_inputMaxBlue;
	m_inputMaxGreen = (m_inputMaxGreen < 128) ? 128 : m_inputMaxGreen;
	m_inputMaxRed = (m_inputMaxRed < 128) ? 128 : m_inputMaxRed;

	pSrc -= height*stride;

	for (j = 0; j < height; ++j) {
		for (i = 0; i < width; ++i) {

			pSrc[0] = m_outMinBlue + (unsigned char)floor((double)(pSrc[0] - m_inputMinBlue) / (double)(m_inputMaxBlue - m_inputMinBlue)*(double)(m_outMaxBlue - m_outMinBlue) + 0.5);
			pSrc[1] = m_outMinGreen + (unsigned char)floor((double)(pSrc[1] - m_inputMinGreen) / (double)(m_inputMaxGreen - m_inputMinGreen)*(double)(m_outMaxGreen - m_outMinGreen) + 0.5);
			pSrc[2] = m_outMinRed + (unsigned char)floor((double)(pSrc[2] - m_inputMinRed) / (double)(m_inputMaxRed - m_inputMinRed)*(double)(m_outMaxRed - m_outMinRed) + 0.5);

			pSrc += CHANNEL;
		}
		pSrc += offset;
	}
	return srcData;

}