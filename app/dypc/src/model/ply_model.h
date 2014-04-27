#ifndef DYPC_PLY_MODEL_H_
#define DYPC_PLY_MODEL_H_

#include <string>
#include <fstream>
#include "model.h"

namespace dypc {

class ply_model : public model {
private:
	class handle : public model::handle {
	private:
		ply_model& model_;
		std::ifstream file_;
	
	public:
		explicit handle(ply_model&);
	
		~handle() override { }
		bool read(point&) override;
		std::unique_ptr<model::handle> clone() override;
	};
	
	static constexpr std::size_t vertex_buffer_length_ = 256;
	static const bool host_is_little_endian_;

	std::string filename_;
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
	

	void open_file_(std::ifstream&);
	void read_header_();
	bool read_point_(std::ifstream&, point&);

	static float extract_flipped_endianness_float_(const unsigned char* buf) {
		unsigned char out[4];
		out[0] = buf[3];
		out[1] = buf[2];
		out[2] = buf[1];
		out[3] = buf[0];
		return *reinterpret_cast<float*>(out);
	}

protected:
	std::unique_ptr<model::handle> make_handle_() override {
		return std::unique_ptr<model::handle>(new handle(*this));
	}
	
public:
	ply_model(const std::string& filename, float scale);
};

}

#endif
