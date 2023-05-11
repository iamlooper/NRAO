#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <unistd.h>

#include "util_functions.hpp"

using namespace std;

// Declare functions.
void mem_preload_tweak();
void system_settings_tweak();
void main_dex_tweak();
void secondary_dex_tweak();
void apply_all();

// Tweak to preload important system objects into memory.
void mem_preload_tweak() {
  vector<string> apex_objects = {
    "/apex/com.android.runtime/bin/linker",
    "/apex/com.android.runtime/bin/linker64",
    "/apex/com.android.runtime/lib/bionic/libc.so",
    "/apex/com.android.runtime/lib/bionic/libm.so",
    "/apex/com.android.runtime/lib/bionic/libdl.so",
    "/apex/com.android.runtime/lib64/bionic/libc.so",
    "/apex/com.android.runtime/lib64/bionic/libm.so",
    "/apex/com.android.runtime/lib64/bionic/libdl.so"
  };

  for (const string& apex_object : apex_objects) {
    if (is_path_exists(apex_object)) {
      preload_item("obj", apex_object);
    }
  }

  vector<string> system_objects = {
    "/bin/linker",
    "/bin/linker64",
    "/system/lib/libc++.so",
    "/system/lib/libgui.so",
    "/system/lib/libsurfaceflinger.so",
    "/system/lib/libandroid.so",
    "/system/lib/libinput.so",   
    "/system/lib64/libc++.so",
    "/system/lib64/libgui.so",
    "/system/lib64/libsurfaceflinger.so",
    "/system/lib64/libandroid.so",    
    "/system/lib64/libinput.so"
  };

  for (const string& system_object : system_objects) {
    if (is_path_exists(system_object)) {
      preload_item("obj", system_object);
    }    
  }

  vector<string> oat_objects = {
    "/system/framework/arm/boot-framework.oat",
    "/system/framework/arm64/boot-framework.oat"
  };

  for (const string& oat_object : oat_objects) {
    if (is_path_exists(oat_object)) {
      preload_item("obj", oat_object);
    }    
  }
  
  vector<string> apks = {
    "com.android.systemui",
    get_home_pkgname(),
    get_ime_pkgname()
  }; 
  
  for (const string& apk : apks) {
    preload_item("dex", apk);
  }
}

// Tweak to disable debugging, statistics & unnecessary background apps.
void system_settings_tweak() {
  // List of commands.
  vector<string> cmds = {
    "settings put system anr_debugging_mechanism 0",
    "cmd looper_stats disable",
    "cmd appops set com.android.backupconfirm RUN_IN_BACKGROUND ignore",
    "cmd appops set com.google.android.setupwizard RUN_IN_BACKGROUND ignore",
    "cmd appops set com.android.printservice.recommendation RUN_IN_BACKGROUND ignore",
    "cmd appops set com.android.onetimeinitializer RUN_IN_BACKGROUND ignore",
    "cmd appops set com.qualcomm.qti.perfdump RUN_IN_BACKGROUND ignore"    
  };
  
  // Iterate through the list of commands.
  for (const string& cmd : cmds) {
    // Use exec_shell() function to execute shell command.
    exec_shell(cmd, false);  
  }  
}

// Tweak to improve main DEX files.
void main_dex_tweak() {
  // Build command to execute.
  string cmd = "cmd package compile -m speed-profile -a";

  // Use exec_shell() function to execute shell command.
  exec_shell(cmd, false);   
}

// Tweak to improve secondary DEX files.
void secondary_dex_tweak() {
  // List of commands.
  vector<string> cmds = {
    "pm compile -m speed-profile --secondary-dex -a", 
    "pm reconcile-secondary-dex-files -a", 
    "pm compile --compile-layouts -a"
  };

  // Iterate through the list of commands.
  for (const string& cmd : cmds) {
    // Use exec_shell() function to execute shell command.
    exec_shell(cmd, false);  
  }
}

void apply_all() {
  // Write all modified data to disk.
  sync();

  xlog("info", "Started NRAO Tweaks at " + print_date("full"));
  xlog("", "");
  
  mem_preload_tweak();
  xlog("date", "Preloaded important system items into RAM");
 
  system_settings_tweak();
  xlog("date", "Tweaked settings provider tunables");

  main_dex_tweak();
  xlog("date", "Applied main DEX tweak");
  
  secondary_dex_tweak();
  xlog("date", "Applied secondary DEX tweak");

  xlog("", "");
  xlog("info", "Completed NRAO Tweaks at " + print_date("full"));
  
  // Write all modified data to disk.
  sync();  
}

int main(int argc, char *argv[]) {  
  // Redirect standard output & error depending upon the given arguments.
  if (argc > 2) { 
    // Redirect standard output to given log path.
    freopen(argv[1], "w", stdout);
    // Redirect standard error to tmp dir for now due to a bug.
    freopen("/data/local/tmp/nrao_error.txt", "w", stderr);
  } else if (argc > 1) {
    // Redirect standard output to given log path.
    freopen(argv[1], "w", stdout);
  }
      
  // Apply all.
  apply_all();
  
  // Move error log from tmp dir to given path.
  if (is_path_exists("/data/local/tmp/nrao_error.txt")) {
    exec_shell("mv -f \'/data/local/tmp/nrao_error.txt\' " + string(argv[2]), false);
  }
      
  return 0;
}