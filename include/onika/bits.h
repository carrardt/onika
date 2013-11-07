#ifndef __onika_bits_h
#define __onika_bits_h

namespace onika {

	template<typename Word>
	static inline Word mask1lsb(unsigned int n)
	{
		Word m = 1;
		m = ( m<<(n-1) ) - 1;
		m = (m<<1) | static_cast<Word>(1);
		m = ( n>0 ) ? m : 0;
		return m;
	}
}



// ======================== UNIT TEST =======================
#endif // end of header file

