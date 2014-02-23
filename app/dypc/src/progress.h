#ifndef DYPC_PROGRESS_H_
#define DYPC_PROGRESS_H_

#include <functional>
#include <string>


namespace dypc {

void set_progress(int value);
void increment_progress(int add = 1);
void progress(const std::string& label, int maximum, int update_step, const std::function<void()>&);

inline void progress(const std::string& label, int maximum, const std::function<void()>& callback) {
	progress(label, maximum, maximum/1000, callback);
}

}

#endif
