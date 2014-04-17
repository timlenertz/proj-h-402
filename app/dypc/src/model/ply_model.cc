#include <stdexcept>
#include <utility>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "ply_model.h"
#include "../progress.h"

namespace dypc {

static bool check_host_little_endian() {
	unsigned int i = 1;
	char* c = reinterpret_cast<char*>(&i);
	return *c;
}
	
const bool ply_model::host_is_little_endian_ = check_host_little_endian();

ply_model::ply_model(const std::string& filename, float scale) :
filename_(filename), file_(filename, std::ios_base::in | std::ios_base::binary), scale_(scale) {
	if(file_.fail()) throw std::runtime_error("Could not open PLY file");
	file_.exceptions(std::ios_base::failbit);
	read_header_();
	rewind();
}

ply_model::ply_model(const ply_model& mod) :
filename_(mod.filename_),
file_(mod.filename_, std::ios_base::in | std::ios_base::binary),
scale_(mod.scale_),
little_endian_(mod.little_endian_),
vertices_offset_(mod.vertices_offset_),
vertices_end_(mod.vertices_end_),
vertex_length_(mod.vertex_length_),
has_colors_(mod.has_colors_),
x_(mod.x_), y_(mod.y_), z_(mod.z_),
r_(mod.r_), g_(mod.g_), b_(mod.b_) {
	auto pos = const_cast<std::ifstream&>(mod.file_).tellg();
	file_.seekg(pos);
}

void ply_model::compute_bounds_() {
	rewind();
	point pt; next_point(pt);
	minimum_ = maximum_ = pt;
	
	progress_foreach(*this, "Finding bounds of PLY model", [&](const point& pt) {
		if(pt.x < minimum_[0]) minimum_[0] = pt.x;
		else if(pt.x > maximum_[0]) maximum_[0] = pt.x;
		
		if(pt.y < minimum_[1]) minimum_[1] = pt.y;
		else if(pt.y > maximum_[1]) maximum_[1] = pt.y;

		if(pt.z < minimum_[2]) minimum_[2] = pt.z;
		else if(pt.z > maximum_[2]) maximum_[2] = pt.z;
	});
}

void ply_model::read_header_() {
	std::string line;

	vertices_offset_ = 0;
	vertices_end_ = 0;
	number_of_points_ = 0;
	has_colors_ = false;
	vertex_length_ = 0;

	file_.seekg(0);
	std::getline(file_, line);
	if(line != "ply") throw std::runtime_error("Not PLY file");
	
	std::getline(file_, line);
	if(line == "format binary_little_endian 1.0") little_endian_ = true;
	else if(line == "format binary_big_endian 1.0") little_endian_ = false;
	else throw std::runtime_error("Unsupported format (ASCII not supported, or other)");
	
	enum { before_vertex = 0, in_vertex, after_vertex } state = before_vertex;
	std::size_t number_of_elements = 0;
	for(;;) {
		std::getline(file_, line);
		if(line.substr(0, 8) == "comment ") continue;
				
		if(line.substr(0, 8) == "element ") {
			auto pos = line.find(' ', 8);
			std::string element = line.substr(8, pos-8);
			number_of_elements = stoul(line.substr(pos + 1));
			if(element == "vertex") {
				if(state != before_vertex) throw std::runtime_error("Two vertex element headers in file");
				state = in_vertex;
				number_of_points_ = number_of_elements;
			} else if(state == in_vertex) {
				state = after_vertex;
			}
						
		} else if(line.substr(0, 10) == "end_header") {
			if(state != in_vertex && state != after_vertex) throw std::runtime_error("No vertex element in PLY header");
			vertices_offset_ += file_.tellg();
			break;
		
		} else if(line.substr(0, 9) == "property ") {
			if(state == after_vertex) continue;
			
			auto pos = line.find(' ', 9);
			std::string type = line.substr(9, pos-9);
			std::string name = line.substr(pos + 1);
			
			std::size_t length;
			if(type == "char") length = 1;
			else if(type == "uchar") length = 1;
			else if(type == "short") length = 2;
			else if(type == "ushort") length = 2;
			else if(type == "int") length = 4;
			else if(type == "uint") length = 4;
			else if(type == "float") length = 4;
			else if(type == "double") length = 8;
			else if(type == "list") throw std::runtime_error("No support for 'list' property type");
			else throw std::runtime_error("Unknown property type");
			
			if(state == in_vertex) {
				if(name == "x") {
					if(type != "float") throw std::runtime_error("X property must be float");
					x_ = vertex_length_;
				} else if(name == "y") {
					if(type != "float") throw std::runtime_error("Y property must be float");
					y_ = vertex_length_;
				} else if(name == "z") {
					if(type != "float") throw std::runtime_error("Z property must be float");
					z_ = vertex_length_;
				} else if(name == "r" || name == "red") {
					if(type != "uchar") throw std::runtime_error("Red property must be uchar");
					has_colors_ = true;
					r_ = vertex_length_;
				} else if(name == "g" || name == "green") {
					if(type != "uchar") throw std::runtime_error("Green property must be uchar");
					has_colors_ = true;
					g_ = vertex_length_;
				} else if(name == "b" || name == "blue") {
					if(type != "uchar") throw std::runtime_error("Blue property must be uchar");
					has_colors_ = true;
					b_ = vertex_length_;
				}
								
				vertex_length_ += length;
			} else if(state == before_vertex) {
				vertices_offset_ += number_of_elements * length;
			}
		}
	}
	
	vertices_end_ = vertices_offset_ + number_of_points_ * vertex_length_;
	
	if(vertex_length_ > vertex_buffer_length_) throw std::runtime_error("Vertex element too long");
}

void ply_model::rewind() {
	file_.seekg(vertices_offset_);
}

float ply_model::extract_flipped_endianness_float_(const unsigned char* buf) const {
	unsigned char out[4];
	out[0] = buf[3];
	out[1] = buf[2];
	out[2] = buf[1];
	out[3] = buf[0];
	return *reinterpret_cast<float*>(out);
}

bool ply_model::next_point(point& pt) {
	if(file_.tellg() >= vertices_end_) return false;
	
	unsigned char buffer[vertex_buffer_length_];
	file_.read((char*)buffer, vertex_length_);
	
	if(host_is_little_endian_ == little_endian_) {
		pt.x = *reinterpret_cast<float*>(buffer + x_) * scale_;
		pt.y = *reinterpret_cast<float*>(buffer + y_) * scale_;
		pt.z = *reinterpret_cast<float*>(buffer + z_) * scale_;
	} else {
		pt.x = extract_flipped_endianness_float_(buffer + x_) * scale_;
		pt.y = extract_flipped_endianness_float_(buffer + y_) * scale_;
		pt.z = extract_flipped_endianness_float_(buffer + z_) * scale_;
	}
	
	if(has_colors_) {
		pt.r = *reinterpret_cast<unsigned char*>(buffer + r_);
		pt.g = *reinterpret_cast<unsigned char*>(buffer + g_);
		pt.b = *reinterpret_cast<unsigned char*>(buffer + b_);
	} else {
		pt.r = pt.g = pt.b = 255;
	}
	
	return true;
}

}
