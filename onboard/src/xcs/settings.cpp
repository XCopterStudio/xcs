#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>
#include "settings.hpp"

using namespace std;
using namespace xcs;

Settings::Settings(const string& filename, bool create) :
    filename_(filename) {
    reset(create);
}

Settings::~Settings() {
}

void Settings::reset(bool create) {
    if (isInit()) {
        if (create) {
            boost::filesystem::path p(filename_);
            if (!boost::filesystem::exists(p)) {
                boost::filesystem::create_directories(p.parent_path());
                ofstream f(p.string(), ios::app);
                if (f.is_open()) {
                    f.close();
                }
            }
        }

        boost::property_tree::info_parser::read_info(filename_, settings_);
    }
}

void Settings::reset(const string& filename, bool create) {
    filename_ = filename;
    reset(create);
}

bool Settings::save() {
    if (isInit()) {
        boost::property_tree::info_parser::write_info(filename_, settings_);
        return true;
    }

    return false;            
}

boost::property_tree::ptree& Settings::getTree() {
    return settings_;
}

bool Settings::isInit() const {
    return !filename_.empty();
}

bool Settings::contains(const string& key) const {
    //return settings_.count(key) > 0;
    //return settings_.find(key) != settings_.not_found();
    
    //TODO: use somethig faster then exception?
    try  {
        settings_.get_child(key);
        return true;
    }
    catch (boost::property_tree::ptree_bad_path ex) {
        return false;
    }
}

list<string> Settings::getSettingsFiles(string foldername, const string& key) {
    list<string> result;

    boost::filesystem::path p(foldername);
    if (boost::filesystem::exists(p) && boost::filesystem::is_directory(p)) {
        boost::filesystem::directory_iterator end_it;
        for (boost::filesystem::directory_iterator it(p); it != end_it; ++it) {
            if (boost::filesystem::is_regular_file(it->status())) {
                try {
                    Settings s(it->path().string());
                    if (key == "" || s.contains(key)) {
                        result.push_back(it->path().filename().string());
                    }
                }
                catch(...) {
                }
            }
        }
    }

    return result;
}