#include "mp.hpp"
#include "mr/log.hpp"

#include <fstream>
#include <map>
#include <vector>

namespace {

struct _Sample {
    mp::TimeT total = 0;
    size_t num = 0;
    std::string name;
    size_t parent = 0;
};

std::map<size_t, _Sample> _total_samples; // <hash name, total time>
std::stack<mp::Sample> _samples;
const std::hash<std::string> _hash_str;

bool _FindWithoutParent(_Sample& out, size_t& out_hash) {
    for(auto const& key_value : _total_samples) {
        if(key_value.second.parent == 0) {
            out = key_value.second;
            out_hash = key_value.first;
            return true;
        }
    }
    return false;
}

void _OutSample(std::ofstream& file, _Sample const& smpl, const double total_time) {
    const double smpl_total_ms = (double)smpl.total;
    std::string parentName = "";
    if(smpl.parent != 0 && _total_samples.count(smpl.parent) != 0) parentName = _total_samples[smpl.parent].name;
    file << smpl.name;
    file << ",";
    file << parentName;
    file << ",";
    file << smpl.total;
    file << ",";
    file << ((smpl_total_ms / total_time) * 100.0);
    file << ",";
    file << smpl.num;
    file << ",";
    file << (smpl_total_ms / (double)smpl.num);
    file << std::endl;
}

bool _FindChildren(size_t top_sample_hash, std::vector<size_t>& out) {
    for(auto const& key_value : _total_samples) {
        if(key_value.second.parent == top_sample_hash) {
            out.push_back(key_value.first);
        }
    }
    return !out.empty();
}

void _RemoveSample(size_t sample_hash) {
    _total_samples.erase(sample_hash);
}

void _OutSampleTree(std::ofstream& file, size_t top_sample_hash, const double total_time) {
    _OutSample(file, _total_samples[top_sample_hash], total_time);
    std::vector<size_t> children;
    if(_FindChildren(top_sample_hash, children)) {
        for(size_t const& child_hash : children) {
            _OutSampleTree(file, child_hash, total_time);
        }
    }
    _RemoveSample(top_sample_hash);
}

}

namespace mp {

std::stack<Sample>& RefSamples() {
    return _samples;
}

bool Init() {
    return true;
}

void Shutdown() {
    //Push form stack
    while(!_samples.empty())
        MP_EndSample();

    // Calc total profiled time
    mp::TimeT total_time_ = 0;
    for(auto const& key_value : _total_samples)
        total_time_ += key_value.second.total;
    const double total_time = (double)total_time_;

    std::ofstream file("stat.csv");
    file << "name,parent,time (ms),percent,num,average";
    file << std::endl;

    // Output samples tree (from top sample, without parents; to bottom, last in tree).
    /**
        Algo:
        1. Find sample without parent, print
        2. Find sample with prev sample as parent, print
        3. Repeat 2

        Remove already printed samples, if there is only 1 children.
    **/

    // Output sample trees
    while(!_total_samples.empty()) {
        _Sample top_sample; size_t top_sample_hash = 0;
        if(!_FindWithoutParent(top_sample, top_sample_hash)) {
            break;
        }
        _OutSampleTree(file, top_sample_hash, total_time);
    }

    // Output single corrupted samples and samples without children
    for(auto const& key_value : _total_samples) {
        _OutSample(file, key_value.second, total_time);
    }

    _total_samples.clear();

    file.close();
}

void Log(Sample const& smpl, std::string const& parentName) {
    const size_t hashName = _hash_str(smpl.name);
    if(_total_samples.count(hashName) == 0) {
        _total_samples[hashName] = _Sample();
        _total_samples[hashName].name = smpl.name;
        if(!parentName.empty()) _total_samples[hashName].parent = _hash_str(parentName);
    }
    ++(_total_samples[hashName].num);
    _total_samples[hashName].total += smpl.time;
}

std::mutex& GlobalMtx() {
    static std::mutex mtx;
    return mtx;
}

}
