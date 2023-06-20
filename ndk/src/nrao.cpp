#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>

#include "util_functions.hpp"

using namespace std;

// Declare functions.
void mem_preload_tweak();
void cmd_services_tweak();
void main_dex_tweak();
void secondary_dex_tweak();
void apply_all_tweaks();

// Tweak to preload important system objects into memory.
void mem_preload_tweak() {
  vector<string> apex_objects = {
    "linker",
    "linker64",
    "libm.so",
    "libdl.so"
  };

  vector<string> bin_objects = {
    "dalvikvm",
    "app_process"
  };

  vector<string> lib_objects = {     
    "libandroid.so",        
    "libandroid_runtime.so",
    "libandroid_servers.so",    
    "libEGL.so", 
    "libGLESv1_CM.so",      
    "libGLESv2.so", 
    "libGLESv3.so",       
    "libsurfaceflinger.so",    
    "libhwui.so",
    "libui.so",
    "libskia.so",     
    "libvulkan.so", 
    "libRScpp.so",
    "libinputflinger.so",  
    "libharfbuzz_ng.so",  
    "libprotobuf-cpp-full.so", 
    "libprotobuf-cpp-lite.so", 
    "libblas.so",
    "libutils.so"    
  };

  vector<string> framework_objects = {
    "framework.jar",
    "services.jar",
    "protobuf.jar"
  };

  vector<string> pkg_names = {
    "com.android.systemui",
    get_home_pkg_name(),
    get_ime_pkg_name()
  };   

  for (const string& apex_object : apex_objects) {
    if (is_path_exists("/apex/com.android.runtime/bin/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/bin/" + apex_object);
    } 
    if (is_path_exists("/apex/com.android.runtime/lib/bionic/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/lib/bionic/" + apex_object);
    } 
    if (is_path_exists("/apex/com.android.runtime/lib64/bionic/" + apex_object)) {
      preload_item("obj", "/apex/com.android.runtime/lib64/bionic/" + apex_object);
    }         
  }

  for (const string& bin_object : bin_objects) {
    if (is_path_exists("/bin/" + bin_object)) {
      preload_item("obj", "/bin/" + bin_object);
    }    
  }  

  for (const string& lib_object : lib_objects) {
    if (is_path_exists("/system/lib/" + lib_object)) {
      preload_item("obj", "/system/lib/" + lib_object);
    } 
    if (is_path_exists("/system/lib64/" + lib_object)) {
      preload_item("obj", "/system/lib64/" + lib_object);    
    } 
  }

  for (const string& framework_object : framework_objects) {
    if (is_path_exists("/system/framework/" + framework_object)) {
      preload_item("obj", "/system/framework/" + framework_object);
    }    
  }  

  for (const string& pkg_name : pkg_names) {
    preload_item("app", pkg_name);
  }

  xlog("date", "Preloaded important system items into RAM.");  
}

// Tweak to disable debugging, statistics & unnecessary background apps etc.
void cmd_services_tweak() {
  vector<string> svc_cmds = {
    "settings put system anr_debugging_mechanism 0",
    "settings put global fstrim_mandatory_interval 3600",
    "looper_stats disable",
    "appops set com.android.backupconfirm RUN_IN_BACKGROUND ignore",
    "appops set com.google.android.setupwizard RUN_IN_BACKGROUND ignore",
    "appops set com.android.printservice.recommendation RUN_IN_BACKGROUND ignore",
    "appops set com.android.onetimeinitializer RUN_IN_BACKGROUND ignore",
    "appops set com.qualcomm.qti.perfdump RUN_IN_BACKGROUND ignore",
    "power set-fixed-performance-mode-enabled true",
    "activity idle-maintenance",
    "thermalservice override-status 1"
  };
  
  for (const string& svc_cmd : svc_cmds) {
    exec_shell("cmd " + svc_cmd, false);  
  }  

  xlog("date", "Tweaked `cmd` services.");
}

// Tweak to improve main DEX files.
void main_dex_tweak() {
  string cmd = "cmd package compile -m speed-profile -a";
  exec_shell(cmd, false);   
  xlog("date", "Applied main DEX tweak.");  
}

// Tweak to improve secondary DEX files.
void secondary_dex_tweak() {
  vector<string> cmds = {
    "pm compile -m speed-profile --secondary-dex -a", 
    "pm reconcile-secondary-dex-files -a", 
    "pm compile --compile-layouts -a"
  };

  for (const string& cmd : cmds) {
    exec_shell(cmd, false);  
  }

  xlog("date", "Applied secondary DEX tweak.");  
}

void apply_all_tweaks() {
  xlog("info", "Started NRAO Tweaks at " + print_date("full"));
  xlog("", "");
  
  mem_preload_tweak();
  cmd_services_tweak();
  main_dex_tweak();  
  secondary_dex_tweak();

  xlog("", "");
  xlog("info", "Completed NRAO Tweaks at " + print_date("full"));
}

int main(int argc, char *argv[]) {        
  sync();
  apply_all_tweaks();
  sync();    
        
  return 0;
}
