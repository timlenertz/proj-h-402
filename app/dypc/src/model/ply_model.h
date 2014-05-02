#ifndef DYPC_PLY_MODEL_H_
#define DYPC_PLY_MODEL_H_

#include <string>
#include <fstream>
#include "model.h"

namespace dypc {

/**
 * Model from PLY point cloud file.
 * Reads points from "vertex" elements in PLY file.
 * Coordinates must be \e float properties "x", "y", "z". Colors (if any) must be in \e uchar properties "r", "g", "b", or "red", "green", "blue". There may be other properties and elements in the file, but ASCII format, and "list" type properties are \e not supported.
 * Optimized so as to allow efficient reading.
 */
class ply_model : public model {
private:
	/**
	 * Model handle for PLY model.
	 * Contains file handle for PLY file
	 */
	class handle : public model::handle {
	private:
		ply_model& model_; ///< The ply_model instance.
		std::ifstream file_; ///< File handle.
	
	public:
		explicit handle(ply_model&);
	
		~handle() override { }
		std::size_t read(point* buffer, std::size_t n) override;
		bool eof() override;
		std::unique_ptr<model::handle> clone() override;
	};
	
	static const bool host_is_little_endian_; ///< Whether host is little-endian.

	std::string filename_; ///< Path to PLY file.
	const float scale_; ///< Amount by which to scale point coordinates.
	
	bool little_endian_; ///< Whether PLY file is encoded little endian (or big endian).
	std::size_t vertices_offset_; ///< File offset where vertices list starts.
	std::size_t vertices_end_; ///< File offset where vertices list ends.
	std::size_t vertex_length_; ///< Length ot a vertex element in file.
	bool has_colors_; ///< Whether r_, b_, g_ are defined.
	std::ptrdiff_t x_; ///< Offset of x_ property in vertex element.
	std::ptrdiff_t y_; ///< Offset of y_ property in vertex element.
	std::ptrdiff_t z_; ///< Offset of z_ property in vertex element.
	std::ptrdiff_t r_; ///< Offset of r_ property in vertex element, if has_colors.
	std::ptrdiff_t g_; ///< Offset of g_ property in vertex element, if has_colors.
	std::ptrdiff_t b_; ///< Offset of b_ property in vertex element, if has_colors.
	

	void open_file_(std::ifstream& str); ///< Open file in stream \a str.
	void read_header_(); ///< Read PLY header. Sets data members used to read data.
	
	/**
	 * Read points from file.
	 * Reads from current position of stream.
	 * @param file File stream located at read position.
	 * @param pts Output buffer.
	 * @param n Number of points to read.
	 * @return Number of points actually read.
	 */
	std::size_t read_points_(std::ifstream& file, point* pts, std::size_t n);

	/**
	 * Read float value and flip endianness.
	 * @param buf byte buffer.
	 * @return The float value. 
	 */
	static float extract_flipped_endianness_float_(const std::uint8_t* buf) {
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
	/**
	 * Create PLY model.
	 * @param filename Path to PLY file.
	 * @param scale Scale multiplier for coordinates.
	 */
	ply_model(const std::string& filename, float scale);
};

}

#endif
