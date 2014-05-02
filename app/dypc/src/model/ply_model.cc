#include <stdexcept>
#include <utility>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "ply_model.h"
#include "../progress.h"

namespace dypc {

/**
 * Check if the host is little endian.
 */
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
	file.exceptions(std::ios_base::failbit); // Throw exception reading beyond end of file.
}


void ply_model::read_header_() {
	std::ifstream file; open_file_(file);
	std::string line;
	
	vertices_offset_ = 0;
	vertices_end_ = 0;
	number_of_points_ = 0;
	has_colors_ = false;
	vertex_length_ = 0;

	// First line must be PLY.
	std::getline(file, line);
	if(line != "ply") throw std::runtime_error("Not PLY file");
	
	// Second line must indicate format.
	std::getline(file, line);
	if(line == "format binary_little_endian 1.0") little_endian_ = true;
	else if(line == "format binary_big_endian 1.0") little_endian_ = false;
	else throw std::runtime_error("Unsupported format (ASCII not supported, or other)");
	
	// Definitions of data elements follow.
	// State before_vertex: Skip definitions until vertex (aka points) definitions start
	//       in_vertex: Reading vertex definitions
	//       after_vertex: Skip other defitions after it, until data is reached.
	enum { before_vertex = 0, in_vertex, after_vertex } state = before_vertex;
	std::size_t number_of_elements = 0;
	for(;;) {
		// Skip comments
		std::getline(file, line);
		if(line.substr(0, 8) == "comment ") continue;
		
		if(line.substr(0, 8) == "element ") { // Read element definition start.
			auto pos = line.find(' ', 8);
			std::string element = line.substr(8, pos-8);
			number_of_elements = stoul(line.substr(pos + 1)); // Get number of elements.
			if(element == "vertex") {
				// Vertex definitions start.
				if(state != before_vertex) throw std::runtime_error("Two vertex element headers in file");
				state = in_vertex;
				number_of_points_ = number_of_elements; // Get number of vertices.
			} else if(state == in_vertex) {
				// Another element definition after vertices.
				state = after_vertex;
			}
	
		} else if(line.substr(0, 10) == "end_header") { // End of header; data follows.
			if(state != in_vertex && state != after_vertex) throw std::runtime_error("No vertex element in PLY header");
			vertices_offset_ += file.tellg(); // Vertices data offset: After header. Length of data before vertices data has already been added
			break;
		
		} else if(line.substr(0, 9) == "property ") { // Reading property definitions for current element.
			if(state == after_vertex) continue; // After vertices: skip
			// If in vertex, capture XYZRGB locations, if before vertex, find total length of data before vertex data.
			
			// Data type and property name
			auto pos = line.find(' ', 9);
			std::string type = line.substr(9, pos-9);
			std::string name = line.substr(pos + 1);
			
			// Determine length of property data.
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
			
			// If vertex element: Find offsets for XYZRGB offsets.
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
								
				vertex_length_ += length; // Length of one vertex data.
			} else if(state == before_vertex) {
				// Accumulate length of non-vertex data before vertex data. Length of header will be added, yielding file offset to vertex data.
				vertices_offset_ += number_of_elements * length;
			}
		}
	}
	
	// Offset to end of vertex data.
	vertices_end_ = vertices_offset_ + number_of_points_ * vertex_length_;
}


std::size_t ply_model::read_points_(std::ifstream& file, point* pts, std::size_t n) {
	auto filepos = file.tellg();
	
	if(filepos >= vertices_end_) return 0;
	
	auto remaining = (vertices_end_ - filepos) / vertex_length_;
	if(n > remaining) n = remaining;
	
	auto len = vertex_length_ * n;
	std::uint8_t buffer[len];
	file.read((char*)buffer, len);
	
	const std::uint8_t* input = buffer;
	for(std::size_t i = 0; i < n; ++i, input += vertex_length_) {
		point& pt = *(pts++);

		if(host_is_little_endian_ == little_endian_) {
			pt.x = *reinterpret_cast<const float*>(input + x_) * scale_;
			pt.y = *reinterpret_cast<const float*>(input + y_) * scale_;
			pt.z = *reinterpret_cast<const float*>(input + z_) * scale_;
		} else {
			pt.x = extract_flipped_endianness_float_(input + x_) * scale_;
			pt.y = extract_flipped_endianness_float_(input + y_) * scale_;
			pt.z = extract_flipped_endianness_float_(input + z_) * scale_;
		}
		
		if(has_colors_) {
			pt.r = *reinterpret_cast<const std::uint8_t*>(input + r_);
			pt.g = *reinterpret_cast<const std::uint8_t*>(input + g_);
			pt.b = *reinterpret_cast<const std::uint8_t*>(input + b_);
		} else {
			pt.r = pt.g = pt.b = 255;
		}
	}

	return n;
}


ply_model::handle::handle(ply_model& mod) : model_(mod) {
	model_.open_file_(file_);
	file_.seekg(model_.vertices_offset_);
}

std::size_t ply_model::handle::read(point* buffer, std::size_t n) {
	return model_.read_points_(file_, buffer, n);
}

bool ply_model::handle::eof() {
	return (file_.tellg() >= model_.vertices_end_);
}

std::unique_ptr<model::handle> ply_model::handle::clone() {
	handle* h = new handle(model_);
	h->file_.seekg(file_.tellg());
	return std::unique_ptr<model::handle>(h);
}


}
