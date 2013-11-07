#ifndef __onika_nullstream_h
#define __onika_nullstream_h

namespace onika { namespace codec {

	struct NullStream
	{
		template<typename T>
		inline NullStream& operator << (const T&) { return *this; }
		inline NullStream debug() { return *this; }
		private:
		int x;
	};

} } // namespace

#endif


//=============== UNIT TEST ================
#endif

