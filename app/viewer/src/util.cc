#include "util.h"
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>
#include <cstdio>
#include <stdexcept>
#include <png.h>

namespace dypc {

std::ptrdiff_t user_choice(const std::vector<std::string>& choices, const std::string& caption) {
	std::vector<wxString> labels;
	for(const std::string& str : choices) labels.emplace_back(str.c_str(), wxConvUTF8);
	
	wxSingleChoiceDialog dialog(
		nullptr,
		wxString(caption.c_str(), wxConvUTF8),
		wxT("Choice"),
		labels.size(),
		labels.data()
	);
	auto result = dialog.ShowModal();
	if(result == wxID_CANCEL) return -1;

	std::ptrdiff_t i = dialog.GetSelection();
	if(i < 0 || i >= choices.size()) return -1;
	else return i;
}

void error_message(const std::string& msg, const std::string& title) {
	wxMessageDialog dialog(
		nullptr,
		wxString(msg.c_str(), wxConvUTF8),
		wxString(title.c_str(), wxConvUTF8),
		wxOK | wxICON_ERROR
	);
	dialog.ShowModal();
}

std::string file_size_to_string(std::size_t sz) {
	const double k = 1024;
	const double M = k*1024;
	const double G = M*1024;
	
	if(sz >= G) return float_to_string((double)sz/G) + " GiB";
	else if(sz >= M) return float_to_string((double)sz/M) + " MiB";
	else if(sz >= k) return float_to_string((double)sz/k) + " kiB";
	else return std::to_string(sz) + " B";
}

std::string time_to_string(std::chrono::milliseconds dur) {
	long ms = dur.count();
	if(ms >= 1000) return float_to_string((double)ms/1000) + " s";
	else return std::to_string(ms) + " ms";
}

std::string float_to_string(double f, std::size_t decimal_digits) {
	char decimal[256];
	std::string format = std::string("%.") + std::to_string(decimal_digits) + "f";
	std::snprintf(decimal, sizeof(decimal), format.c_str(), f);
	return std::string(decimal);
}


std::string file_path_extension(const std::string& path) {
	auto pos = path.rfind('.');
	if(pos == std::string::npos) return "";
	else return path.substr(pos + 1);
}

std::string file_formats_to_wildcard(const std::map<std::string, std::string>& file_formats) {
	std::string wildcard;
	for(const auto& p : file_formats) {
		if(wildcard != "") wildcard += "|";
		wildcard += p.second + " (*." + p.first + ")|*." + p.first;
	}
	return wildcard;
}

void write_to_png(std::uint8_t* data, const std::string& filename, std::size_t width, std::size_t height) {
	FILE* fp = std::fopen(filename.c_str(), "wb");
	if(! fp) throw std::runtime_error("Could not open file for writing");

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if(! png_ptr) {
		std::fclose(fp);
		throw std::runtime_error("Could not allocate write struct");
	}
	
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(! info_ptr) {
		std::fclose(fp);
		png_destroy_write_struct(&png_ptr, nullptr);
		throw std::runtime_error("Could not allocate info struct");
	}
	
	if(setjmp(png_jmpbuf(png_ptr))) {
		std::fclose(fp);
		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
		png_destroy_write_struct(&png_ptr, nullptr);
		throw std::runtime_error("Error during PNG creating");
	}

	png_init_io(png_ptr, fp);
	
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	std::size_t row_length = width * 3;
	std::size_t data_length = row_length * height;
	const std::uint8_t* data_end = data + data_length;
	while(data < data_end) {
		png_write_row(png_ptr, (png_bytep)data);
		data += row_length;
	}
	
	png_write_end(png_ptr, nullptr);
	
	std::fclose(fp);
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct(&png_ptr, nullptr);
}


}
