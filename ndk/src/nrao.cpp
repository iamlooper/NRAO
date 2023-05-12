#include <iostream>
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

  // Print tweak completion message.  
  xlog("date", "Preloaded important system items into RAM");  
}

// Tweak to disable debugging, statistics & unnecessary background apps.
void system_settings_tweak() {
  // List of shell commands to execute.
  vector<string> cmds = {
    "settings put system anr_debugging_mechanism 0",
    "settings put global fstrim_mandatory_interval 3600",
    "cmd looper_stats disable",
    "cmd appops set com.android.backupconfirm RUN_IN_BACKGROUND ignore",
    "cmd appops set com.google.android.setupwizard RUN_IN_BACKGROUND ignore",
    "cmd appops set com.android.printservice.recommendation RUN_IN_BACKGROUND ignore",
    "cmd appops set com.android.onetimeinitializer RUN_IN_BACKGROUND ignore",
    "cmd appops set com.qualcomm.qti.perfdump RUN_IN_BACKGROUND ignore",
    "cmd power set-fixed-performance-mode-enabled true",
    "am idle-maintenance"
  };
  
  // Iterate through the list of commands.
  for (const string& cmd : cmds) {
    // Use exec_shell() function to execute shell command.
    exec_shell(cmd, false);  
  }  

  // Print tweak completion message.
  xlog("date", "Tweaked settings provider tunables");
}

// Tweak to improve main DEX files.
void main_dex_tweak() {
  // Build command to execute.
  string cmd = "cmd package compile -m speed-profile -a";

  // Use exec_shell() function to execute shell command.
  exec_shell(cmd, false);   

  // Print tweak completion message.  
  xlog("date", "Applied main DEX tweak");  
}

// Tweak to improve secondary DEX files.
void secondary_dex_tweak() {
  // List of shell commands to execute.
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

  // Print tweak completion message.
  xlog("date", "Applied secondary DEX tweak");  
}

void apply_all() {
  xlog("info", "Started NRAO Tweaks at " + print_date("full"));
  xlog("", "");
  
  mem_preload_tweak();
  system_settings_tweak();
  main_dex_tweak();  
  secondary_dex_tweak();

  xlog("", "");
  xlog("info", "Completed NRAO Tweaks at " + print_date("full"));
}

int main(int argc, char *argv[]) {        
  // Write all modified data to disk.
  sync();

  // Apply all.
  apply_all();
  
  // Write all modified data to disk.
  sync();    
        
  return 0;
}