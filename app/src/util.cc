#include "util.h"
#include <cstdio>
#include <string>
#include <memory>
#include <wx/msgdlg.h>
#include <wx/choicdlg.h>

namespace dypc {

std::string file_path_extension(const std::string& path) {
	auto pos = path.rfind('.');
	if(pos == std::string::npos) return "";
	else return path.substr(pos + 1);
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

std::string user_choice(const user_choices_t& choices, const std::string& caption) {	
	size_t n = choices.size();
	std::unique_ptr<wxString[]> labels(new wxString [n]);
	std::unique_ptr<std::string[]> names(new std::string [n]);
	
	std::ptrdiff_t i = 0;
	for(const auto& it : choices) {
		labels[i] = wxString(it.second.c_str(), wxConvUTF8);
		names[i++] = it.first;
	}
	
	wxSingleChoiceDialog dialog(
		nullptr,
		wxString(caption.c_str(), wxConvUTF8),
		wxT("Choice"),
		n,
		labels.get()
	);
	auto result = dialog.ShowModal();
	if(result == wxID_CANCEL) return "";

	i = dialog.GetSelection();
	if(i < 0 || i >= n) return "";
	else return names[i];
}


std::string file_formats_to_wildcard(const user_choices_t& file_formats) {
	std::string wildcard;
	for(const auto& p : file_formats) {
		if(wildcard != "") wildcard += "|";
		wildcard += p.second + " (*." + p.first + ")|*." + p.first;
	}
	return wildcard;
}

}

