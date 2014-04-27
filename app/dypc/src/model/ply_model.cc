#include <stdexcept>
#include <utility>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "ply_model.h"
#include "../progress.h"

namespace dypc {

static bool check_host_little_endian_() {
	unsigned int i = 1;
	char* c = reinterpret_cast<char*>(&i);
	return *c;
}
	
const bool ply_model::host_is_little_endian_ = check_host_little_endian_();

ply_model::ply_model(const std::string& filename, float scale) :
filename_(filename), scale_(scale) {
	read_header_();
}

void ply_model::open_file_(std::ifstream& file) {
	file.open(filename_, std::ios_base::in | std::ios_base::binary);
	file.exceptions(std::ios_base::failbit);
}


void ply_model::read_header_() {
	std::ifstream file; open_file_(file);
	std::string line;
	
	vertices_offset_ = 0;
	vertices_end_ = 0;
	number_of_points_ = 0;
	has_colors_ = false;
	vertex_length_ = 0;

	std::getline(file, line);
	if(line != "ply") throw std::runtime_error("Not PLY file");
	
	std::getline(file, line);
	if(line == "format binary_little_endian 1.0") little_endian_ = true;
	else if(line == "format binary_big_endian 1.0") little_endian_ = false;
	else throw std::runtime_error("Unsupported format (ASCII not supported, or other)");
	
	enum { before_vertex = 0, in_vertex, after_vertex } state = before_vertex;
	std::size_t number_of_elements = 0;
	for(;;) {
		std::getline(file, line);
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
			vertices_offset_ += file.tellg();
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


bool ply_model::read_point_(std::ifstream& file, point& pt) {
	if(file.tellg() >= vertices_end_) return false;
	
	unsigned char buffer[vertex_buffer_length_];
	file.read((char*)buffer, vertex_length_);
	
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


ply_model::handle::handle(ply_model& mod) : model_(mod) {
	model_.open_file_(file_);
	file_.seekg(model_.vertices_offset_);
}

bool ply_model::handle::read(point& pt) {
	return model_.read_point_(file_, pt);
}

std::unique_ptr<model::handle> ply_model::handle::clone() {
	return model_.make_handle_();
}


}
