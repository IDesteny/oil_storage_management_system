#ifndef __OIL_PRODUCT_HPP__
#define __OIL_PRODUCT_HPP__

class oil_product
{
private:
	uint64_t content_volume = 0;
	uint64_t capacity;

public:
	oil_product(uint64_t capacity = 0): capacity(capacity) {}

	void set_content_volume(uint64_t volume) noexcept
	{
		content_volume = volume;
	}

	void set_capacity(uint64_t cap) noexcept
	{
		capacity = cap;
	}

	[[nodiscard]] uint64_t get_content_volume() const noexcept
	{
		return content_volume;
	}

	[[nodiscard]] uint64_t get_capacity() const noexcept
	{
		return capacity;
	}
};

#endif // !__OIL_PRODUCT_HPP__
