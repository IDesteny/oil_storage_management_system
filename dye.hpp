#ifndef __DYE_HPP__
#define __DYE_HPP__

#include <iomanip>

class dye
{
public:
	enum code
	{
		red = 31,
		green = 32,
		white = 39,
		gray = 90,
		yellow = 93,
	};

private:
	code now_color;
	
public:
	explicit dye(code color = white): now_color(color)
	{}

	std::string forever(code color)
	{
		now_color = color;
		std::stringstream dyestuff;
		dyestuff << "\033[" << color << 'm';
		return dyestuff.str();
	}
	
	template <typename T>
	std::string colorant(T &&str, code color) const
	{
		std::stringstream colorful_string;
		colorful_string << "\033[" << color << 'm' << std::forward<T>(str)
					<< "\033[" << now_color << 'm';
		return colorful_string.str();
	}	
};

#endif // !__DYE_HPP__
