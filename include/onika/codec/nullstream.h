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
#ifdef onika_codec_nullstream_TEST
int main()
{
	onika::codec::NullStream ns;
	ns<<"This number "<<65<<" will neve be printed\n";
	return 0;
}

#endif

