#ifndef DYPC_CANVAS_H_
#define DYPC_CANVAS_H_

#include <cassert>
#include <utility>
#include <cstdint>

namespace dypc {

template<class Value, std::size_t Width, std::size_t Height>
class canvas {
public:
	struct pixel {
		std::ptrdiff_t x;
		std::ptrdiff_t y;
		
		static pixel invalid() { return { PTRDIFF_MIN, 0 }; }
		bool is_invalid() const { return (x == PTRDIFF_MIN); }
	};
	
private:
	Value canvas_[Height][Width];

	void fill_bottom_flat_triangle_(Value v, pixel top, pixel bottom1, pixel bottom2);
	void fill_top_flat_triangle_(Value v, pixel bottom, pixel top1, pixel top2);

public:
	bool in_range(pixel p) const { return p.x >= 0 && p.x < Width && p.y >= 0 && p.y < Height; }
	Value operator[](pixel p) const { assert(in_range(p)); return canvas_[p.y][p.x]; }
	Value& operator[](pixel p) { assert(in_range(p)); return canvas_[p.y][p.x]; }

	void clear(Value v);
	void draw_horizontal_line(Value v, pixel origin, std::ptrdiff_t length);
	void fill_rectangle(Value v, pixel origin, std::ptrdiff_t w, std::ptrdiff_t h);
	void fill_triangle(Value v, pixel a, pixel b, pixel c);
	
	void print() const;
};



template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::clear(Value v) {
	Value* c = (Value*)canvas_;
	Value* c_end = c + (Width * Height);
	while(c != c_end) *(c++) = v;
}


template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::draw_horizontal_line(Value v, pixel origin, std::ptrdiff_t length) {
	assert(length >= 0);
	
	if(origin.y < 0 || origin.y >= Height || origin.x >= Width) {
		return;
	} else if(origin.x < 0) {
		length += origin.x; origin.x = 0;
		if(length <= 0) return;
	}
	if(origin.x + length >= Width) length = Width - origin.x - 1;
	
	Value* start = canvas_[origin.y] + origin.x;
	Value* end = start + length;
	for(Value* c = start; c != end; ++c) *c = v;
}



template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::fill_rectangle(Value v, pixel origin, std::ptrdiff_t w, std::ptrdiff_t h) {
	assert(w >= 0 && h >= 0);
	
	if(origin.x < 0) origin.x = 0;
	else if(origin.x >= Width) origin.x = Width - 1;
	if(origin.y < 0) origin.y = 0;
	else if(origin.y >= Height) origin.y = Height - 1;
	
	pixel extremity = { origin.x + w, origin.y + h };
	if(extremity.x >= Width) extremity.x = Width - 1;
	if(extremity.y >= Height) extremity.y = Height - 1;
	
	for(std::ptrdiff_t x = origin.x; x != extremity.x; ++x) {
		for(std::ptrdiff_t y = origin.y; y != extremity.y; ++y) {
			canvas_[y][x] = v;
		}
	}
}


template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::fill_triangle(Value v, pixel a, pixel b, pixel c) {
	if(a.is_invalid() || b.is_invalid() || c.is_invalid()) return;
	
	// Sort 3 pixels by ascending y
	if(a.y > b.y) std::swap(a, b);
	if(b.y > c.y) std::swap(b, c);
	if(a.y > b.y) std::swap(a, b);
	
	if(b.y == c.y) {
		fill_bottom_flat_triangle_(v, a, b, c);
	} else if(a.y == b.y) {
		fill_top_flat_triangle_(v, c, a, b);
	} else {
		pixel d;
		d.x = a.x + ( float(b.y - a.y) / float(c.y - a.y) ) * (c.x - a.x),
		d.y = b.y;

		fill_bottom_flat_triangle_(v, a, b, d);
		fill_top_flat_triangle_(v, c, b, d);
	}
}


template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::fill_bottom_flat_triangle_(Value v, pixel top, pixel bottom1, pixel bottom2) {
	assert(top.y <= bottom1.y);
	assert(bottom1.y == bottom2.y);
	if(bottom2.x < bottom1.x) std::swap(bottom1, bottom2);
	
	float inv_slope_1 = float(bottom1.x - top.x) / float(bottom1.y - top.y);
	float inv_slope_2 = float(bottom2.x - top.x) / float(bottom2.y - top.y);
	
	float x_1 = top.x, x_2 = top.x;
	
	for(std::ptrdiff_t y = top.y; y <= bottom1.y; ++y) {
		std::ptrdiff_t length = x_2 - x_1;
		std::ptrdiff_t x = x_1;
		draw_horizontal_line(v, {x, y}, length);
		
		x_1 += inv_slope_1;
		x_2 += inv_slope_2;
		
		//std::cout << "len=" << length << " x12=(" << x_1 << ", " << x_2 << ") slp=(" << inv_slope_1 << "; " << inv_slope_2 << ")" << std::endl;
	}
}


template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::fill_top_flat_triangle_(Value v, pixel bottom, pixel top1, pixel top2) {
	assert(bottom.y >= top1.y);
	assert(top1.y == top2.y);
	if(top2.x < top1.x) std::swap(top1, top2);
	
	float inv_slope_1 = float(bottom.x - top1.x) / float(bottom.y - top1.y);
	float inv_slope_2 = float(bottom.x - top2.x) / float(bottom.y - top2.y);
	
	float x_1 = bottom.x, x_2 = bottom.x;
	
	for(std::ptrdiff_t y = bottom.y; y > top1.y; --y) {
		x_1 -= inv_slope_1;
		x_2 -= inv_slope_2;
		
		std::ptrdiff_t length = x_2 - x_1;
		std::ptrdiff_t x = x_1;
		draw_horizontal_line(v, {x, y}, length);
	}
}


template<class Value, std::size_t Width, std::size_t Height>
void canvas<Value, Width, Height>::print() const {
	for(std::ptrdiff_t y = 0; y != Height; ++y) {
		for(std::ptrdiff_t x = 0; x != Width; ++x) {
			std::cout << (canvas_[y][x] ? '#' : ' ');
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------" << std::endl;
}

	
}

#endif
