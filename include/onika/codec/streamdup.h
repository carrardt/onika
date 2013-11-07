#ifndef __onika_codec_streamdup_h
#define __onika_codec_streamdup_h


namespace onika { namespace codec {

	template<typename StreamT1, typename StreamT2>
	struct StreamDup
	{
		inline StreamDup(StreamT1& s1, StreamT2& s2) : stream1(s1), stream2(s2) {}

		template<typename T>
		inline StreamDup& operator << (const T& x)
		{
			stream1 << x;
			stream2 << x;
			return *this;
		}

		inline void flush() { stream1.flush(); stream2.flush(); }

		StreamT1& stream1;
		StreamT2& stream2;
	};

	template<typename StreamT1, typename StreamT2>
	inline StreamDup<StreamT1,StreamT2> streamdup(StreamT1& s1, StreamT2& s2) { return StreamDup<StreamT1,StreamT2>(s1,s2); }

} }

#endif


// ====================================================
// =============== UNIT TEST ==========================
// ====================================================
#endif


