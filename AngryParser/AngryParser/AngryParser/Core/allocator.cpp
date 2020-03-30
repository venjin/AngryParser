#include "allocator.h"

NS_BEGINE
inline namespace Core
{

	Allocator::~Allocator()
	{
		clear();
	}

	void* Allocator::allocate(std::size_t size)
	{
		assert(size);
		if (!lastBlock || (lastBlock->size - lastBlock->free) < size)
			allocateBlock(size > (S - sizeof(Block)) ? size : (S - sizeof(Block)));
		void* p = reinterpret_cast<char*>(lastBlock + 1) + lastBlock->free;
		lastBlock->free += size;
		return p;
	}

	void Allocator::deallocate(void* data, std::size_t size) noexcept
	{
		std::free(data);
	}

	void Allocator::clear()
	{
		for (auto p = firstBlock; p; ) { auto next = p->next; deallocate(p, sizeof(Block) + p->size); p = next; }
		firstBlock = lastBlock = nullptr;
	}

	void Allocator::allocateBlock(std::size_t size)
	{
		auto block = static_cast<Block*>(std::malloc(sizeof(Block) + size));
		block->next = nullptr;
		block->size = size;
		block->free = 0;
		if (lastBlock) lastBlock->next = block, lastBlock = block;
		else firstBlock = lastBlock = block;
	}

}
NS_END