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
  xlog("date", "Preloaded important system items into RAM.");  
}

// Tweak to disable debugging, statistics & unnecessary background apps etc.
void cmd_services_tweak() {
  // List of service commands of `cmd` to execute.
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
  
  // Iterate through the list of service commands.
  for (const string& svc_cmd : svc_cmds) {
    // Use exec_shell() function to execute shell command.
    exec_shell("cmd " + svc_cmd, false);  
  }  

  // Print tweak completion message.
  xlog("date", "Tweaked `cmd` services.");
}

// Tweak to improve main DEX files.
void main_dex_tweak() {
  // Build command to execute.
  string cmd = "cmd package compile -m speed-profile -a";

  // Use exec_shell() function to execute shell command.
  exec_shell(cmd, false);   

  // Print tweak completion message.  
  xlog("date", "Applied main DEX tweak.");  
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
  // Write all modified data to disk.
  sync();

  // Apply all tweaks.
  apply_all_tweaks();
  
  // Write all modified data to disk.
  sync();    
        
  return 0;
}