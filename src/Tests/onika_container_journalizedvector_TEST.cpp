#include "onika/container/journalizedvector.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "onika/container/iterator.h"
#include "onika/container/sequence.h"
#include "onika/container/algorithm.h"

typedef onika::container::SequenceContainer<uint32_t> Sequence;

typedef onika::container::JournalizedVector<
		Sequence,
		std::vector<int64_t>,
		std::vector<short> >
	JVector;

static inline void printjvec(const JVector& jvec)
{
	for( auto i : jvec )
	{
		std::cout<<i<<' ';
	}
	std::cout<<" size="<<onika::container::memory_bytes(jvec)<<"\n";
}


int main(int argc, char* argv[])
{
	Sequence serie(10);
	JVector jvec(serie);

	std::cout<<"initial values\n";
	printjvec(jvec);

	std::cout<<"write @66 <- 7\n";
	jvec[7] = 66;
	printjvec(jvec);

	std::cout<<"push_back 101-106\n";
	jvec.push_back( 101 );
	printjvec(jvec);
	jvec.push_back( 102 );
	jvec.push_back( 103 );
	jvec.push_back( 104 );
	jvec.push_back( 105 );
	jvec.push_back( 106 );
	printjvec(jvec);

	std::cout<<"erase @12\n";
	jvec.erase( jvec.begin()+12 );
	printjvec(jvec);

	std::cout<<"std::swap(jvec[5],jvec[11])\n";
	std::swap( jvec[5], jvec[11] );
	printjvec(jvec);

	std::cout<<"std::iter_swap(@6,@10)\n";
	std::iter_swap( jvec.begin()+6, jvec.begin()+10 );
	printjvec(jvec);

	std::cout<<"push_back 107\n";
	jvec.push_back( 107 );
	printjvec(jvec);

	std::cout<<"erase @12\n";
	jvec.erase( 12 );
	printjvec(jvec);

	std::cout<<"push_back 108\n";
	jvec.push_back( 108 );
	printjvec(jvec);

	std::cout<<"sort (rev. comp)\n";
	std::sort( jvec.begin(), jvec.end(), [](uint32_t a, uint32_t b) -> bool { return b<a; } );
	printjvec(jvec);

	std::cout<<"sort\n";
	std::sort( jvec.begin(), jvec.end() );
	printjvec(jvec);

	std::cout<<"strip\n";
	jvec.strip();
	printjvec(jvec);

	std::cout<<"make_heap\n";
	std::make_heap( jvec.begin(), jvec.end() );
	printjvec(jvec);

	std::cout<<"sort_heap\n";
	std::sort_heap( jvec.begin(), jvec.end() );
	printjvec(jvec);

	return 0;
}

 // end of journalizedvector.h

