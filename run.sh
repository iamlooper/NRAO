#!/system/bin/sh
# NRAO (Non root android optimizer) runner
# Author: Looper (iamlooper @ github)

BASEDIR="$(dirname "$0")"

# Check for shell permissions and exit if not granted.
[ "$(id -u)" -ne 2000 ] && {
  echo "[Error] No shell permissions. Exiting."
  exit 1
}

# Setup binaries for execution.
setup_binaries() {
  [ -e "$BASEDIR/libs" ] && {
    cp -f "$BASEDIR/libs/$1/nrao" '/data/local/tmp'
    cp -f "$BASEDIR/libs/$1/vmtouch" '/data/local/tmp'
    [ "$?" -ne 0 ] && {
      echo "[Error] Unsupported ABI. Exiting."
      exit 1
    } 
    chmod -R 0777 '/data/local/tmp'
  } || {
    echo "[Error] Binaries not found. Exiting."
    exit 1
  }
}

# Run NRAO.
run_nrao() {
  # Prepare binaries.
  setup_binaries "$(getprop ro.product.cpu.abi)"
  # Delete old logs.
  [ -f '/sdcard/Android/nrao.txt' ] && {
    rm -f '/sdcard/Android/nrao.txt'
    rm -f '/sdcard/Android/nrao_error.txt'
    rm -f '/data/local/tmp/nrao_error.txt'
  }
  # Execute NRAO.
  /data/local/tmp/nrao '/sdcard/Android/nrao.txt' '/sdcard/Android/nrao_error.txt' &
  # Show a message.
  echo "[Info] NRAO is executed in background. Check Internal Storage / Android / nrao.txt for more information."
  echo "[Info] Error log (nrao_error.txt) will appear in Internal Storage / Android after NRAO is completely executed."
  # Misc.
  echo "# Execute NRAO.
/data/local/tmp/nrao '/sdcard/Android/nrao.txt' '/sdcard/Android/nrao_error.txt' &
# Show a message.
echo \"[Info] NRAO is executed in background. Check Internal Storage / Android / nrao.txt for more information.\"
echo \"[Info] Error log (nrao_error.txt) will appear in Internal Storage / Android after NRAO is completely executed.\"" >"$HOME/../usr/bin/nrao"
  chmod 0777 "$HOME/../usr/bin/nrao"
  exit 0
}

# Main
run_nrao