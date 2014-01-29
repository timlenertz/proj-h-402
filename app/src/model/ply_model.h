#ifndef DYPC_PLY_MODEL_H_
#define DYPC_PLY_MODEL_H_

#include <string>
#include <fstream>
#include "model.h"

namespace dypc {

class ply_model : public model {
private:
	static constexpr std::size_t vertex_buffer_length_ = 256;
	static const bool host_is_little_endian_;

	std::string filename_;
	std::ifstream file_;
	const float scale_;
	
	bool little_endian_;
	std::size_t vertices_offset_;
	std::size_t vertices_end_;
	std::size_t vertex_length_;
	bool has_colors_;
	std::ptrdiff_t x_;
	std::ptrdiff_t y_;
	std::ptrdiff_t z_;
	std::ptrdiff_t r_;
	std::ptrdiff_t g_;
	std::ptrdiff_t b_;
	
	void read_header_();
	void find_limits_();
	
	float extract_flipped_endianness_float_(const unsigned char*) const;
	
public:
	ply_model(const std::string& filename, float scale);
	ply_model(const ply_model&);
	
	void rewind() override;
	bool next_point(point&) override;
};

}

#endif
