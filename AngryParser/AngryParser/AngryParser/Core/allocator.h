#ifndef _ALLOCATOR_H

#include <cassert>
#include <cstdlib>

#include "compilerdetection.h"

NS_BEGINE
inline namespace Core
{
	class AngryParser_API Allocator
	{
	public:

		Allocator() = default;
		Allocator(const Allocator& src) = delete;
		~Allocator();

		void* allocate(std::size_t size);

		void deallocate(void* data, std::size_t size) noexcept;

		void clear();

	private:
		void allocateBlock(std::size_t size);

	private:

		struct Block {

			Block* next;
			std::size_t size;
			std::size_t free;

		};

		const std::size_t S = 65536;
		Block* firstBlock;
		Block* lastBlock;
	};

}
NS_END

#endif
